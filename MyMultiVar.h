#pragma once

struct MyMultiVar
{
	int x = 42;
};

namespace std
{
	template <>
	class tuple_size<MyMultiVar> : public integral_constant<size_t, 3>
	{

	};

	template <>
	class tuple_element<0, MyMultiVar>
	{
	public:
		typedef const int type;
	};

	template <>
	class tuple_element<1, MyMultiVar>
	{
	public:
		typedef const int type;
	};

	template <>
	class tuple_element<2, MyMultiVar>
	{
	public:
		typedef const int type;
	};
}



template <size_t Idx>
inline const int& get(const MyMultiVar &m)
{
	return m.x;
}



inline MyMultiVar Return() { MyMultiVar m; return m; }