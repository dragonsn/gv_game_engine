namespace gv
{
gv_ani_set::~gv_ani_set(){

};
gv_ani_sequence* gv_ani_set::get_sequence(const gv_id& id)
{
	for (int i = 0; i < this->m_sequences.size(); i++)
		if (m_sequences[i]->get_name_id() == id)
			return m_sequences[i];
	return NULL;
};
void gv_ani_set::add_sequence(gv_ani_sequence* sq)
{
	gv_ani_sequence* old = get_sequence(sq->get_name_id());
	if (old)
	{
		m_sequences.remove(old);
		get_sandbox()->delete_object(old);
	}
	if (sq->get_owner() != this)
	{
		sq->set_owner(this);
	};
	this->m_sequences.push_back(sq);
};

void gv_ani_set::compress_all(gv_float ratio)
{
	for (int i = 0; i < this->get_nb_sequence(); i++)
	{
		this->m_sequences[i]->compress(ratio);
	}
};
GVM_IMP_CLASS(gv_ani_set, gv_resource)
GVM_VAR(gvt_array< gvt_ref_ptr< gv_ani_sequence > >, m_sequences)
GVM_END_CLASS
}