//============================================================================================
//								:
//============================================================================================

namespace  tool_memory_alloc_analysis
{
	template<class T>
	struct gvt_endian32_helper
	{
		union
		{
			T		d; 
			gv_byte b[4];
		};
	};

	template<class T> 
	T gvt_swap_endian_32( const T &  input )
	{
		gvt_endian32_helper<T> h; 
		h.d=input; 
		gvt_swap(h.b[0], h.b[3]);
		gvt_swap(h.b[1], h.b[2]);
		return h.d;
	};

	static const int			c_base_backtrace_depth	=4;
	static const int			c_max_backtrace_depth	=16;
	static const int			c_backtrace_size		=c_max_backtrace_depth-c_base_backtrace_depth+1;

	struct alloc_info
	{
		alloc_info ()
		{
			gvt_zero(*this);
		}

		void	swap_endian()
		{
			memory_ptr=gvt_swap_endian_32(memory_ptr); 
			memory_size=gvt_swap_endian_32(memory_size); 
			backtrace_size=gvt_swap_endian_32(backtrace_size); 
			for ( int i=0 ; i< c_backtrace_size; i++) backtrace[i]=gvt_swap_endian_32(backtrace[i]); 
		};

		template < class type_of_stream > 
		void	print (type_of_stream & input ,bool skip_size=false)
		{
			gv_string_tmp s;
			s<<gv_endl; 
			if (!skip_size)
			{
				s<<"size :"<<memory_size<<gv_endl; 
				s<<"ptr  :"<<memory_ptr <<gv_endl;
			};
			s<<"stack:"<<gv_endl; 
			for ( int i =0; i<backtrace_size; i++)
			{
				s<<"    0x"<<(void *)(gv_int_ptr)backtrace[i]<<gv_endl;
			}
			input<<*s;
		};

		bool			include( gv_uint ptr)
		{
			return ptr>=memory_ptr && ptr<=memory_ptr+(memory_size<0?0:memory_size); 
		}

		gv_uint			memory_ptr; 
		int				memory_size; //neg -1 is free 
		int				backtrace_size;
		gv_uint			backtrace [c_backtrace_size];

		bool	operator==(const  alloc_info &  s)
		{
			return gvt_for_each_is_equal(backtrace,s.backtrace,backtrace_size); 
		}
	};

	struct alloc_statistic
	{
		alloc_statistic(){gvt_zero(*this);min_alloc_size=1000000000;}
		int size; 
		int times; 
		int max_alloc_size; 
		int min_alloc_size;

		template < class type_of_stream > 
		void	print (type_of_stream & input)
		{
			gv_string_tmp s;
			s<<"size="<<size<<gv_endl;
			s<<"times="<<times<<gv_endl;
			s<<"max_alloc_size="<<max_alloc_size<<gv_endl;
			s<<"min_alloc_size="<<min_alloc_size<<gv_endl;
			input<<*s;
		}

	};

	gv_uint gvt_hash(const alloc_info &info)
	{
		gv_uint	sum=0; 
		for (int i=0; i<info.backtrace_size; i++)
		{
			sum+=info.backtrace[i]/23;
		}
		return sum;
	}

	struct alloc_result
	{
		alloc_info a; 
		alloc_statistic s;
		bool operator< (const alloc_result & b)
		{
			return s.size >b.s.size;
		}

		template < class type_of_stream > 
		void	print (type_of_stream & input ,bool skip_size=false)
		{
			input<<"<<<========================"<<gv_endl;
			s.print(input);
			a.print(input,true);
		};
	};
	
	bool find_first_match (int & pos , gvt_array<alloc_info >&  infos ,gv_uint ptr, gv_bool back=true)
	{
		gv_int step=1;
		if (back)  step=-1; 
		while (1)
		{
			if (infos.is_valid(pos) )
			{
				if (infos[pos].include(ptr ))
				{
					pos+=step;
					return true;
				}
			}
			else return false;
			pos+=step;
		}
		return false;
	}
	typedef gvt_hash_map<void *    ,alloc_info,4096,gvp_memory_256 > type_of_p_to_callstack_map;
	typedef gvt_hash_map<alloc_info,alloc_statistic ,4096,gvp_memory_256    > type_of_callstack_to_info_map;
	void main( gvt_array<gv_string > &args )
	{
		gvt_array<alloc_info >  infos; 
		gvt_array<gv_byte> array; 
		gv_string base_name("alloc_info");

		type_of_p_to_callstack_map p_to_callstack_map; //used to find whether the memory is allocated .
		type_of_callstack_to_info_map callstack_to_info_map; 
		//======================================================================
		test_log()<<"reading..."<<gv_endl;
		int count =0; 
		int last_idx =100000000; 
		int list_number=20;
		int extreme_detail=0;
		if (args.size())    args[0]>>count;
		else return;
		if (args.size()>1)  args[1]>>last_idx;
		if (args.size()>2)  args[2]>>list_number;
		if (args.size()>3)  args[2]>>list_number;  
		alloc_statistic mem_info0; 
		while(1)
		{
			gv_string file_name=base_name;
			if (count==last_idx) break;
			file_name<<count++<<".bin";
			test_log()<<"try to load "<<*file_name<<gv_endl;
			if (!gv_load_file_to_array(*file_name,array)) 	{	break;}
			int seek_size=0;
			while (seek_size<array.size())
			{
				alloc_info tmp;
				alloc_info * info=&tmp;//infos.add_dummy(); ;
				*info=*((alloc_info*)(&array[seek_size])); 
				if (extreme_detail) infos.add(tmp);
				info->swap_endian(); 
				seek_size+=sizeof(alloc_info);
				if (info->memory_size >0 && info->memory_ptr)
				{
					void * p=(void*)(gv_int_ptr) info->memory_ptr;
					//GV_ASSERT(p_to_callstack_map.find(p)); 
					p_to_callstack_map.add(p,*info);
					mem_info0.times++;
					mem_info0.size+=info->memory_size;
				}
				if (info->memory_size< 0 && info->memory_ptr)
				{
					void * p=(void*)(gv_int_ptr)info->memory_ptr;
					alloc_info * pinfo=p_to_callstack_map.find(p);
					if (pinfo) 
					{
						mem_info0.size-=pinfo->memory_size;
					}
					p_to_callstack_map.erase(p);
					mem_info0.times--;
				}
			}
			test_log()<<"not freed block number:"<<mem_info0.times<<gv_endl; 
			test_log()<<"total size:"<<mem_info0.size<<gv_endl; 
			array.clear_and_reserve();
		};
		//=====================================================
		//get statistic 
		//=====================================================
		test_log()<<"merging..."<<gv_endl;
		alloc_statistic mem_info; 
		{
			type_of_p_to_callstack_map::iterator  it;
			it=p_to_callstack_map.begin(); 
			while (it!=p_to_callstack_map.end() )
			{
				if(it.is_empty()) 
				{
					it++; continue;
				}
				void * p=it.get_key(); 
				alloc_info & stack=*it;
				alloc_statistic  & s=callstack_to_info_map[stack]; 
				s.size+=stack.memory_size; 
				s.times++; 
				s.min_alloc_size=gvt_min(s.min_alloc_size, stack.memory_size);
				s.max_alloc_size=gvt_max(s.max_alloc_size, stack.memory_size);

				mem_info.times++; 
				mem_info.size+=stack.memory_size; 
				it++;
			}
		}
		//=====================================================
		//sort by size 
		//=====================================================
		test_log()<<"sorting..."<<gv_endl;
		gvt_array<alloc_result> results;
		{
			type_of_callstack_to_info_map::iterator  it;
			it=callstack_to_info_map.begin(); 
			while (it!=callstack_to_info_map.end() )
			{
				if(it.is_empty()) 
				{
					it++; continue;
				}
				alloc_result * r=results.add_dummy(); 
				r->a=it.get_key();
				r->s=*it;
				it++;
			}
			results.sort();
		}
		//=====================================================
		test_log()<<"done!"<<gv_endl;
		test_log()<<"not freed block number:"<<mem_info.times<<gv_endl; 
		test_log()<<"total size:"<<mem_info.size<<gv_endl;
		list_number=gvt_min(list_number,results.size() );
		test_log()<<"TOP :"<<list_number<<gv_endl;
		for ( int i=0 ;i<list_number ; i++)
		{
			test_log()<<"======>>"<<i;
			results[i].print(test_log()); 
		}
		test_log()<<"input your command:!"<<gv_endl;
		//==console ===========================================
		while(1)
		{
			string _s;
			char buffer[2048];
			gets_s(buffer);
			gv_string s=buffer; 
			s.to_lower();
			gvt_array<gv_string> cmd; 
			s.split(cmd); 
			if (!cmd.size()) continue;
			if (cmd[0]== "f")
			{
				if (cmd.size()<2) continue; 
				gv_uint ptr; 
				cmd[1]>>ptr; 
				gv_int  cu_pos=infos.size()-1;
				while (1) 
				{
					if (!find_first_match(cu_pos,infos,ptr) )
					{
					    test_log()<<"not found!"<<gv_endl; 
						break;
					}; 
					test_log()<< "find in "<<cu_pos<<gv_endl;
					infos[cu_pos+1].print(test_log()); 
					test_log()<<"q to end"<<gv_endl; 
					cin>>_s;
					if (_s=="q") break;
				}
			}
			if (cmd[0]== "s")
			{
				if (cmd.size()<2) continue; 
				gv_int idx; 
				cmd[1]>>idx;
				if (idx<0 ) idx=infos.size()+idx;
				if (!infos.is_valid(idx)) 
				{
					test_log()<<"not found!"<<gv_endl; 
					break;
				}
				test_log()<< "find in "<<idx<<gv_endl;
				infos[idx].print(test_log());
			}
			if (cmd[0]=="q")
			{
				break;
			}
		}
	}//main =======================================================
}