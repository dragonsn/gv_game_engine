#pragma once
/*
namespace	gv
{//!!!!!!!!!!!IT'S NOT SAFE , USE LOCK FREE LIST !!
        template<class type_of_data>
        class gv_lock_free_buffer_base
        {
        public:
                union buffer_pointer
                {
                        gv_uint u_value;
                        gv_int	i_value;
                        struct
                        {
                                gv_uint buffer_idx:1;
                                gv_uint buffer_offset:31;
                        };
                };
                class func_masked_add
                {
                public:
                        gv_int operator()(const gv_int&  a )
                        {
                                buffer_pointer p;
                                p.i_value=a;
                                p.buffer_offset =p.buffer_offset+1;
                                return p.i_value;
                        }
                };

                class func_flip_buffer
                {
                public:
                        gv_int operator()(const gv_int&  a )
                        {
                                buffer_pointer p;
                                p.i_value=a;
                                p.buffer_offset=0;
                                p.buffer_idx =!p.buffer_idx;
                                return p.i_value;
                        }
                };

                gv_lock_free_buffer_base():
                        m_cu_write_pos(0),
                        m_read_buffer_size(0),
                        m_pending_write(0)
                {

                }

                virtual type_of_data *	alloc()
                {
                        return NULL;
                }

                //==================================
                void write (const type_of_data & d )
                {
                        int time_out_cout=0;
                        type_of_data * ret=NULL;
                        while(!ret)
                        {
                                ++m_pending_write;
                                if (m_pending_write.get())
                                {
                                        ret=this->alloc();//make sure it happen
after pending write count is increased!
                                        if (!ret)
                                        {
                                                --m_pending_write;
                                                ++time_out_cout;
                                                // the buffer is full , wait for
swapbuffer.
                                                if (time_out_cout%10)
                                                {//be always prepare large
buffer,and fast switch, the yield will give the CPU away for awhile.
                                                        GVM_DEBUG_OUT("!!!double_buffer_write_time_out!!");
                                                        gv_thread::yield();
                                                }
                                        }
                                        else
                                        {
                                                if (m_pending_write.get())
                                                {
                                                        *ret=d;//make sure it
happen before pending write count is decreased!
                                                }
                                                --m_pending_write;
                                                return ;
                                        }
                                }
                        };
                }
                bool try_write (type_of_data & d )
                {
                        int time_out_cout=0;
                        ++m_pending_write;
                        if (m_pending_write.get())
                        {
                                type_of_data * ret=NULL;
                                ret=this->alloc();
                                if (ret)
                                {
                                        if(m_pending_write.get())
                                        {
                                                *ret=d;
                                        }
                                        --m_pending_write;
                                        return true;
                                }
                                --m_pending_write;
                        }
                        return false;
                }
        protected:
                gv_atomic_count m_cu_write_pos;
                volatile gv_int			m_read_buffer_size;
                gv_atomic_count	m_pending_write;

        };

        template<class type_of_data  , int buffer_size>
        class gvt_lock_free_double_buffer : public
gv_lock_free_buffer_base<type_of_data>
        {

        public:
                typedef type_of_data * iterator;
                gvt_lock_free_double_buffer()
                {
                }
                ~gvt_lock_free_double_buffer()
                {
                }
                //assume the write operation is fast enough,it's quite tricky
during the flip time...

                //==================================
                void swap_buffer()
                {
                        int time_out_cout=0;
                        while (m_pending_write.get()) //wait for time out.
                        {
                                time_out_cout++;
                                if (time_out_cout%30)
                                {//maybe the poor guy only have 1 cpu, poor boy.
                                        gv_thread::yield();
                                }
                        }
                        func_flip_buffer func;
                        gv_int idx=m_cu_write_pos.interlock_any(func );
                        buffer_pointer p; p.i_value=idx;
                        m_read_buffer_size=p.buffer_offset;
                        if (m_read_buffer_size>buffer_size )
m_read_buffer_size=buffer_size;
                }

                type_of_data	* get_read_buffer()
                {
                        return (type_of_data	*)m_buffer[get_read_idx()];
                }

                type_of_data * begin()	{return get_read_buffer() ;}

                type_of_data * end ()	{return	get_read_buffer()
+m_read_buffer_size;}

                gv_int	get_read_buffer_size()
                {
                        return m_read_buffer_size;
                }
        protected:
                int  get_read_idx()
                {
                        buffer_pointer p;
                        p.i_value=m_cu_write_pos.get();
                        int idx=!p.buffer_idx;
                        return idx;
                }

                int  get_write_idx()
                {
                        buffer_pointer p;
                        p.i_value=m_cu_write_pos.get();
                        int idx=p.buffer_idx;
                        return idx;
                }

                type_of_data *	alloc()
                {
                        buffer_pointer p;
                        func_masked_add func;
                        gv_int idx=m_cu_write_pos.interlock_any( func);
                        p.i_value=idx;
                        if (p.buffer_offset >=buffer_size ) return NULL;
                        return &m_buffer[p.buffer_idx][p.buffer_offset];
                }
                type_of_data * get_write_buffer()
                {
                        return m_buffer[get_write_idx()];
                }
                //==================================

        protected:
                type_of_data	m_buffer[2][buffer_size];

        };



        template<class type_of_data >
        class gvt_lock_free_double_buffer_resizable : public
gv_lock_free_buffer_base<type_of_data>
        {

        public:
                typedef type_of_data * iterator;
                gvt_lock_free_double_buffer_resizable()
                {
                        m_buffer_size=0;
                }
                ~gvt_lock_free_double_buffer_resizable()
                {
                }
                void reset(int size )
                {
                        m_buffer_size=size;
                        m_buffer[0].resize(size);
                        m_buffer[1].resize(size);
                }


                //==================================
                void swap_buffer()
                {
                        int time_out_cout=0;
                        while (m_pending_write.get()) //wait for time out.
                        {
                                time_out_cout++;
                                if (time_out_cout%30)
                                {//maybe the poor guy only have 1 cpu, poor boy.
                                        gv_thread::yield();
                                }
                        }
                        func_flip_buffer func;
                        gv_int idx=m_cu_write_pos.interlock_any(func );
                        buffer_pointer p; p.i_value=idx;
                        m_read_buffer_size=p.buffer_offset;
                        if (m_read_buffer_size>m_buffer_size )
m_read_buffer_size=m_buffer_size;
                }

                type_of_data	* get_read_buffer()
                {
                        return m_buffer[get_read_idx()].begin();
                }

                type_of_data * begin()	{return get_read_buffer() ;}

                type_of_data * end ()	{return	get_read_buffer()
+m_read_buffer_size;}

                gv_int	get_read_buffer_size()
                {
                        return m_read_buffer_size;
                }
        protected:
                int  get_read_idx()
                {
                        buffer_pointer p;
                        p.i_value=m_cu_write_pos.get();
                        int idx=!p.buffer_idx;
                        return idx;
                }

                int  get_write_idx()
                {
                        buffer_pointer p;
                        p.i_value=m_cu_write_pos.get();
                        int idx=p.buffer_idx;
                        return idx;
                }

                type_of_data *	alloc()
                {
                        buffer_pointer p;
                        func_masked_add func;
                        gv_int idx=m_cu_write_pos.interlock_any( func);
                        p.i_value=idx;
                        if (p.buffer_offset >=(gv_uint)
                                m_buffer_size ) return NULL;
                        return &m_buffer[p.buffer_idx][p.buffer_offset];
                }
                type_of_data * get_write_buffer()
                {
                        return m_buffer[get_write_idx()];
                }
                //==================================

        protected:
                gv_int m_buffer_size;
                gvt_array<type_of_data>	m_buffer[2];
        };
}
*/