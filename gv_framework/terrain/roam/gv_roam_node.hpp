namespace gv
{
//============================================================================================
//								:
//============================================================================================
struct node_pad
{
	gv_char pad[64];
};
GV_STATIC_ASSERT(sizeof(node_pad) >= sizeof(gv_roam_node));
gvt_stack_static< node_pad, ROAM_MAX_DISPLAY_NODE > s_node_stack;
void clear_roam_node_stack()
{
	s_node_stack.pop_all();
}
inline gv_int get_chuck_vertex_index(gv_vector2i& v)
{
	return v.get_x() + v.get_y() * (GV_ROAM_PATCH_SIZE + 1);
}

void* gv_roam_node::operator new(size_t size)
{
	s_node_stack.push();
	return (void*)&s_node_stack.top();
};
void gv_roam_node::operator delete(void* p){};

gv_roam_node::gv_roam_node()
{
}

gv_roam_node::~gv_roam_node()
{
}

void gv_roam_node::reset()
{
	m_left_neighbour = 0;
	m_right_neighbour = 0;
	m_basic_neighbour = 0;
	m_left_child = 0;
	m_right_child = 0;
}

void gv_roam_node::force_split()
{
	if (this->get_left_child())
	{
		return;
	}
	gv_roam_node* pbn = this->get_basic_neighbour();
	if (pbn == 0)
	{
		this->split();
	}
	else if (pbn->get_basic_neighbour() == this)
	{

		this->split();
		pbn->split();

		pbn->get_left_child()->set_right_neighbour(this->get_right_child());
		pbn->get_right_child()->set_left_neighbour(this->get_left_child());
		this->get_right_child()->set_left_neighbour(pbn->get_left_child());
		this->get_left_child()->set_right_neighbour(pbn->get_right_child());
	}

	else // need recursive split
	{
		pbn->force_split();
		pbn = this->get_basic_neighbour();
		pbn->split();
		this->split();

		pbn->get_left_child()->set_right_neighbour(this->get_right_child());
		pbn->get_right_child()->set_left_neighbour(this->get_left_child());
		this->get_right_child()->set_left_neighbour(pbn->get_left_child());
		this->get_left_child()->set_right_neighbour(pbn->get_right_child());
	}
}

void gv_roam_node::split()
{

	gv_roam_node* plc = new gv_roam_node;
	gv_roam_node* prc = new gv_roam_node;
	this->set_left_child(plc);
	this->set_right_child(prc);

	gv_int v0 = this->m_vi[0];
	gv_int v1 = this->m_vi[1];
	gv_int v2 = this->m_vi[2];
	gv_int newindex = (v1 + v2) / 2;

	plc->set_basic_neighbour(this->get_left_neighbour());
	plc->set_left_neighbour(prc); // right neighbor not decided yet
	plc->set_right_neighbour(NULL);
	plc->set_left_child(NULL);
	plc->set_right_child(NULL);

	plc->m_vi[0] = newindex;
	plc->m_vi[1] = this->m_vi[2]; //
	plc->m_vi[2] = this->m_vi[0]; //

	// assigned right  child
	prc->set_basic_neighbour(this->get_right_neighbour());
	prc->set_right_neighbour(plc); // left neighbor not decided yet
	prc->set_left_neighbour(NULL);
	prc->set_left_child(NULL);
	prc->set_right_child(NULL);

	prc->m_vi[0] = newindex;
	prc->m_vi[1] = this->m_vi[0];
	prc->m_vi[2] = this->m_vi[1];

	gv_roam_node* pn;
	pn = get_left_neighbour();
	if (pn)
	{
		if (pn->get_right_neighbour() == this)
		{
			pn->set_right_neighbour(plc);
		}
		else if (pn->get_left_neighbour() == this)
		{
			pn->set_left_neighbour(plc);
		}
		else
		{
			pn->set_basic_neighbour(plc);
		}
	}

	// right
	pn = get_right_neighbour();
	if (pn)
	{
		if (pn->get_right_neighbour() == this)
		{
			pn->set_right_neighbour(prc);
		}
		else if (pn->get_left_neighbour() == this)
		{
			pn->set_left_neighbour(prc);
		}
		else
		{
			pn->set_basic_neighbour(prc);
		}
	}
	plc->m_idx_in_full_tree = (this->m_idx_in_full_tree << 1);
	prc->m_idx_in_full_tree = (this->m_idx_in_full_tree << 1) + 1;
}

void gv_roam_node::make_root(gv_vector2i& v0, gv_vector2i& v1,
							 gv_vector2i& v2)
{
	this->set_basic_neighbour(0);
	this->set_left_child(0);
	this->set_left_neighbour(0);
	this->set_right_child(0);
	this->set_right_neighbour(0);

	// this->set_root(this);
	this->set_idx_in_full_tree(1);
	this->m_vi[0] = get_chuck_vertex_index(v0);
	this->m_vi[1] = get_chuck_vertex_index(v1);
	this->m_vi[2] = get_chuck_vertex_index(v2);
}

bool gv_roam_node::fill_index(gv_int* pindex, gv_int& length,
							  gv_int max_length)
{
	if (this->get_left_child())
	{
		if (!this->get_left_child()->fill_index(pindex, length, max_length))
			return false;
		if (!this->get_right_child()->fill_index(pindex, length, max_length))
			return false;
	}
	else
	{
		if (length + 3 >= max_length)
			return false;
		pindex[length++] = this->m_vi[0];
		pindex[length++] = this->m_vi[1];
		pindex[length++] = this->m_vi[2];
	};
	return true;
};

//============================================================================================
//								:
//============================================================================================

GVM_IMP_STRUCT(gv_roam_node)
GVM_VAR(gvt_ptr< gv_roam_node >, m_left_child)
GVM_VAR(gvt_ptr< gv_roam_node >, m_right_child)
GVM_STATIC_ARRAY(gv_int, 3, m_vi)
GVM_VAR(gv_int, m_idx_in_full_tree)
GVM_VAR_ATTRIB_SET(transient)
GVM_VAR(gvt_ptr< gv_roam_node >, m_left_neighbour)
GVM_VAR(gvt_ptr< gv_roam_node >, m_right_neighbour)
GVM_VAR(gvt_ptr< gv_roam_node >, m_basic_neighbour)
GVM_END_STRUCT
}