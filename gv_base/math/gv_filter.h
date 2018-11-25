#pragma once

namespace gv
{
//=======================================================================================
//
//=======================================================================================
template < class T, class C = gv_float, int history_size = 4 >
class gvt_filtered_blend
{
protected:
	gv_uint m_current_size;
	gv_uint m_cu_index;
	T m_history[history_size];
	T m_filtered;

public:
	typedef C type_of_factor;
	typedef gvt_value< C > value_trait;
	gvt_filtered_blend()
	{
		GVM_ZERO_ME;
	}

	T get_result()
	{
		return m_filtered;
	}

	void append(const T& t)
	{
		m_cu_index = (m_current_size++) % history_size;
		m_history[m_cu_index] = t;
	}

	T calculate_filtered()
	{
		type_of_factor f_normalized_confidence[history_size];
		type_of_factor f_confidence_sum;
		type_of_factor f_cur_normalized_confidence = 1;
		f_confidence_sum = f_cur_normalized_confidence;
		for (gv_uint i = 0; i < history_size; ++i)
		{
			f_normalized_confidence[i] = value_trait::one();
			f_confidence_sum += f_normalized_confidence[i];
		}
		if (f_confidence_sum > 0)
		{
			f_cur_normalized_confidence /= f_confidence_sum;
			for (gv_uint i = 0; i < history_size; ++i)
			{
				f_normalized_confidence[i] /= f_confidence_sum;
			}
		}
		else
		{
			f_cur_normalized_confidence = 1 / (type_of_factor)history_size;
			for (gv_uint i = 0; i < history_size; ++i)
			{
				f_normalized_confidence[i] = 1 / (type_of_factor)history_size;
			}
		}
		m_filtered *= f_cur_normalized_confidence;
		for (gv_uint i = 0; i < history_size; ++i)
		{
			gv_uint prev_skeleton_idx =
				(m_cu_index - (i + 1) + history_size) % history_size;
			m_filtered = m_filtered +
						 m_history[prev_skeleton_idx] * f_normalized_confidence[i];
		}
		return m_filtered;
	};
};
//=======================================================================================
//
//=======================================================================================
template < class T, class C = gv_float, int history_size = 4 >
class gvt_filtered_taylor
{
protected:
	typedef C type_of_factor;
	typedef gvt_value< C > value_trait;

	gv_uint m_current_size;
	gv_uint m_cu_index;
	T m_filtered;
	T m_pos[history_size];
	T m_est_vel[history_size];
	T m_est_vel2[history_size];
	T m_est_vel3[history_size];
	type_of_factor m_alpha_coeff;

public:
	gvt_filtered_taylor()
	{
		GVM_ZERO_ME;
		m_alpha_coeff = (type_of_factor)0.3f;
	}

	T get_result()
	{
		return m_filtered;
	}

	void reset()
	{
		GVM_ZERO_ME;
		m_alpha_coeff = (type_of_factor)0.3f;
	}

	void append(const T& t)
	{
		m_cu_index = (m_current_size++) % history_size;
		m_pos[m_cu_index] = t;
	}

	void set_alpha_coeff(type_of_factor f)
	{
		m_alpha_coeff = f;
	}

	T calculate_filtered()
	{
		type_of_factor f_beta_coeff =
			(m_alpha_coeff * m_alpha_coeff) / (2 - m_alpha_coeff);
		gv_uint prev = (m_cu_index - 1 + history_size) % history_size;
		gv_uint cur = m_cu_index;

		T predicted, error, actual;
		gvt_vector4< type_of_factor > constants(
			0, value_trait::one(), (type_of_factor)0.5f, (type_of_factor)0.1667f);

		if (m_current_size <= 1)
		{
			m_filtered = m_pos[cur];
			m_est_vel[cur] = 0;
			m_est_vel2[cur] = 0;
			m_est_vel3[cur] = 0;
		}
		else
		{
			actual = m_pos[prev] - m_pos[cur];

			predicted = m_pos[prev] + m_est_vel[prev];
			predicted = predicted +
						m_est_vel2[prev] * (constants.y * constants.y * constants.z);
			predicted = predicted +
						m_est_vel3[prev] *
							(constants.y * constants.y * constants.y * constants.w);
			error = m_pos[cur] - predicted;

			m_filtered = m_pos[prev] = predicted + error * m_alpha_coeff;
			m_est_vel[cur] = m_est_vel[prev] + error * f_beta_coeff;
			m_est_vel2[cur] = m_est_vel[cur] - m_est_vel[prev];
			m_est_vel3[cur] = m_est_vel2[cur] - m_est_vel2[prev];
		}
		return m_filtered;
	}
};
}
