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

inline int x = 5;

int main()
{

//	gsl::
	cout << (float*)x << endl;
	FallThrough(2);
	CheckResult();

	auto[num, text] = TupleTest();
	cout << num << " " << text << endl;

	CMutableLambda t;
	t.CaptureByValue();
	t.CaptureByRef();

	vector<int> v{ 1,2,3 };

	for (auto &&x : v)
	{
		cout << x << endl;
	}



	if constexpr (sizeof(int) > 4)
	{
		cout << "test" << endl;
	}

	cout << v.size() << endl;
    return 0;
}

