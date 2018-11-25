#pragma once

#include "gv_ani_track.h"
#include "gv_ani_sequence.h"

namespace gv
{
class gv_ani_sequence;

class gv_ani_set : public gv_resource
{
public:
	GVM_DCL_CLASS_WITH_CONSTRUCT(gv_ani_set, gv_resource)
	~gv_ani_set();
	gv_ani_sequence* get_sequence(const gv_id& id);
	gv_ani_sequence* get_sequence(int index)
	{
		if (index >= m_sequences.size())
			return NULL;
		return m_sequences[index];
	};
	gv_int get_nb_sequence()
	{
		return m_sequences.size();
	};
	void add_sequence(gv_ani_sequence* sq);
	void remove_sequence(gv_ani_sequence* sq)
	{
		m_sequences.remove(sq);
	};
	void compress_all(gv_float ratio = 1.0f);

protected:
	gvt_array< gvt_ref_ptr< gv_ani_sequence > > m_sequences;
};
};