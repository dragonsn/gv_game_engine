#include <cstdlib>
#include <iostream>
#include <boost/boost/aligned_storage.hpp>
#include <boost/boost/array.hpp>
#include <boost/boost/bind.hpp>
#include <boost/boost/enable_shared_from_this.hpp>
#include <boost/boost/noncopyable.hpp>
#include <boost/boost/shared_ptr.hpp>
#include <boost/boost/thread/thread.hpp>
#include "../network/gv_asio.h"
#include <deque>
#include <set>
namespace unit_test_asio_chat
{
using boost::asio::ip::tcp;
class chat_message
{
public:
	enum
	{
		header_length = 4
	};
	enum
	{
		max_body_length = 512
	};

	chat_message()
		: body_length_(0)
	{
		GVM_ZERO_ME;
	}

	const char* data() const
	{
		return data_;
	}

	char* data()
	{
		return data_;
	}

	size_t length() const
	{
		return header_length + body_length_;
	}

	const char* body() const
	{
		return data_ + header_length;
	}

	char* body()
	{
		return data_ + header_length;
	}

	size_t body_length() const
	{
		return body_length_;
	}

	void body_length(size_t new_length)
	{
		body_length_ = new_length;
		if (body_length_ > max_body_length)
			body_length_ = max_body_length;
	}

	bool decode_header()
	{
		using namespace std; // For strncat and atoi.
		char header[header_length + 1] = "";
		strncat(header, data_, header_length);
		body_length_ = atoi(header);
		if (body_length_ > max_body_length)
		{
			body_length_ = 0;
			return false;
		}
		return true;
	}

	void encode_header()
	{
		using namespace std; // For sprintf and memcpy.
		char header[header_length + 1] = "";
		sprintf(header, "%4d", (int)body_length_);
		memcpy(data_, header, header_length);
	}

private:
	char data_[header_length + max_body_length];
	size_t body_length_;
};
typedef std::deque< chat_message > chat_message_queue;

namespace client
{

class chat_client
{
public:
	chat_client(boost::asio::io_service& io_service,
				tcp::resolver::iterator endpoint_iterator)
		: io_service_(io_service),
		  socket_(io_service)
	{
		boost::asio::async_connect(socket_, endpoint_iterator,
								   boost::bind(&chat_client::handle_connect, this,
											   boost::asio::placeholders::error));
	}

	void write(const chat_message& msg)
	{
		GV_PROFILE_EVENT(chat_client__write, 0);
		io_service_.post(boost::bind(&chat_client::do_write, this, msg));
	}

	void close()
	{
		GV_PROFILE_EVENT(chat_client__close, 0);
		io_service_.post(boost::bind(&chat_client::do_close, this));
	}

private:
	void handle_connect(const boost::system::error_code& error)
	{
		GV_PROFILE_EVENT(chat_client__handle_connect, 0);
		if (!error)
		{
			boost::asio::async_read(socket_,
									boost::asio::buffer(read_msg_.data(), chat_message::header_length),
									boost::bind(&chat_client::handle_read_header, this,
												boost::asio::placeholders::error));
		}
	}

	void handle_read_header(const boost::system::error_code& error)
	{
		GV_PROFILE_EVENT(chat_client__handle_read_header, 0);
		if (!error && read_msg_.decode_header())
		{
			boost::asio::async_read(socket_,
									boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
									boost::bind(&chat_client::handle_read_body, this,
												boost::asio::placeholders::error));
		}
		else
		{
			do_close();
		}
	}

	void handle_read_body(const boost::system::error_code& error)
	{
		GV_PROFILE_EVENT(chat_client__handle_read_body, 0);
		if (!error)
		{
			if (read_msg_.body_length())
			{
				gv_string temp;
				temp << " client READ MESSAGE: " << read_msg_.data();
				test_log() << *temp;
			}
			std::cout.write(read_msg_.body(), read_msg_.body_length());
			std::cout << "\n";
			boost::asio::async_read(socket_,
									boost::asio::buffer(read_msg_.data(), chat_message::header_length),
									boost::bind(&chat_client::handle_read_header, this,
												boost::asio::placeholders::error));
		}
		else
		{
			do_close();
		}
	}

	void do_write(chat_message msg)
	{
		GV_PROFILE_EVENT(chat_client__do_write, 0);
		bool write_in_progress = !write_msgs_.empty();
		write_msgs_.push_back(msg);
		if (!write_in_progress)
		{
			boost::asio::async_write(socket_,
									 boost::asio::buffer(write_msgs_.front().data(),
														 write_msgs_.front().length()),
									 boost::bind(&chat_client::handle_write, this,
												 boost::asio::placeholders::error));
		}
	}

	void handle_write(const boost::system::error_code& error)
	{
		GV_PROFILE_EVENT(chat_client__handle_write, 0);
		if (!error)
		{
			write_msgs_.pop_front();
			if (!write_msgs_.empty())
			{
				boost::asio::async_write(socket_,
										 boost::asio::buffer(write_msgs_.front().data(),
															 write_msgs_.front().length()),
										 boost::bind(&chat_client::handle_write, this,
													 boost::asio::placeholders::error));
			}
		}
		else
		{
			do_close();
		}
	}

	void do_close()
	{
		GV_PROFILE_EVENT(chat_client__do_close, 0);
		socket_.close();
	}

private:
	boost::asio::io_service& io_service_;
	tcp::socket socket_;
	chat_message read_msg_;
	chat_message_queue write_msgs_;
};

void main(const gvt_array< gv_string >& argv)
{
	try
	{
		if (argv.size() != 3)
		{
			GVM_ERROR("Usage: chat_client <host> <port> <loop>\n");
			return;
		}

		boost::asio::io_service io_service;

		tcp::resolver resolver(io_service);
		tcp::resolver::query query(*argv[0], *argv[1]);
		tcp::resolver::iterator iterator = resolver.resolve(query);

		chat_client c(io_service, iterator);

		boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));
		gv_int nb_loop;
		argv[2] >> nb_loop;
		for (int i = 0; i < nb_loop; i++)
		{
			using namespace std; // For strlen and memcpy.
			gv_string_tmp line;
			line << "hello world , I am from " << (size_t)gv_thread::current() << "||" << i;
			chat_message msg;
			msg.body_length(line.strlen());
			memcpy(msg.body(), *line, msg.body_length());
			msg.encode_header();
			c.write(msg);
			gv_thread::sleep(10);
		}

		c.close();
		t.join();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return;
}
}

namespace server
{

class chat_participant
{
public:
	virtual ~chat_participant()
	{
	}
	virtual void deliver(const chat_message& msg) = 0;
};

typedef boost::shared_ptr< chat_participant > chat_participant_ptr;

//----------------------------------------------------------------------

class chat_room
{
public:
	void join(chat_participant_ptr participant)
	{
		GV_PROFILE_EVENT(chat_room__join, 0);
		participants_.insert(participant);
		std::for_each(recent_msgs_.begin(), recent_msgs_.end(),
					  boost::bind(&chat_participant::deliver, participant, _1));
	}

	void leave(chat_participant_ptr participant)
	{
		GV_PROFILE_EVENT(chat_room__leave, 0);
		participants_.erase(participant);
	}

	void deliver(const chat_message& msg)
	{
		GV_PROFILE_EVENT(chat_room__deliver, 0);
		recent_msgs_.push_back(msg);
		while (recent_msgs_.size() > max_recent_msgs)
			recent_msgs_.pop_front();

		std::for_each(participants_.begin(), participants_.end(),
					  boost::bind(&chat_participant::deliver, _1, boost::ref(msg)));
	}

private:
	std::set< chat_participant_ptr > participants_;
	enum
	{
		max_recent_msgs = 100
	};
	chat_message_queue recent_msgs_;
};

//----------------------------------------------------------------------

class chat_session
	: public chat_participant,
	  public boost::enable_shared_from_this< chat_session >
{
public:
	chat_session(boost::asio::io_service& io_service, chat_room& room)
		: socket_(io_service),
		  room_(room)
	{
	}

	tcp::socket& socket()
	{
		return socket_;
	}

	void start()
	{
		GV_PROFILE_EVENT(chat_session__start, 0);
		room_.join(shared_from_this());
		boost::asio::async_read(socket_,
								boost::asio::buffer(read_msg_.data(), chat_message::header_length),
								boost::bind(
									&chat_session::handle_read_header, shared_from_this(),
									boost::asio::placeholders::error));
	}

	void deliver(const chat_message& msg)
	{
		GV_PROFILE_EVENT(chat_session__deliver, 0);
		bool write_in_progress = !write_msgs_.empty();
		write_msgs_.push_back(msg);
		if (!write_in_progress)
		{
			boost::asio::async_write(socket_,
									 boost::asio::buffer(write_msgs_.front().data(),
														 write_msgs_.front().length()),
									 boost::bind(&chat_session::handle_write, shared_from_this(),
												 boost::asio::placeholders::error));
		}
	}

	void handle_read_header(const boost::system::error_code& error)
	{
		GV_PROFILE_EVENT(chat_session__handle_read_header, 0);
		if (!error && read_msg_.decode_header())
		{
			boost::asio::async_read(socket_,
									boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
									boost::bind(&chat_session::handle_read_body, shared_from_this(),
												boost::asio::placeholders::error));
		}
		else
		{
			room_.leave(shared_from_this());
		}
	}

	void handle_read_body(const boost::system::error_code& error)
	{
		GV_PROFILE_EVENT(chat_session__handle_read_body, 0);
		if (!error)
		{
			if (read_msg_.body_length())
			{
				gv_string temp;
				temp << " server READ MESSAGE: " << read_msg_.data();
				test_log() << *temp;
			}
			room_.deliver(read_msg_);
			boost::asio::async_read(socket_,
									boost::asio::buffer(read_msg_.data(), chat_message::header_length),
									boost::bind(&chat_session::handle_read_header, shared_from_this(),
												boost::asio::placeholders::error));
		}
		else
		{
			room_.leave(shared_from_this());
		}
	}

	void handle_write(const boost::system::error_code& error)
	{
		GV_PROFILE_EVENT(chat_session__handle_write, 0);
		if (!error)
		{
			write_msgs_.pop_front();
			if (!write_msgs_.empty())
			{
				boost::asio::async_write(socket_,
										 boost::asio::buffer(write_msgs_.front().data(),
															 write_msgs_.front().length()),
										 boost::bind(&chat_session::handle_write, shared_from_this(),
													 boost::asio::placeholders::error));
			}
		}
		else
		{
			room_.leave(shared_from_this());
		}
	}

private:
	tcp::socket socket_;
	chat_room& room_;
	chat_message read_msg_;
	chat_message_queue write_msgs_;
};

typedef boost::shared_ptr< chat_session > chat_session_ptr;

//----------------------------------------------------------------------

class chat_server
{
public:
	chat_server(boost::asio::io_service& io_service,
				const tcp::endpoint& endpoint)
		: io_service_(io_service),
		  acceptor_(io_service, endpoint)
	{
		start_accept();
	}

	void start_accept()
	{
		GV_PROFILE_EVENT(chat_server__start_accept, 0);
		chat_session_ptr new_session(new chat_session(io_service_, room_));
		acceptor_.async_accept(new_session->socket(),
							   boost::bind(&chat_server::handle_accept, this, new_session,
										   boost::asio::placeholders::error));
	}

	void handle_accept(chat_session_ptr session,
					   const boost::system::error_code& error)
	{
		GV_PROFILE_EVENT(chat_server__handle_accept, 0);
		if (!error)
		{
			session->start();
		}

		start_accept();
	}

private:
	boost::asio::io_service& io_service_;
	tcp::acceptor acceptor_;
	chat_room room_;
};

typedef boost::shared_ptr< chat_server > chat_server_ptr;
typedef std::list< chat_server_ptr > chat_server_list;

//----------------------------------------------------------------------
boost::asio::io_service* p_io_service;
void main(const gvt_array< gv_string >& argv)
{
	try
	{
		p_io_service = new boost::asio::io_service;
		if (argv.size() < 2)
		{
			GVM_ERROR("Usage: chat_server <loop> <port> [<port> ...]\n");
			return;
		}
		gv_int nb_loop;
		argv[0] >> nb_loop;

		chat_server_list servers;
		for (int i = 1; i < argv.size(); ++i)
		{
			using namespace std; // For atoi.
			tcp::endpoint endpoint(tcp::v4(), atoi(*argv[i]));
			chat_server_ptr server(new chat_server(*p_io_service, endpoint));
			servers.push_back(server);
		}
		p_io_service->run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return;
}
}

void main(gvt_array< gv_string >& args)
{
	gv_int nb_client = 5;
	gv_int nb_loop = 1000;
	if (args.size() >= 1)
	{
		args[0] >> nb_client;
	}
	if (args.size() >= 2)
	{
		args[1] >> nb_loop;
	}
	gv_string s;
	s << nb_loop;

	gvt_array< gv_string > server_param;
	server_param.push_back(s);
	server_param.push_back("8822");
	server_param.push_back("8823");
	gv_thread server;
	server.set_name(gv_id("server"));
	server.start(boost::bind(server::main, server_param));
	gv_thread client;

	gvt_array< gv_thread > clients;
	clients.resize(nb_client);
	for (int i = 0; i < nb_client; i++)
	{
		gv_string name("client");
		name << i;
		clients[i].set_name(gv_id(*name));
		gvt_array< gv_string > client_param;
		client_param.push_back("127.0.0.1");
		if (i % 1)
			client_param.push_back("8822");
		else
			client_param.push_back("8823");

		client_param.push_back(s);
		clients[i].start(boost::bind(client::main, client_param));
	}
	for (int i = 0; i < clients.size(); i++)
	{
		clients[i].join();
	}
	server::p_io_service->stop();
	server.join();
	delete server::p_io_service;
}
};