#pragma once

namespace gv
{
class gv_unit_test_with_renderer
{
public:
	gv_unit_test_with_renderer()
	{
		m_next = NULL;
	}
	virtual ~gv_unit_test_with_renderer()
	{
	}
	virtual gv_string name()
	{
		return "test";
	}
	virtual gv_string help()
	{
		return "a empty test";
	}
	virtual void initialize(){};
	virtual void destroy(){};
	virtual void update(gv_float dt){};
	virtual void render(){};
	virtual bool is_finished()
	{
		return true;
	}
	virtual gv_unit_test_with_renderer* next()
	{
		return m_next;
	}
	virtual void set_next(gv_unit_test_with_renderer* p)
	{
		m_next = p;
	}

protected:
	gv_unit_test_with_renderer* m_next;
};
};