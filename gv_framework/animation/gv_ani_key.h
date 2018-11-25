#pragma once
namespace gv
{
//============================================================================================
//								:
//============================================================================================
template < class type_of_key, class type_of_time >
class gvt_ani_key
{
public:
	gvt_ani_key()
	{
	}
	gvt_ani_key(const type_of_key& key, const type_of_time& time)
	{
		m_key = key;
		m_time = time;
	}
	gvt_ani_key(const type_of_time& time)
	{
		m_time = time;
	}
	~gvt_ani_key()
	{
	}
	bool operator<(const gvt_ani_key& key) const
	{
		return m_time < key.m_time;
	}
	bool operator==(const gvt_ani_key& key) const
	{
		return m_time == key.m_time;
	}
	type_of_key& get_key()
	{
		return m_key;
	}
	type_of_time& get_time()
	{
		return m_time;
	}
	const type_of_key& get_key() const
	{
		return m_key;
	}
	const type_of_time& get_time() const
	{
		return m_time;
	}

public:
	type_of_key m_key;
	type_of_time m_time;
};
//============================================================================================
//								:
//============================================================================================
template < class type_of_key, class type_of_time >
class gvt_ani_key_track
{
public:
	typedef gvt_ani_key< type_of_key, type_of_time > type_of_ani_key;
	gvt_ani_key_track(){};

	void insert_key(type_of_time time, const type_of_key& key)
	{
		m_keys.insert_in_sorted_array(type_of_ani_key(key, time));
	}
	type_of_key get(type_of_time time, bool loop = true) const
	{
		if (!m_keys.size())
			return gvt_indentity< type_of_key >();
		if (m_keys.size() == 1)
			return m_keys[0].m_key;
		if (loop)
		{
			time = gvt_div_remain(time, get_duration());
		}
		type_of_ani_key the_key(time);
		gv_int index;
		if (m_keys.find_in_sorted_array(the_key, index))
		{
			return m_keys[index].m_key;
		}
		index = -index - 1;
		if (index == 0)
			return m_keys[0].m_key;
		if (index >= m_keys.size())
		{
			return m_keys.last()->m_key;
		}
		type_of_time d = m_keys[index].m_time - m_keys[index - 1].m_time;
		if (gvt_is_almost_zero(d))
		{
			return m_keys[index].m_key;
		}
		else
		{
			gv_float alpha =
				((gv_float)(time - m_keys[index - 1].m_time)) / (gv_float)d;
			alpha = gvt_clamp(alpha, 0.f, 1.f);
			return gvt_interpolate(alpha, m_keys[index - 1].m_key,
								   m_keys[index].m_key);
		}
	}

	type_of_time get_duration() const
	{
		if (m_keys.size())
			return m_keys.last()->m_time;
		else
			return 0;
	};

	void digest_equal_keys()
	{
		for (int i = 1; i < this->m_keys.size() - 1; i++)
		{
			if (this->m_keys[i].m_key == this->m_keys[i - 1].m_key)
			{
				m_keys.erase(i);
				i--;
			}
		}
	}
	void clear()
	{
		m_keys.clear();
	}
	gv_int size() const
	{
		return m_keys.size();
	}

public:
	gvt_array< type_of_ani_key > m_keys;

private:
	gvt_ani_key_track(const gvt_ani_key_track& t);
};

typedef gvt_ani_key< gv_vector3, gv_float > gv_ani_pos_key;
typedef gvt_ani_key< gv_vector3, gv_float > gv_ani_scale_key;
typedef gvt_ani_key< gv_quat, gv_float > gv_ani_rot_key;
typedef gvt_ani_key< gv_float, gv_float > gv_ani_float_key;
typedef gvt_ani_key_track< gv_vector3, gv_float > gv_ani_pos_track;
typedef gvt_ani_key_track< gv_quat, gv_float > gv_ani_rot_track;
typedef gvt_ani_key_track< gv_vector3, gv_float > gv_ani_scale_track;
typedef gvt_ani_key_track< gv_float, gv_float > gv_ani_float_track;
}