// CGCCPP17.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <vector>
#include <tuple>
#include <string>
#include <map>
#include <functional>
#include <unordered_map>
#include <variant>
#include <optional>
#include <any>
#include <thread>

#include <filesystem>

#include <execution>

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
	/// A lambda can capture "this" by value = as if (CMutableLambda l = *this;, ... 
	void CaptureByValue()
	{
		auto l1 = [*this]() mutable { x++; cout << "x is modified = " << x << endl; };
		l1();
		cout << "x in the class = " << x << endl;
	}

	void CaptureByRef()
	{
		auto l1 = [this] { x++; };
		l1();
		cout << "ByRef:" << endl;
	}

	int x = 5;
};

[[nodiscard]] bool NoDiscardTest()
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

constexpr int GetRGBImageSize(int W, int H)
{
	return 3*W*H;
}

void TestConstexpr()
{
	constexpr int MaxW = 20, MaxH = 40;
	char ImageBuffer[GetRGBImageSize(MaxW, MaxH)];			// in C++ 17 even constexpr classes are supported, but not yet implemented in VS17
	cout << sizeof(ImageBuffer) << " MaxImgSize=" << GetRGBImageSize(MaxW, MaxH) << endl;
}

template <typename T>
constexpr T pi = T(3.1415);

struct IA
{
	virtual void HiA() = 0;
};

struct IB
{
	virtual void HiB() = 0;
};

struct A : IA, IB
{
	void HiA() noexcept override {}
	void HiB() noexcept override {}
};

struct SubObject
{
	void Hi()
	{
		cout << "Hi" << endl;
	}
};

typedef std::shared_ptr<SubObject> SPSubObject;

struct MainObject : enable_shared_from_this<MainObject>
{
	SPSubObject GetSubObject()
	{
		return SPSubObject(shared_from_this(), &m_Object);
	}
private:
	SubObject m_Object;
};

typedef std::shared_ptr<MainObject> SPMainObject;


struct DtorClass
{

	~DtorClass() noexcept(false)
	{
		throw std::runtime_error("Error in dtor");
	}
};

void DtorTest()
{
	try
	{
		DtorClass c;
	}
	catch (const std::exception& e)
	{
		cerr << e.what() << endl;
	}
}

constexpr int IntPow(int base, int exp)
{
	int Res = 1;
	for (int i = 0; i < exp; ++i)
		Res *= base;
	return Res;
}

std::string operator"" _bs(const char* pData, size_t Size)
{
	auto ChToNum = [](char c)
	{
		if (c >= 'a' && c <= 'f')
			return c - 'a';
		if (c >= 'A' && c <= 'F')
			return c - 'A';
		if (c >= '0' && c <= '9')
			return c - '0';
	};
	std::string res;
	for (gsl::index i = 0; i < Size; i += 2)
	{
		res += static_cast<char>(ChToNum(pData[i]) * 16 + ChToNum(pData[i + 1]));
	}
	return res;
}

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

inline MyMultiVar Return() { MyMultiVar m; return m; }

template <size_t Idx>
inline const int& get(const MyMultiVar &m)
{
	return m.x;
}

int main()
{
	//auto xw = "123456789ABCDEF123456789ABCDEF"_bx;

	DtorTest();

	SPMainObject spMain = make_shared<MainObject>();
	auto spSub = spMain->GetSubObject();
	spMain.reset();
	spSub->Hi();

	A a;
	IB * const pB = &a;
	IA * const pA = &a;
	cout << "pA=" << pA << " pB=" << pB << endl;
	cout << "pvA=" << dynamic_cast<void*>(pA) << " pvB=" << dynamic_cast<void*>(pB) << endl;  // you can test if 2 different interfaces point to the same object

	cout << pi<int> << endl;

	TestConstexpr();

	AlignedClass Data[2];
	cout << "AlignOf=" << alignof(AlignedClass) << " &Data[1] - &Data[0] = " << (char*)&Data[1] - (char*)&Data[0] << endl;
	cout << "Convert: " << Convert<int>("42") + 5 << " " << Convert<string>("abs") << endl;


	this_thread::sleep_for(10ms);

	FallThrough(2);

	NoDiscardTest();

	auto [num, text] = TupleTest();
	cout << num << " " << text << endl;

	CMutableLambda t;
	t.CaptureByValue();
	t.CaptureByRef();

	MyVec<int> v{ 1,2,3 };

	int BinNum = 0b0'1011'1001;
	cout << "Bin=" << BinNum << endl;

	cout << u8"Mít" << U"Mít" << endl;
	cout << "Raw string literals : a\\b" << " " << R"(a\b)" << endl;

	any x;
	x = spMain;
	x = t;
	// what is it good for ? 

	variant<int, string> vd;		// used in descent parser for token
	vd = 42;
	vd = "test";

	// visitor pattern + overload lambda

	cout << "VD which=" << vd.index() << endl;

	optional<int> oTest;

	namespace fi = std::filesystem;
	fi::path p = fi::current_path();
	for (fi::directory_iterator i{ p }; i != fi::directory_iterator{}; ++i)
	{
		cout << "F=" << i->path() << endl;
	}

	// parallel stl
	std::vector<int> Arr(IntPow(2, 15));

	// explicitly sequential sort
	std::sort(std::execution::seq, Arr.begin(), Arr.end());

	// permitting parallel execution
	std::sort(std::execution::par, Arr.begin(), Arr.end());

	// permitting vectorization as well
	std::sort(std::execution::par_unseq, Arr.begin(), Arr.end());

	cout << "3031"_bs << endl;

	map<string, string> Vals = { 
		{"a", "1"}, 
		{ "b", "2" },
	};

	if (auto[iter, IsInserted] = Vals.emplace("c", "3"); IsInserted)
	{
		cout << "c was inserted:" << IsInserted << endl;
	}
	else
	{
		cout << " c was not inserted : " << IsInserted << endl;
	}

	auto[ta, tb] = TupleTest();
	cout << "a=" << ta << " b=" << tb << endl;

	auto [ma, mb, mc] = Return();
	cout << "ma=" << ma << " mb=" << mb << " mc=" << mc << endl;

    return 0;
}

