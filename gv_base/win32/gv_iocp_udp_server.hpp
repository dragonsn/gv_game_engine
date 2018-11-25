//-----------------------------------------------------------------------------

#include "gv_iocp_udp_server.h"

namespace gv
{

//-----------------------------------------------------------------------------
const int GV_UDP_MAX_MTU = gvc_default_mtu;
//-----------------------------------------------------------------------------
const BYTE HPS_OPERATION_READ = 0;  // WSARecvFrom event
const BYTE HPS_OPERATION_WRITE = 1; // WSASendTo event
const int HPS_OVERLAPPED_BUFFER_SIZE = GV_UDP_MAX_MTU;

VOID CALLBACK ProcessEventWrapper(ULONG_PTR param);

struct UDPOverlappedEvent
{
	OVERLAPPED overlapped;
	gv_socket_address address;
	int addressLength;
	BYTE operation;
	gv_packet* owner;
};

#if 0
#define DEBUG_DUMP_EVENT(e)                                           \
	GVM_DEBUG_LOG(net, "[IOCP][EVENT] " << (void*)e << ":"            \
										<< e->address.string() << ":" \
										<< e->operation << gv_endl);
#else
#define DEBUG_DUMP_EVENT(e)
#endif

//-----------------------------------------------------------------------------
class gv_iocp_udp_server_impl; // Forward declare
struct APCEvent
{
	GVM_OVERRIDE_NEW;
	gv_iocp_udp_server_impl* server;
	UDPOverlappedEvent* event;
};
GVM_IMP_POOLED(APCEvent, 32);
//-----------------------------------------------------------------------------

// gvt_lock_free_pool<gvt_pad<sizeof(APCEvent )>,1024> s_pool_apc_event;
// typedef boost::singleton_pool<gv_packet, sizeof(gv_packet)> s_pool_packet;

gv_atomic_count packet_number;
//-----------------------------------------------------------------------------
class iocp_udp_packet_factory : public gvt_factory< gv_packet >
{
public:
	gv_packet* create()
	{
		gv_packet* p;
		p = new gv_packet;
		p->set_user_data(UDPOverlappedEvent(), false);
		UDPOverlappedEvent* event = p->get_user_data< UDPOverlappedEvent >();
		// This is mandatory! We can cause crashes if we
		// forget this due to how this object is used internally.
		memset(&event->overlapped, 0, sizeof(OVERLAPPED));
		event->addressLength = sizeof(gv_socket_address);
		event->owner = p;
		event->operation = 255;
		++packet_number;
		return p;
	};

	void destroy(gv_packet* p)
	{
		p->dec_ref();
		--packet_number;
	};
};
static iocp_udp_packet_factory s_packet_factory;

class gv_iocp_udp_server_impl
{
private:
	gv_ushort port;
	HANDLE completionPort;
	gv_uint numberOfConcurrentThreads;
	gv_uint numberOfWorkerThreads;
	gv_bool socket_killed;
	gv_socket server_socket;
	gv_socket_address internetAddr;
	// These variables control how many posted overlapped
	// receives we can have at a time.
	LONG maxPendingRecvs;
	LONG curPendingRecvs;
	LONG refillPendingRecvs;
	// An event to signal when we should refill the posted
	// pending overlapped receive queue.
	gv_event RefillEvent;
	// Our event processing thread handle
	gv_thread APCEventProcessingThread;
	gvt_runnable< gv_iocp_udp_server_impl > runable_APCEventProcessing;

	// An array of worker thread handles to help us
	// clean up when the server needs to exit
	gv_thread* workerThreads;
	gvt_runnable< gv_iocp_udp_server_impl > runable_WorkThread;

	// The gv_iocp_udp_server object that owns this object
	gv_iocp_udp_server* parent;
	bool m_apc_enabled;

	// A cache of addresses to make sending to an address more
	// efficient in the long run.

public:
	// This is our function pointer to an external function that
	// will handle client to server data. We only make this public
	// so we don't have to 'friend' the gv_iocp_udp_server
	// class to this class. Since this class is not exposed to
	// anyone else, it is 'ok'.
	gv_callback_on_recv_complete OnClientToServer;
	gv_callback_on_io_error OnIOError;

public:
	gv_iocp_udp_server_impl(gv_iocp_udp_server* parent_);
	~gv_iocp_udp_server_impl();

	// create the UDP server. Returns true on success and false on failure.
	bool create(gv_ushort port_, gv_uint numberOfConcurrentThreads_,
				gv_uint numberOfWorkerThreads_, LONG maxPendingRecvs_,
				LONG refillPendingRecvs_);

	// Close down the server and free internally used resources.
	void destroy();

	// Dispatches the UDP event (one buffer at a time)
	bool Send(gv_packet* event);

	bool send_to(const gv_socket_address& address, gv_int count, UINT8* data);

	void ProcessEvent(gv_packet* packet);
	void APCEventProcessThread();
	void WorkerThread();

	gv_packet* create_packet()
	{
		gv_packet* p = s_packet_factory.create();
		return p;
	};

	void destroy_packet(gv_packet* p)
	{
		if (p->get_ref())
			p->dec_ref();
		else
			return s_packet_factory.destroy(p);
	};

	void enable_apc_call(bool enable)
	{
		m_apc_enabled = enable;
	};
	APCEvent* create_apc_event()
	{
		return new APCEvent;
	};
	void destroy_apc_event(APCEvent* p)
	{
		delete p;
	}
};
//-----------------------------------------------------------------------------
// Default function place holder
void WINAPI DefaultOnClientToServer(gvi_server_transport_layer_udp* server,
									gv_socket_address& address, gv_packet*);
void WINAPI DefaultOnIOError(class gvi_server_transport_layer_udp* server,
							 gv_socket_address& address, gv_packet*,
							 gv_byte operation, gv_uint error_code){

};
//-----------------------------------------------------------------------------

gv_iocp_udp_server_impl::gv_iocp_udp_server_impl(gv_iocp_udp_server* parent_)
	: RefillEvent(false), APCEventProcessingThread(gv_id_server_udp_iocp_APC)
{
	runable_APCEventProcessing.init(
		this, &gv_iocp_udp_server_impl::APCEventProcessThread);
	runable_WorkThread.init(this, &gv_iocp_udp_server_impl::WorkerThread);
	port = 0;
	completionPort = INVALID_HANDLE_VALUE;
	numberOfConcurrentThreads = 0;
	numberOfWorkerThreads = 0;
	workerThreads = NULL;
	maxPendingRecvs = 0;
	refillPendingRecvs = 0;
	curPendingRecvs = 0;
	OnClientToServer = DefaultOnClientToServer;
	OnIOError = DefaultOnIOError;
	parent = parent_;
	m_apc_enabled = true;
	socket_killed = false;
}

//-----------------------------------------------------------------------------
gv_iocp_udp_server_impl::~gv_iocp_udp_server_impl()
{
}
//-----------------------------------------------------------------------------

bool gv_iocp_udp_server_impl::create(gv_ushort port_,
									 gv_uint numberOfConcurrentThreads_,
									 gv_uint numberOfWorkerThreads_,
									 LONG maxPendingRecvs_,
									 LONG refillPendingRecvs_)
{
	// Get the system information
	GV_PROFILE_EVENT_SLOW(gv_iocp_udp_server_create)
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);

	port = port_;
	numberOfConcurrentThreads = numberOfConcurrentThreads_;
	numberOfWorkerThreads = numberOfWorkerThreads_;
	maxPendingRecvs = maxPendingRecvs_;
	refillPendingRecvs = refillPendingRecvs_;

	// Try to create an I/O completion port
	completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0,
											numberOfConcurrentThreads);
	if (completionPort == NULL)
	{
		GVM_DEBUG_LOG(net,
					  "[IOCP]Could not create the IOCP. GetLastError returned [%i]"
						  << (int)GetLastError() << "\r\n");
		destroy();
		return false;
	}

	// Calculate how many worker threads we should create to process IOCP events
	if (numberOfWorkerThreads == 0)
	{
		if (numberOfConcurrentThreads == 0)
		{
			numberOfWorkerThreads = SystemInfo.dwNumberOfProcessors * 2;
		}
		else
		{
			numberOfWorkerThreads = numberOfConcurrentThreads * 2;
		}
	}

	// create an array of thread information objects. Since we only will be
	// creating
	// a handful of these only once, there is no need to make a pool of data;
	// using
	// new is ok.
	workerThreads = new gv_thread[numberOfWorkerThreads];
	// Setup our overlapped UDP socket
	// socket_ = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0,
	// WSA_FLAG_OVERLAPPED);
	server_socket.init_socket(e_socket_type_datagram, e_socket_proto_udp, true);

	// Allow LAN based broadcasts since this option is disabled by default.
	// int value = 1;
	// setsockopt(socket_, SOL_SOCKET, SO_BROADCAST, (char *)&value, sizeof(int));
	server_socket.opt_set_broadcast(true);
	server_socket.opt_set_receive_buffer_size(1024 * 1024);
	// server_socket.opt_set_blocking(false);

	// Bind the socket to the requested port
	internetAddr = gv_socket_address(gv_ip_address_ipv4::addr_wild_card(), port);
	// int bindResult	=bind(socket_, internetAddr.system_socket_addr(),
	// sizeof(internetAddr));
	if (port != 0)
		server_socket.bind(internetAddr);

	// Connect the listener socket to IOCP
	if (CreateIoCompletionPort(
			reinterpret_cast< HANDLE >(server_socket.get_socket_handle()),
			completionPort, 0, numberOfConcurrentThreads) == 0)
	{
		GVM_DEBUG_LOG(net, "[IOCP]Could not assign the socket to the IOCP handle. "
						   "GetLastError returned [%i]"
							   << (int)GetLastError() << "\r\n");
		GV_ASSERT(0);
		destroy();
		return false;
	}

	// Set automatic detection of pending overlapped receives. To do this we check
	// the size of
	// a pointer. If the pointer is 8 bytes, we are in 64-bit Windows and can have
	// 32,767
	// pending receives. Otherwise, we are in 32-bit Windows and we can only have
	// 2500. This
	// is a bit of a hack for now, but it should work reliably.
	if (maxPendingRecvs == 0)
		maxPendingRecvs = (sizeof(UINT8*) == 8 ? /*32767*/ 2500 : 2500);
	if (refillPendingRecvs == 0)
		refillPendingRecvs = static_cast< LONG >(.5 * maxPendingRecvs);

	// This is our main event processing thread that we use to handle APC events
	// that hold network data.
	//= CreateThread(0, 0, APCEventProcessThreadWrapper, this, 0, 0);
	APCEventProcessingThread.start(&runable_APCEventProcessing);
	// create all of our worker threads
	for (gv_uint x = 0; x < numberOfWorkerThreads; ++x)
	{
		// We only create in the suspended state so our workerThreadHandles object
		// is updated before the thread runs.
		// CreateThread(0, 0, WorkerThreadWrapper, this, CREATE_SUSPENDED,
		// &workerThreadHandles[x].dwThreadId);
		gv_string_tmp temp("server_udp_iocp_worker_");
		temp << x;
		workerThreads[x].set_name(gv_id(*temp));
		workerThreads[x].start(&runable_WorkThread, true);
		workerThreads[x].resume_thread();
	}
	return true;
}

//-----------------------------------------------------------------------------

void gv_iocp_udp_server_impl::destroy()
{
	GV_PROFILE_EVENT_SLOW(gv_iocp_udp_server_destroy)
	if (!workerThreads)
		return;
	// Clean up the worker threads waiting on the IOCP
	// If we have a socket to cleanup, do so. We close this here to force our UDP
	// processing thread to generate an error when it uses the socket again.
	socket_killed = true;
	server_socket.close();
	gv_thread::sleep(1500);
	if (completionPort != INVALID_HANDLE_VALUE)
	{
		GV_PROFILE_EVENT_SLOW(gv_iocp_udp_server_post_exit_events)
		// destroy all of our worker threads by posting an IOCP event that signals
		// the threads
		// to exit. There is no other way around having to do this that is more
		// efficient!
		for (gv_uint x = 0; x < numberOfWorkerThreads; ++x)
		{
			GV_PROFILE_EVENT_SLOW(gv_iocp_udp_server_post_exit_event)
			PostQueuedCompletionStatus(completionPort, 0, static_cast< ULONG_PTR >(-1),
									   0);
			Sleep(0); // Try to give up our time slice so the other threads can exit
		}
	}
	// If we have worker threads allocated
	if (workerThreads)
	{
		GV_PROFILE_EVENT_SLOW(gv_iocp_udp_server_kill_works)
		// Wait for all worker threads to close
		for (gv_uint x = 0; x < numberOfWorkerThreads; x++)
		{
			// Arbitrarily wait a second, it should never take more than a couple ms
			// if
			// that, but if it does timeout, something is wrong. We can terminate the
			// thread forcefully safely.
			workerThreads[x].join();
			/*
      bool result=workerThreads[x].try_join(5000);
      if(!result )
      {
              workerThreads[x].kill_thread();
      }*/
		}
		delete[] workerThreads;
		workerThreads = 0;
	}

	if (completionPort != INVALID_HANDLE_VALUE)
	{
		CloseHandle(completionPort);
		completionPort = INVALID_HANDLE_VALUE;
	}

	// We clean up this last so we can give our socket time to close

	// Set the refill event to trigger the event processing thread out of
	// it's waiting loop. We resort to the socket closing logic and
	// error generation to avoid checking a flag each loop to exit.
	{
		GV_PROFILE_EVENT_SLOW(gv_iocp_udp_server_end_APC_thread)
		RefillEvent.set();
		APCEventProcessingThread.join();
		/*
    if(!APCEventProcessingThread.try_join(5000))
    {
            APCEventProcessingThread.kill_thread();
    }*/
	}
}

//-----------------------------------------------------------------------------
bool gv_iocp_udp_server_impl::Send(gv_packet* packet)
{
	// This is not used in our function, but we will have lower layer memory
	// exceptions raised
	// if we don't pass in an address!
	GV_PROFILE_EVENT_SLOW(gv_iocp_udp_server_Send);
	packet->inc_ref();
	DWORD dwSent = 0;
	UDPOverlappedEvent* event = packet->get_user_data< UDPOverlappedEvent >();
	WSABUF sendBufferDescriptor;
	sendBufferDescriptor.buf = packet->get_network_diagram_start();
	sendBufferDescriptor.len = packet->get_network_diagram_size();
	packet->m_time_stamp = gv_global::time->time_stamp_unix();
	// Send the packet! If all goes well, we will get an IOCP notification when
	// it's sent
	int result =
		WSASendTo(server_socket.get_socket_handle(), &sendBufferDescriptor, 1,
				  &dwSent, 0, reinterpret_cast< sockaddr* >(&event->address),
				  event->addressLength, &event->overlapped, NULL);
	if (result == SOCKET_ERROR) // We don't expect this, but...
	{
		result = WSAGetLastError();
		if (result != WSA_IO_PENDING) // This means everything went well
		{
			GVM_DEBUG_LOG(net, "[IOCP]WSASendTo failed. WSAGetLastError returned "
								   << (int)WSAGetLastError() << "\r\n");
			this->destroy_packet(packet);
			return false;
		}
	}
	return true;
}

//-----------------------------------------------------------------------------

bool gv_iocp_udp_server_impl::send_to(const gv_socket_address& address,
									  gv_int count, UINT8* data)
{
	// Make sure we do not overflow the buffer
	GV_PROFILE_EVENT_SLOW(gv_iocp_udp_server_sendto);

	GV_ASSERT(count <= HPS_OVERLAPPED_BUFFER_SIZE);

	// create our event object
	gv_packet* packet = create_packet();
	UDPOverlappedEvent* event = packet->get_user_data< UDPOverlappedEvent >();
	event->address = address;
	event->operation = HPS_OPERATION_WRITE;
	packet->write(data, count);
	// Finally we can dispatch it
	DEBUG_DUMP_EVENT(event);
	return Send(packet);
}

//-----------------------------------------------------------------------------

// This function will process an event sent from a client to the server. It
// is only to be invoked from the ProcessEventWrapper function, which is turn
// is only invoked from the APCEventProcessThread function. You may not call
// this function yourself as it would break the thread-safe guarantee.
void gv_iocp_udp_server_impl::ProcessEvent(gv_packet* packet)
{
	// Dispatch the event to the external function.
	GV_PROFILE_EVENT_SLOW(gv_iocp_udp_server_impl_ProcessEvent);
	UDPOverlappedEvent* event = packet->get_user_data< UDPOverlappedEvent >();
	OnClientToServer(parent, event->address, packet);
	destroy_packet(packet);
}

//-----------------------------------------------------------------------------
// This function will loop through dispatching APC events as they are posted
// to this threads APC event queue. It will also keep overlapped events posted
// as needed.
void gv_iocp_udp_server_impl::APCEventProcessThread()
{
	// Start out by signaling that we need to fill the pending overlapped receive
	// buffer
	// SetEvent(hRefillEvent);
	RefillEvent.set();

	// Loop forever until we need to exit the thread. When we do need to exit, a
	// socket
	// error will force a return from the function.
	for (;;)
	{
		// Wait until we need to refill the pending receive queue or we have
		// and APC to handle.
		// if(RefillEvent, INFINITE, TRUE) == WAIT_OBJECT_0)
		RefillEvent.wait();
		{
			// Store how many new overlapped reads we need to post. Note we always
			// want to try
			// and post at least one event so we can detect when we need to exit. This
			// is a more
			// efficient way of detecting exit rather than having to always check
			// another event.
			LONG dwEnd = max(1, maxPendingRecvs - curPendingRecvs);
			for (LONG x = 0; x < dwEnd; ++x)
			{
				GV_PROFILE_EVENT_SLOW(gv_iocp_udp_server_call_recvfrom)

				gv_packet* packet = create_packet();
				packet->inc_ref();
				UDPOverlappedEvent* event = packet->get_user_data< UDPOverlappedEvent >();
				event->addressLength = sizeof(gv_socket_address);
				event->address =
					gv_socket_address(gv_ip_address_ipv4::addr_wild_card(), port);
				event->operation = HPS_OPERATION_READ;
				DEBUG_DUMP_EVENT(event);

				// If WSARecvFrom is completed in an overlapped manner, it is the
				// Winsock service provider's
				// responsibility to capture the WSABUF structures before returning from
				// this call. This enables
				// applications to build stack-based WSABUF arrays pointed to by the
				// lpBuffers parameter.
				// http://msdn.microsoft.com/en-us/library/ms741686(VS.85).aspx
				packet->reserve_max_content_size();
				WSABUF recvBufferDescriptor;
				recvBufferDescriptor.buf = packet->get_network_diagram_start();
				recvBufferDescriptor.len = packet->get_network_diagram_size();

				// Post our overlapped receive event on the socket. We expect to get an
				// error in this situation.
				DWORD numberOfBytes = 0;
				DWORD recvFlags = 0;
				INT result;
				{
					GV_PROFILE_EVENT_SLOW(WSARecvFrom)
#pragma GV_REMINDER( \
	"[PITFALL] WSARecvFrom will change the ip address in the event ! so when remote is not existed  , event contained the remote address!!!")
					result =
						WSARecvFrom(server_socket.get_socket_handle(),
									&recvBufferDescriptor, 1, &numberOfBytes, &recvFlags,
									reinterpret_cast< sockaddr* >(&event->address),
									&event->addressLength, &event->overlapped, NULL);
				}
				if (result == SOCKET_ERROR)
				{
					int error = WSAGetLastError();

					// We expect to get an ERROR_IO_PENDING result as we are posting
					// overlapped events
					if (error != ERROR_IO_PENDING)
					{
						// If we get here, then we are trying to post more events than
						// we can currently handle, so we just break out of the loop
						if (error == WSAENOBUFS)
						{
							break;
						}
						// Time to exit since the socket was closed. This should only happen
						// if destroy is called.
						else if (error == WSAENOTSOCK)
						{
							destroy_packet(packet);
							return;
						}
						// Otherwise, we have some other error to handle
						else
						{
							GVM_DEBUG_LOG(
								net, "[IOCP]WSARecvFrom failed. WSAGetLastError returned [%i]"
										 << error << "\r\n");
							destroy_packet(packet);
						}
					}
					// As expected
					else
					{
						// Add a pending overlapped events just posted
						InterlockedIncrement(&curPendingRecvs);
					}
				} // error
				else
				{
					// If we get here, we did not have enough pending receives posted on
					// the socket.
					// This is ok because a worker thread will still process the data,
					// it's just we
					// hit a burst operation (can be simulated by holding the console
					// thread) and
					// we churned through the data. It is very important we do not free
					// the 'event'
					// pointer here! It *will* be handled by a WorkerThread. There is
					// nothing we
					// need to do here really.
					GVM_DEBUG_LOG(
						net, "[IOCP] pending receives not enough, the recv is completed "
								 << "\r\n");
				}
			}

			// We now need to check to make sure our pending receive count is greater
			// than our warning pending receive count. The reason we have to do this
			// is
			// in case we consume events too fast in a burst of traffic or the thread
			// is blocked and we are not in a state of being able to satisfy more
			// events.
			// if we do not check this here, we could essentially kill the server by
			// not
			// having any overlapped io events to be retrieved by the worker threads!
			if (curPendingRecvs > refillPendingRecvs && curPendingRecvs > 0)
			{
				// Allow the event to be signaled now that we have more reads
				// posted. If this is not executed, then we will want to execute our
				// previous loop until this actually happens.
				RefillEvent.reset();
			}
		}
		// else
		{
			// Otherwise we received an APC and it is going to be handled by our event
			// processing function automatically for us. We do not need to call reset
			// event here since the event was not signaled; the thread was alerted.
		}
	}
}

//-----------------------------------------------------------------------------
// This is the worker thread for the IOCP process. It's task is to poll
// overlapped
// completion events and dispatch them to the event processing thread.
void gv_iocp_udp_server_impl::WorkerThread()
{
	GV_PROFILE_EVENT_SLOW(gv_iocp_udp_server_work_thread)
	BOOL result = 0;
	DWORD numberOfBytes = 0;
	ULONG_PTR key = 0;
	OVERLAPPED* lpOverlapped = 0;
	UDPOverlappedEvent* event = 0;
	for (;;)
	{
		GV_PROFILE_EVENT_SLOW(gv_iocp_udp_server_work_thread_loop)
		{
			GV_PROFILE_EVENT_SLOW(gv_iocp_udp_server_get_queued_completing_status)
			result = GetQueuedCompletionStatus(completionPort, &numberOfBytes, &key,
											   &lpOverlapped, INFINITE);
		}
		// We reserve a special event of having a passed key of -1 to signal an
		// exit. In this case
		// we just want to break from our infinite loop. Since we are using pools to
		// manage memory,
		// we do not have to worry about anything else here.
		if (key == -1)
		{
			GV_PROFILE_EVENT_SLOW(gv_iocp_udp_server_get_exit_event)
			break;
		}
		event = CONTAINING_RECORD(lpOverlapped, UDPOverlappedEvent, overlapped);
		// If the GetQueuedCompletionStatus call was successful, we can pass our
		// data to the
		// thread for processing network data.
		if (result == TRUE)
		{
			// No key means no context operation data, so WSARecvFrom/WSASendTo
			// generated
			// this event for us.
			if (key == 0)
			{
				// WSARecvFrom event
				if (event->operation == HPS_OPERATION_READ)
				{
					GV_PROFILE_EVENT_SLOW(gv_iocp_udp_server_process_read)
					// If we now have less pending receives than our minimal quota, set
					// the event to signal
					// a refill is needed on the overlapped receives.
					if (InterlockedDecrement(&curPendingRecvs) < refillPendingRecvs)
					{
						RefillEvent.set();
					}

					// Store how many bytes we retrieved for future use. If we don't, we
					// won't know
					// how many bytes the data is.
					// event->dataBufferSize = static_cast<gv_ushort>(numberOfBytes);
					gv_packet* packet = event->owner;
					packet->reserve_content_size(numberOfBytes);
					packet->m_address = event->address;
					packet->m_time_stamp = gv_global::time->time_stamp_unix();

					// Generate the structure to pass to our event processing function
					APCEvent* apcData = create_apc_event();
					apcData->event = event;
					apcData->server = this;
					if (!m_apc_enabled)
					{
						this->OnClientToServer(parent, event->address, event->owner);
					}
					// Queue the APC event to our event processing thread. This call is
					// thread safe!
					else
					{
						if (QueueUserAPC(ProcessEventWrapper,
										 APCEventProcessingThread.get_handle(),
										 reinterpret_cast< ULONG_PTR >(apcData)) == 0)
						{
							GV_PROFILE_EVENT_SLOW(gv_iocp_udp_server_queue_user_apc_fail !!!)
							GVM_DEBUG_LOG(net, "[IOCP]QueueUserAPC failed. The event [%i "
											   "bytes] will be discarded. GetLastError "
											   "returned [%i]"
												   << (int)numberOfBytes
												   << (gv_int)GetLastError() << "\r\n");
							// Free this data since we cannot process it in the appropriate
							// thread.
							destroy_packet(event->owner);
							destroy_apc_event(apcData);
						}
					}
				}

				// WSASendTo event
				else if (event->operation == HPS_OPERATION_WRITE)
				{
					GV_PROFILE_EVENT_SLOW(gv_iocp_udp_server_process_write)
					// We don't have anything in particular to do here framework wise,
					// but you can add additional logic similar to the read operation
					// if you wish to do post processing logic on all data sent for
					// logging or whatever you think of.

					// Free this data since Winsock has processed it already.
					destroy_packet(event->owner);
				}
				// Uh-oh! Bad data, perhaps the user did not initialize everything
				else
				{
					GV_PROFILE_EVENT_SLOW(gv_iocp_udp_server_process_error !!!)
					GVM_DEBUG_LOG(net, "[IOCP]Invalid event operation specified [%.2X]."
										   << event->operation << "\r\n");
					destroy_packet(event->owner);
				}
			}
			else
			{
				// We did something via PostQueuedCompletionStatus. We don't
				// have any specific code in this example, but if we do pass
				// something via the key parameter, we have to consider if it
				// needs to be cleaned up or not. We assume in this case we
				// will pass an event we need to free, that might not always
				// be the case though!
				destroy_packet(event->owner);
			}
		}
		else
		{
			GV_PROFILE_EVENT_SLOW(gv_iocp_udp_server_Queue_FAIL !)
			if (event->operation == HPS_OPERATION_READ)
			{
				if (InterlockedDecrement(&curPendingRecvs) < refillPendingRecvs)
				{
					RefillEvent.set();
				}
				if (!socket_killed)
					GVM_DEBUG_LOG(net, "[IOCP]READ ERROR!" << event->address.string()
														   << gv_endl);
			}
			else if (event->operation == HPS_OPERATION_WRITE)
			{
				if (!socket_killed)
					GVM_DEBUG_LOG(net, "[IOCP]WRITE ERROR!" << event->address.string()
															<< gv_endl);
			};
			gv_uint error_code = GetLastError();
			if (OnIOError && error_code != 995)
			{
				// typedef void (GV_STDCALL * gv_callback_on_io_error)     (class
				// gvi_server_transport_layer_udp * server, gv_socket_address & address,
				// gv_packet * , gv_byte operation , gv_uint error_code);
				OnIOError(parent, event->address, event->owner, event->operation,
						  error_code);
				if (!socket_killed)
					GVM_DEBUG_LOG(
						net,
						"[IOCP]GetQueuedCompletionStatus failed. GetLastError returned "
							<< (gv_int)GetLastError() << "\r\n");
			}
			if (error_code == 995 && !socket_killed)
			{
				if (!socket_killed)
					GVM_DEBUG_LOG(net, "[IOCP]socket shut down with 995 error");
			}
			// Free this connection data since we cannot process it in the appropriate
			// thread.
			destroy_packet(event->owner);
		}
	}
}
//-----------------------------------------------------------------------------
VOID CALLBACK ProcessEventWrapper(ULONG_PTR param)
{
	APCEvent* acpData = reinterpret_cast< APCEvent* >(param);
	acpData->server->ProcessEvent(acpData->event->owner);
	acpData->server->destroy_apc_event(acpData);
}

//-----------------------------------------------------------------------------
// Default function place holder
void WINAPI DefaultOnClientToServer(gvi_server_transport_layer_udp* server,
									gv_socket_address& address, gv_packet* p)
{
	UNREFERENCED_PARAMETER(address);
	UNREFERENCED_PARAMETER(p);
	UNREFERENCED_PARAMETER(server);
}

//-----------------------------------------------------------------------------
gv_iocp_udp_server::gv_iocp_udp_server()
{
	m_pimpl = new gv_iocp_udp_server_impl(this);
}
//-----------------------------------------------------------------------------
// The default dtor will cleanup our internal data object
gv_iocp_udp_server::~gv_iocp_udp_server()
{
	delete m_pimpl;
	// s_pool_packet::purge_memory();
	// s_pool_apc_event::purge_memory();
}
//-----------------------------------------------------------------------------
bool gv_iocp_udp_server::create(gv_ushort port,
								gv_uint numberOfConcurrentThreads,
								gv_uint numberOfWorkerThreads,
								gv_int maxPendingRecvs,
								gv_int refillPendingRecvs)
{
	return m_pimpl->create(port, numberOfConcurrentThreads, numberOfWorkerThreads,
						   maxPendingRecvs, refillPendingRecvs);
}
//-----------------------------------------------------------------------------
// Close down the server and free internally used resources.
void gv_iocp_udp_server::destroy()
{
	return m_pimpl->destroy();
}
//-----------------------------------------------------------------------------
void gv_iocp_udp_server::set_callback_on_recv(
	gv_callback_on_recv_complete function)
{
	m_pimpl->OnClientToServer = function;
	if (m_pimpl->OnClientToServer == NULL)
	{
		m_pimpl->OnClientToServer = DefaultOnClientToServer;
	}
}

void gv_iocp_udp_server::set_callback_on_error(
	gv_callback_on_io_error function)
{
	m_pimpl->OnIOError = function;
};
//-----------------------------------------------------------------------------
bool gv_iocp_udp_server::send_to(const gv_socket_address& address, int count,
								 UINT8* data)
{
	return m_pimpl->send_to(address, count, data);
}
//-----------------------------------------------------------------------------
bool gv_iocp_udp_server::send_to(const gv_socket_address& address,
								 gv_packet* p)
{
	UDPOverlappedEvent* event = p->get_user_data< UDPOverlappedEvent >();
	event->address = address;
	event->operation = HPS_OPERATION_WRITE;
	DEBUG_DUMP_EVENT(event);
	return m_pimpl->Send(p);
}

gv_packet* gv_iocp_udp_server::create_packet()
{
	return m_pimpl->create_packet();
};
void gv_iocp_udp_server::destroy_packet(gv_packet* p)
{
	return m_pimpl->destroy_packet(p);
};

void gv_iocp_udp_server::enable_apc_call(bool enable)
{
	return m_pimpl->enable_apc_call(enable);
};
//-----------------------------------------------------------------------------
} // gv;
