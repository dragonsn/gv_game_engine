#include "stdafx.h"
using namespace gv;
//#pragma   GV_DISABLE_OPTIMIZATION
namespace tool_perf_viewer_2d {
	static int tick_count = 0;

	class text_bar : public gv_refable {
	public:
		gv_rect rect;
		gv_rect rect_include_children;
		gv_string title;
		gv_string extra_string;
		gv_color color;
		gv_bool show_extra;
		gv_bool is_checked;

		gvt_ptr<text_bar> parent;
		gvt_array<text_bar *> children;

		static gv_rect root_window;
		static gv_vector2 root_offset;
		static double root_scale_x;

		text_bar() {
			show_extra = true;
			is_checked = false;
			parent = NULL;
		}

		int font_width() { return 6; }

		void add_child(text_bar *p) {
			children.add(p);
			p->parent = this;
		}

		void update_rect_include_children() {
			rect_include_children = rect;
			for (int i = 0; i < children.size(); i++) {
				children[i]->update_rect_include_children();
				rect_include_children.add(children[i]->rect_include_children);
			}
		}

		gv_rect to_disp(gv_rect r) {
			gv_rect rect_disp = r;
			rect_disp.scale_x((float)root_scale_x);
			rect_disp.move(root_offset.x, root_offset.y);
			rect_disp = rect_disp.clip(root_window);
			return rect_disp;
		}

		void render() {
			gv_rect rect_disp;
			rect_disp = to_disp(rect_include_children);
			if (rect_disp.area() < 12)
				return;
			if (rect_disp.width() < 1.0f)
				return;
			for (int i = 0; i < children.size(); i++) {
				children[i]->render();
			}

			rect_disp = to_disp(rect);
			if (rect_disp.is_empty())
				return;
			if (rect_disp.width() < 1.0f)
				return;
			gv_global::debug_draw->draw_rect(rect_disp, gv_color::BLACK(), 0.9f);
			rect_disp.extend(-1);
			gv_color cu_color = color;
			if (is_checked) {
				cu_color = color / 2;
			}

			gv_global::debug_draw->draw_rect(rect_disp, cu_color, 0.9f);
			gv_string_tmp s = title;
			if (show_extra) {
				s << extra_string;
			}
			int max_char = ((int)rect_disp.width() - 4) / font_width();
			if (max_char && s.strlen()) {
				s = s.left(max_char);
				gv_vector2i pos;
				pos.x = (gv_int)rect_disp.left + 4;
				pos.y = (gv_int)rect_disp.top + 4;
				gv_global::debug_draw->draw_string(*s, pos, gv_color::BLACK());
			}
		}
	};

	gv_rect text_bar::root_window;
	gv_vector2 text_bar::root_offset(0, 0);
	double text_bar::root_scale_x = 1;

	class button : text_bar {
	public:
		button() {
			show_extra = false;
			on_clicked = NULL;
			is_check_box = false;
			mouse_botton_down = false;
			bound_var = NULL;
			click_start = false;
			s_button_list.add(this);
		}

		~button() { s_button_list.remove(this); }

		void set_is_checked(bool b) {
			is_check_box = b;
			if (bound_var) {
				*bound_var = b;
			}
		}

		bool get_is_checked() { return is_checked; }

		void tick(float dt) {
			gv_rect rect_disp = rect;
			rect_disp.scale_x((float)root_scale_x);
			rect_disp.move(root_offset.x, root_offset.y);
			rect_disp = rect_disp.clip(root_window);

			bool down = gv_global::input->is_key_down(e_key_lbutton);
			gv_vector2i mouse_pos;
			gv_global::input->get_mouse_pos(mouse_pos);
			gv_vector2 mp;
			mp = mouse_pos;
			bool in_box = rect_disp.inside(mp);
			bool click_down_on_button = down && !mouse_botton_down && in_box;
			bool release_button = !down && mouse_botton_down && in_box;
			if (click_down_on_button) {
				if (!is_check_box) {
					set_is_checked(true);
				}
				click_start = true;
			}
			if (release_button && click_start) {
				if (on_clicked) {
					on_clicked();
				}
				if (is_check_box) {
					set_is_checked(!get_is_checked());
				}
				else {
					set_is_checked(false);
				}
				click_start = false;
			}
			mouse_botton_down = down;
		}

		static void tick_all(float time) {
			for (int i = 0; i < s_button_list.size(); i++) {
				s_button_list[i]->tick(time);
			}
		}

	public:
		static gvt_array<button *> s_button_list;
		boost::function<void()> on_clicked;
		bool is_check_box;
		bool mouse_botton_down;
		bool click_start;
		bool *bound_var;
	};

	gvt_array<button *> button::s_button_list;

	class tool_perf_viewer_2d : public gv_unit_test_with_renderer {
		virtual gv_string name() { return "tool_perf_viewer_2d"; }

		class gv_time_mark_for_display {
		public:
			gv_time_mark_for_display() { GVM_ZERO_ME; }
			int stack_depth() { return m_user_data_ints[1]; }
			int user_index() { return m_user_data_ints[0]; }
			bool operator<(const gv_time_mark_for_display &m) {
				return m_time_stamp < m.m_time_stamp;
			}

			gv_ulong m_time_stamp;
			gv_id m_thread_id;
			gv_id m_marker_name;
			int m_type;
			bool processed;
			gvt_ptr<text_bar> bar;
			union {
				gv_uint m_user_data_uints[3];
				gv_int m_user_data_ints[3];
				gv_float m_user_data_floats[3];
				gv_double m_user_data_double;
				gv_long m_user_data_long;
				struct {
					gv_int m_user_index;
					gv_int m_stack_depth;
					gv_int m_duration;
				};
			};
		};

		struct thread_info {
			static const int c_max_stack_depth = 16;

			thread_info() {
				max_event_stack_depth = 0;
				m_min_time_stamp = gvt_value<gv_ulong>::max();
				m_max_time_stamp = gvt_value<gv_ulong>::min();
				m_event_count = 0;
				m_thread_id = 0;
			}
			gvt_array_fixed<gvt_array<gv_time_mark_for_display>, c_max_stack_depth>
				m_marks;

			int max_event_stack_depth;
			gv_ulong m_min_time_stamp;
			gv_ulong m_max_time_stamp;
			gv_uint m_thread_id;
			gv_int m_event_count;
			gv_id name;
			gvt_ptr<text_bar> bar;
		};

		template <class type_of_string>
		gv_color text_to_color(const type_of_string &s) {
			gv_uint h = gvt_hash(s);
			gv_color c;
			c.fixed32 = h;
			gv_colorf cf = c;
			cf.v /= 1.5f;
			cf.v += 0.4f;
			cf.clamp();
			c = cf;
			return c;
		}

	public:
		gv_string m_file_name;
		gvt_hash_map<gv_id, thread_info> m_threads;
		gvt_array<thread_info *> m_thread_array;
		gvt_hash_map<gv_uint, gv_id> m_thread_name_table;
		gvt_array<gvt_ref_ptr<text_bar>> m_bars;
		gvt_array<gv_int> m_ignored_list;
		gvt_array<gv_int> m_include_list;
		int m_min_event_thread_to_show;

		gv_ulong m_min_time_stamp;
		gv_ulong m_max_time_stamp;
		gv_ulong m_perf_frequency;
		gv_int m_nb_event_rows;

		gv_double m_time_duration_in_ms;
		gv_double m_min_time_point_in_ms;
		gv_double m_max_time_point_in_ms;

		gv_double m_ms_to_pixel;
		gv_double m_last_tick_time;
		gv_double m_delta_time;

		gv_vector2i m_cu_mouse_pos;
		gv_vector2i m_last_mouse_pos;
		bool m_is_drag;

		tool_perf_viewer_2d() {
			m_ms_to_pixel = 10.0;
			m_min_event_thread_to_show = 0;
		}

		gv_double pixel_to_ms(gv_double pixel) {
			return (gv_double)pixel / m_ms_to_pixel;
		}

		gv_double ms_to_pixel(gv_double ms) {
			return (gv_double)(ms * m_ms_to_pixel);
		}

		gv_double perf_count_to_time_ms(gv_ulong s) {
			return (gv_double)s * 1000.0 / (gv_double)m_perf_frequency;
		}

		int bar_height() { return 20; }

		gv_rect build_rect(gv_ulong time_start, gv_ulong time_end, int row) {
			time_start -= m_min_time_stamp;
			time_end -= m_min_time_stamp;
			double t0 = perf_count_to_time_ms(time_start);
			double t1 = perf_count_to_time_ms(time_end);
			gv_rect r;
			r.left = (gv_float)(ms_to_pixel(t0));
			r.right = (gv_float)(ms_to_pixel(t1));
			r.top = (float)row * bar_height();
			r.bottom = r.top + bar_height();
			return r;
		}

		gv_double get_duration(gv_ulong time_start, gv_ulong time_end) {
			double t0 = perf_count_to_time_ms(time_end - time_start);
			return t0;
		}

		gv_time_mark_for_display *march_marker_point(gv_time_mark_for_display *parent,
			gv_time_mark_for_display *start,
			gv_time_mark_for_display *last) {
			text_bar *bar = parent->bar;
			if (!bar)
				return start;
			gv_rect r = bar->rect;
			while (start <= last) {
				if (start->bar) {
					gv_rect r2 = start->bar->rect;
					// skip too early event.
					if (r2.left < r.left) {
						if (parent->m_marker_name == "STAT_TickTime") {
							start = start;
						}
						start++;
						continue;
					}
					else if ((r2.left, r.left, r.right)) {
						if (gvt_between(r2.right, r.left, r.right)) {
							bar->add_child(start->bar);
						}
						else { // something wrong , overlap with parent.skip and break;
							if (parent->m_marker_name == "STAT_TickTime") {
								start = start;
							}
							// start++;
							break;
						}
					}
					else {
						break;
					}
				}
				start++;
			}
			return start;
		}
		// BUG : FIXME need to round up march_marker_point
		void build_bars_hierachy(thread_info &the_thread) {
			text_bar *root = the_thread.bar;
			// find_first_call_stack_with event
			int first_stack = 0;
			for (int i = 0; i < the_thread.max_event_stack_depth; i++, first_stack++) {
				gvt_array<gv_time_mark_for_display> &marks = the_thread.m_marks[i];
				if (marks.size()) {
					break;
				}
			}

			gvt_array<gv_time_mark_for_display> &marks =
				the_thread.m_marks[first_stack];
			for (int i = 0; i < marks.size(); i++) {
				gv_time_mark_for_display *pmark = &the_thread.m_marks[first_stack][i];
				if (!pmark->bar)
					continue;
				root->add_child(pmark->bar);
			}

			for (int stack = first_stack; stack <= the_thread.max_event_stack_depth; stack++) {
				the_thread.m_marks[stack].sort();
			}

			for (int stack = first_stack; stack < the_thread.max_event_stack_depth;
				stack++) {
				gvt_array<gv_time_mark_for_display> &marks = the_thread.m_marks[stack];
				gvt_array<gv_time_mark_for_display> &marks_next =
				the_thread.m_marks[stack + 1];
				gv_time_mark_for_display *pstart_next = marks_next.begin();
				for (int i = 0; i < marks.size(); i++) {
					pstart_next =
						march_marker_point(&marks[i], pstart_next, marks_next.last());
				}
			}
			root->update_rect_include_children();
		}

		void build_bars_one_thread(gv_id name, thread_info &the_thread) {

			{
				text_bar &t = *(new text_bar());
				m_nb_event_rows++;
				t.rect = build_rect(the_thread.m_min_time_stamp,
					the_thread.m_max_time_stamp, m_nb_event_rows++);
				t.title = the_thread.name.string();
				t.extra_string << ": thread " << get_duration(the_thread.m_min_time_stamp,
					the_thread.m_max_time_stamp)
					<< " ms ";
				t.extra_string << ": events " << the_thread.m_event_count;
				t.color = text_to_color(t.title);
				m_thread_array.add(&the_thread);
				the_thread.bar = *m_bars.add(&t);
			}
			for (int cu_stack = 0; cu_stack <= the_thread.max_event_stack_depth;
				cu_stack++) {
				gvt_array<gv_time_mark_for_display> &marks = the_thread.m_marks[cu_stack];

				for (int i = 0; i < marks.size(); i++) {
					gv_time_mark_for_display &mk = marks[i];
					// m_user_data_ints[1] is the stack depth
					int j = i + 1;
					j %= marks.size();
					if (mk.m_type == gv_profiler_marker::e_profile_paired) {
						text_bar &t = *(new text_bar());
						t.rect = build_rect(mk.m_time_stamp, mk.m_time_stamp + mk.m_duration,
							m_nb_event_rows);
						t.title = mk.m_marker_name.string();
						t.extra_string << "(" << mk.user_index() << ")  :"
							<< get_duration(mk.m_time_stamp,
								mk.m_time_stamp + mk.m_duration)
							<< " ms ";
						t.color = text_to_color(t.title);
						text_bar *bar = *m_bars.add(&t);
						mk.bar = bar;
					}
					// older slow tag..
					else if (mk.m_type == gv_profiler_marker::e_profile_start) {
						int searched = 0;
						while (searched < marks.size() - 1) {
							gv_time_mark_for_display &mk2 = marks[j];
							if (mk2.m_type == gv_profiler_marker::e_profile_end &&
								mk2.m_marker_name == mk.m_marker_name &&
								mk2.m_time_stamp >= mk.m_time_stamp &&
								mk2.user_index() == mk.user_index()) {
								//! we find the couple!!
								text_bar &t = *(new text_bar());
								t.rect = build_rect(mk.m_time_stamp, mk2.m_time_stamp,
									m_nb_event_rows);
								t.title = mk.m_marker_name.string();
								t.extra_string
									<< "(" << mk.user_index()
									<< ")  :" << get_duration(mk.m_time_stamp, mk2.m_time_stamp)
									<< " ms ";
								t.color = text_to_color(t.title);
								text_bar *bar = *m_bars.add(&t);
								mk.bar = bar;
								break;
							}
							j++;
							j %= marks.size();
							searched++;
						} // next match test //
						if (searched == marks.size() - 1) {
							// not found
							// GVM_DEBUG_OUT("bad event, not paired!!")
						}

					} // if is start event
					else if (mk.m_type == gv_profiler_marker::e_profile_end &&
						marks.size() == 1) {
						text_bar &t = *(new text_bar());
						t.rect =
							build_rect(m_min_time_stamp, mk.m_time_stamp, m_nb_event_rows);
						t.title = mk.m_marker_name.string();
						t.extra_string << "(" << mk.user_index() << ")  :"
							<< get_duration(m_min_time_stamp, mk.m_time_stamp)
							<< " ms ";
						t.color = text_to_color(t.title);
						text_bar *bar = *m_bars.add(&t);
						mk.bar = bar;
						break;
					}
				} // next couple
				if (marks.size())
					m_nb_event_rows++;
			} // next stack
		}

		void build_bars() {
			gvt_hash_map<gv_id, thread_info>::iterator it = m_threads.begin();
			while (it != m_threads.end()) {
				if (!it.is_empty()) {
					gv_id name(*it.get_key());
					thread_info &the_thread = *it;
					if (m_ignored_list.find(the_thread.m_thread_id)) {
						it++;
						continue;
					}
					if (m_include_list.size() &&
						!m_include_list.find(the_thread.m_thread_id)) {
						it++;
						continue;
					}
					if (the_thread.m_event_count < m_min_event_thread_to_show) {
						it++;
						continue;
					}
					build_bars_one_thread(name, the_thread);
					build_bars_hierachy(the_thread);
				}
				it++;
			} // next iterator in hash
		}

		void add_marker(gv_time_mark_for_display &mark) {
			m_min_time_stamp = gvt_min(m_min_time_stamp, mark.m_time_stamp);
			m_max_time_stamp = gvt_max(m_max_time_stamp, mark.m_time_stamp);

			if (mark.m_type == gv_profiler_marker::e_profile_start ||
				mark.m_type == gv_profiler_marker::e_profile_end ||
				mark.m_type == gv_profiler_marker::e_profile_paired) // perf event
			{

				if (mark.stack_depth() >= thread_info::c_max_stack_depth) {
					return;
				}
				thread_info &the_thread = m_threads[mark.m_thread_id];
				the_thread.max_event_stack_depth =
					gvt_max(the_thread.max_event_stack_depth, mark.m_user_data_ints[1]);
				the_thread.m_min_time_stamp =
					gvt_min(the_thread.m_min_time_stamp, mark.m_time_stamp);
				the_thread.m_max_time_stamp =
					gvt_max(the_thread.m_max_time_stamp, mark.m_time_stamp);
				if (mark.m_type == gv_profiler_marker::e_profile_paired) {
					gv_ulong end_time = mark.m_time_stamp + mark.m_duration;
					the_thread.m_min_time_stamp =
						gvt_min(the_thread.m_min_time_stamp, end_time);
					the_thread.m_max_time_stamp =
						gvt_max(the_thread.m_max_time_stamp, end_time);
				}
				int cu_stack = mark.stack_depth();
				if (!the_thread.m_marks[cu_stack].size()) {
					the_thread.m_marks[cu_stack].reserve(1000);
					the_thread.name = mark.m_thread_id;
				}
				the_thread.m_marks[cu_stack].add(mark);
				the_thread.m_event_count++;
			};
		}

		void read_xml() {
			gv_xml_parser parser;
			bool file_find = parser.load_file(*m_file_name);
			if (!file_find) {
				std::cout << "can't find file:" << *m_file_name;
				m_last_tick_time = gv_global::time->get_sec_from_start();
				m_delta_time = 0;
				return;
			}
			parser.read_element_open("gv_performance_snap_shot", false);
			int version;
			parser.read_attribute("version", version);
			int event_count;
			parser.read_attribute("event_count", event_count);
			parser.read_attribute("perf_frequency", m_perf_frequency);
			parser.read_element_open();

			for (int i = 0; i < event_count; i++) {
				parser.read_element_open("mark", false);
				//<mark	time="17073639"	thread="main"	type="1"
				// name="test_profile"	user_data0="0"	user_data1="0"
				// user_data2="0"	/>
				gv_time_mark_for_display mark;
				parser.read_attribute("time", mark.m_time_stamp);
				parser.read_attribute("thread", mark.m_thread_id);
				parser.read_attribute("type", mark.m_type);
				parser.read_attribute("name", mark.m_marker_name);
				parser.read_attribute("user_data0", mark.m_user_data_uints[0]);
				parser.read_attribute("user_data1", mark.m_user_data_uints[1]);
				parser.read_attribute("user_data2", mark.m_user_data_uints[2]);
				parser.read_element_close();
				add_marker(mark);
			}
			parser.read_element_close("gv_performance_snap_shot");
		}

		void read_marker(gvi_stream &s, gv_time_mark_for_display &mark) {
			s >> mark.m_time_stamp; //  mark.TimeStamp;
			gv_uint tid;
			gv_char type;
			s >> tid;
			s >> type;
			mark.m_type = type;
			s >> mark.m_marker_name;
			s >> mark.m_user_data_uints[0];
			s >> mark.m_user_data_uints[1];
			s >> mark.m_user_data_uints[2];
			if (mark.m_type == gv_profiler_marker::e_profile_thread_info) {
				gv_string_tmp thread_name;
				thread_name << mark.m_marker_name;
				thread_name << "_" << tid;
				m_thread_name_table[tid] = *thread_name;
				m_threads.add(gv_id(*thread_name), thread_info());
				thread_info *pthread_info = m_threads.find(gv_id(*thread_name));
				pthread_info->name = *thread_name;
				pthread_info->m_thread_id = tid;
				// m_thread_array.add(pthread_info);
				return;
			}

			gv_id *pthread_name = m_thread_name_table.find(tid);
			if (!pthread_name) {
				// only in older file
				gv_string_tmp thread_name;
				thread_name << "thread_" << tid;
				m_thread_name_table[tid] = *thread_name;
				pthread_name = m_thread_name_table.find(tid);
			}
			mark.m_thread_id = *pthread_name;
		}

		void read_bin() {
			gvi_stream *ps = gv_global::fm->open_binary_file_for_read(m_file_name);
			if (!ps)
				return;
			gvi_stream &s = *ps;
			gv_string tag;
			s >> tag;

			if (tag == "pf") {
				gv_int version;
				s >> version;
				gv_int count;
				s >> count;
				s >> m_perf_frequency;
				// count = gvt_min(count, 512 * 1024);//to be optimized
				for (int i = 0; i < count; i++) {
					gv_time_mark_for_display mark;
					read_marker(s, mark);
					if (mark.m_type == gv_profiler_marker::e_profile_end_of_file) {
						break;
					}
					if (mark.m_marker_name == "TEST") {
						i = i;
					}
					if (mark.m_type != gv_profiler_marker::e_profile_thread_info) {
						add_marker(mark);
					}
				}
			}
			delete ps;
		}

		virtual void initialize() {

			m_nb_event_rows = 0;

			m_last_mouse_pos = m_cu_mouse_pos = gv_vector2i(0, 0);
			m_min_time_stamp = gvt_value<gv_ulong>::max();
			m_max_time_stamp = gvt_value<gv_ulong>::min();
			m_is_drag = false;
			bool bin = false;
			if (gv_global::command_line_options.size()) {
				m_file_name << gv_global::command_line_options[0];
			}
			else
				m_file_name = "../profiler.xml";
			if (gv_global::command_line_options.find("bin")) {
				bin = true;
			}
			int idx;
			if (gv_global::command_line_options.find("ign", idx)) {
				gv_string ss;
				ss = gv_global::command_line_options[idx + 1];
				gvt_array<gv_string> results;
				ss.split(results, ":");
				for (int i = 0; i < results.size(); i++) {
					int ii;
					results[i] >> ii;
					m_ignored_list.add(ii);
				}
			}

			if (gv_global::command_line_options.find("inc", idx)) {
				gv_string ss;
				ss = gv_global::command_line_options[idx + 1];
				gvt_array<gv_string> results;
				ss.split(results, ":");
				for (int i = 0; i < results.size(); i++) {
					int ii;
					results[i] >> ii;
					m_include_list.add(ii);
				}
			}

			if (gv_global::command_line_options.find("filter", idx)) {
				gv_string ss;
				ss = gv_global::command_line_options[idx + 1];
				ss >> m_min_event_thread_to_show;
			};
			m_bars.reserve(1024 * 128);
			if (!bin)
				read_xml();
			else
				read_bin();

			build_bars();
			m_last_tick_time = gv_global::time->get_sec_from_start();
			m_delta_time = 0;
		};
		virtual void render() {
			gv_double cu = gv_global::time->get_sec_from_start();
			m_delta_time = gvt_clamp(cu - m_last_tick_time, 0.01, 0.1);
			m_last_tick_time = cu;

			gv_string_tmp s = ">>";
			s << "press enter to quite ,tick_count:" << tick_count
				<< "fps: " << 1.0 / m_delta_time;
			gv_rect window = gv_global::debug_draw->get_window_rect();
			text_bar::root_window = window;

			gv_global::debug_draw.get()->draw_string(*s, gv_vector2i(20, 20),
				gv_color::RED());

			{
				bool down = gv_global::input->is_key_down(e_key_lbutton);
				if (m_is_drag && !down) {
					gv_global::debug_draw->release_mouse();
				}
				if (down && !m_is_drag) {
					gv_global::debug_draw->capture_mouse();
				}
				m_is_drag = down;

				m_last_mouse_pos = m_cu_mouse_pos;
				gv_global::input->get_mouse_pos(m_cu_mouse_pos);
				if (m_is_drag) {
					gv_vector2i delta = m_cu_mouse_pos - m_last_mouse_pos;
					text_bar::root_offset.x += gvt_clamp(delta.x, -100, +100);
					text_bar::root_offset.y += gvt_clamp(delta.y, -100, +100);
				}
			}

			{
				gv_double last_scale = text_bar::root_scale_x;
				text_bar::root_scale_x =
					text_bar::root_scale_x *
					gvt_clamp<double>((1.0 +
					(double)gv_global::input->get_mouse_wheel_delta() /
						1000.f * (m_delta_time / 0.01)),
						0, 2);
				text_bar::root_scale_x =
					gvt_clamp<double>(text_bar::root_scale_x, 0.001, 10000000.0);
				//(400-offset_x)/root_scale_x*scale_new-400=offset_new
				gv_double screen_h_w = window.width() / 2;
				gv_double new_offset = screen_h_w -
					(screen_h_w - text_bar::root_offset.x) /
					last_scale * text_bar::root_scale_x;
				text_bar::root_offset.x = (float)new_offset;
				gv_global::input->set_mouse_wheel_delta(0);
			}
			// to do:  high level visibility test
			for (int i = 0; i < m_thread_array.size(); i++) {
				m_thread_array[i]->bar->render();
			}
			gv_global::debug_draw->draw_line(
				gv_vector2(window.width() / 2.f, 0),
				gv_vector2(window.width() / 2.f, window.height()), gv_color::RED_B(),
				gv_color::RED_D());
			tick_count++;
		};
		virtual bool is_finished() {
			// if (tick_count<m_max_frame) return false;
			// return true;
			if (gv_global::input->is_key_down(e_key_return))
				return true;
			return false;
		}

		virtual void destroy() {
			m_file_name.clear();
			m_bars.clear();
			m_threads.clear();
			m_thread_name_table.clear();
			gv_id::static_purge();
		}
	} test;
	gv_unit_test_with_renderer *ptest = &test;
};
