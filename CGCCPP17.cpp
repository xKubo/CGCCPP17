// CGCCPP17.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <vector>
#include <tuple>
#include <string>
#include <functional>
#include <unordered_map>
#include <variant>
#include <optional>
#include <any>
#include <thread>

#include <GSL/gsl>

using namespace std;

void FallThrough(int x)
{
	switch (x)
	{
	case 1 : 
		cout << "x == 1" << endl;
		[[fallthrough]];	 
	case 2 :
		cout << "x is 1 or 2" << endl;
	default:
		cout << "x is something else";
	}
}

struct CMutableLambda
{
	/// A lambda can modify its variables, only when declared as mutable
	/// A lambda can capture this by value = as if (CMutableLambda l = *this;, ... 
	void CaptureByValue()
	{
		auto l1 = [*this]() mutable { x++; };
		l1();
		cout << "ByVal:" << x << endl;
	}

	void CaptureByRef()
	{
		auto l1 = [this] { x++; };
		l1();
		cout << "ByRef:" << endl;
	}

	int x = 5;
};

[[nodiscard]] bool CheckResult()
{
	cout << "Test" << endl;
	return true;
}


tuple<int, string> TupleTest()
{
	return { 42, "Harley Street" };
}

struct alignas(64) AlignedClass
{
	char c;
	char d;
};

/*
struct ConstExpr
{
	constexpr ConstExpr(int Value) : m_Value(Value)
	{

	}

	constexpr int GetValue()
	{
		return m_Value;
	}

private:
	int m_Value;
};

void ConstExprTest()
{
	ConstExpr e(5);
	char Data[e.GetValue()];
	cout << "SizeOf=" << sizeof(Data) << endl;
}*/

inline int x = 5;

template <typename T>
inline T Convert(const string& s)
{
	if constexpr (std::is_integral_v<T>)
		return std::atoi(s.c_str());
	else
		return s;
}

template <typename T>
using MyVec = std::vector<T>;

constexpr int GetValue()
{
	return 42 + 2*sizeof(int);
}

void TestArray()
{
	constexpr int x = 42;
	char Data[GetValue()];
	cout << sizeof(Data) << endl;
}

int main()
{
	TestArray();
	cout << "AlignOf=" << alignof(AlignedClass) << endl;
	cout << "Convert: " << Convert<int>("42") + 5 << " " << Convert<string>("abs") << endl;

	cout << "Raw string literals : a\\b" << " " << R"(a\b)" << endl;

	this_thread::sleep_for(3s);

	cout << (float*)x << endl;
	FallThrough(2);
	CheckResult();

	auto[num, text] = TupleTest();
	cout << num << " " << text << endl;

	CMutableLambda t;
	t.CaptureByValue();
	t.CaptureByRef();

	MyVec<int> v{ 1,2,3 };

	int BinNum = 0b0'1011'1001;
	cout << "Bin=" << BinNum << endl;

	cout << u8"Mít" << endl;

    return 0;
}

