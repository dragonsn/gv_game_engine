#pragma once
namespace gv
{
template <
	class T1, class T2, int size_of_map = 256,
	class policy_memory = gvt_policy_memory_default< gvt_hash_pair< T1, T2 > >,
	class policy_thread_mode = gvt_policy_thread_mode< gvt_hash_pair< T2, T1 > > >
class gvt_dictionary
{
public:
	gvt_dictionary(){};
	~gvt_dictionary(){};

public:
	typedef gvt_hash_map< T1, T2, size_of_map, policy_memory, policy_thread_mode >
		type_of_forward_map;
	typedef gvt_hash_map< T2, T1, size_of_map, policy_memory, policy_thread_mode >
		type_of_backward_map;
	typedef typename type_of_forward_map::const_iterator const_iterator;
	typedef typename type_of_forward_map::iterator iterator;

	inline iterator begin()
	{
		return m_lookup_table1.begin();
	}
	inline const_iterator begin() const
	{
		return m_lookup_table1.begin();
	}
	inline iterator end()
	{
		return m_lookup_table1.end();
	}
	inline const_iterator end() const
	{
		return m_lookup_table1.end();
	}

public:
	gv_bool erase(const T1& key1)
	{
		gv_bool ret = false;
		T2* pt2 = this->m_lookup_table1.find(key1);
		if (pt2)
		{
			this->m_lookup_table1.erase(key1);
			this->m_lookup_table2.erase(*pt2);
			ret = true;
		}
		return ret;
	};

	gv_bool erase(const T2& key2)
	{
		gv_bool ret = false;
		T1* pt1 = this->m_lookup_table2.find(key2);
		if (pt1)
		{
			this->m_lookup_table2.erase(key2);
			this->m_lookup_table1.erase(*pt1);
			ret = true;
		}
		return ret;
	};

	gv_int query_all(gvt_array< T1 >& a, gvt_array< T2 >& b)
	{
		gv_int ret = 0;
		ret = this->m_lookup_table1.query_all(a, b);
		return ret;
	};

	T2& operator[](const T1& key)
	{
		T2* pret = NULL;
		pret = this->m_lookup_table1.find(key);
		GV_ASSERT(pret);
		return *pret;
	}

	T1& operator[](const T2& key)
	{
		T1* pret = NULL;
		pret = this->m_lookup_table2.find(key);
		GV_ASSERT(pret);
		return *pret;
	}

	const T2& operator[](const T1& key) const
	{
		const T2* pret = NULL;
		pret = this->m_lookup_table1.find(key);
		GV_ASSERT(pret);
		return *pret;
	}

	const T1& operator[](const T2& key) const
	{
		const T1* pret = NULL;
		pret = this->m_lookup_table2.find(key);
		GV_ASSERT(pret);
		return *pret;
	}

	gv_bool add_pair(const T1& t1, const T2& t2)
	{
		gv_bool ret = false;
		// if (!&((*this)[t1] ) && !&((*this)[t2]))
		if (!this->m_lookup_table1.find(t1))
		{
			this->m_lookup_table1.add(t1, t2);
			this->m_lookup_table2.add(t2, t1);
			ret = true;
		}
		else
		{
			this->m_lookup_table1[t1] = t2;
			this->m_lookup_table2[t2] = t1;
			ret = true;
		}
		return ret;
	}

	gv_bool erase_pair(const T1& t1, const T2& t2)
	{
		gv_bool ret = false;
		T1* p1;
		T2* p2;
		p1 = &(*this)[t2];
		p2 = &(*this)[t1];
		if (p1 && p2 && *p1 == t1 && *p2 == t2)
		{
			this->m_lookup_table1.erase(t1);
			this->m_lookup_table2.erase(t2);
			ret = true;
		}
		return ret;
	}

	gv_bool find(const T1& t1, T2& t2) const
	{
		const T2* pret = NULL;
		pret = this->m_lookup_table1.find(t1);
		if (pret)
		{
			t2 = *pret;
			return true;
		}
		return false;
	}

	gv_bool exist(const T1& t1) const
	{
		return this->m_lookup_table1.find(t1) != NULL;
	}

	gv_bool find(const T2& t2, T1& t1) const
	{
		const T1* pret = NULL;
		pret = this->m_lookup_table2.find(t2);
		if (pret)
		{
			t1 = *pret;
			return true;
		}
		return false;
	}

	void empty()
	{
		this->m_lookup_table1.clear();
		this->m_lookup_table2.clear();
	}

private:
	type_of_forward_map m_lookup_table1;
	type_of_backward_map m_lookup_table2;
};
}