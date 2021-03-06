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
#include <range/v3/algorithm/find.hpp>

#include "MyMultiVar.h"

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
		break;
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
	cout << "Do not discard return value warning" << endl;
	return true;
}


tuple<int, string> TupleTest()
{
	return { 42, "Harley Street" };
}


/// Inline variables - declspec selectany
//////////////////////////////////////////////////////////////////////////
inline int iv = 5;

template <typename T>
inline T Convert(const string& s)
{
	if constexpr (std::is_integral_v<T>)
		return std::atoi(s.c_str());
	else
		return s;
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
	[[gsl::suppress(f.6)]]
	~DtorClass() noexcept(false)			// breaking change - if the class throws in dtor - it should be marked noexcept(false)
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
		else
			throw std::runtime_error("Invalid char");
	};
	std::string res;
	for (gsl::index i = 0; i < static_cast<int>(Size); i += 2)
	{
		res += static_cast<char>(ChToNum(pData[i]) * 16 + ChToNum(pData[i + 1]));
	}
	return res;
}

template <typename ... Args>
inline ostream& TestFold(ostream& o, const Args& ... args)
{
	return (o << ... << args);
}

int main()
{
	//////////////////////////////////////////////////////////////////////////
	cout << " --- Dtor test - noexcept breaking change --- " << endl;
	DtorTest();

	//////////////////////////////////////////////////////////////////////////
	cout << " --- Shared_ptr special constructor --- " << endl;
	SPMainObject spMain = make_shared<MainObject>();
	auto spSub = spMain->GetSubObject();
	spMain.reset();
	spSub->Hi();


	//////////////////////////////////////////////////////////////////////////
	// you can test if 2 different interfaces point to the same object, or you can store the top-level object for reporting
	cout << " --- Dynamic cast to the pointer to the complete object --- " << endl;
	{
		A a;
		IB * const pB = &a;
		IA * const pA = &a;
		cout << "pA=" << pA << " pB=" << pB << endl;
		cout << "pvA=" << dynamic_cast<void*>(pA) << " pvB=" << dynamic_cast<void*>(pB) << endl;
	}

	//////////////////////////////////////////////////////////////////////////
	cout << " --- Template variables --- " << endl;	
	cout << pi<int> << endl;

	//////////////////////////////////////////////////////////////////////////
	cout << " --- Constexpr --- " << endl;
	{
		constexpr struct X
		{
			X() noexcept : X(0)  {}

			constexpr X(int Data) : m_Data(Data)
			{

			}

			constexpr int Get() const
			{
				return m_Data;
			}

		private:
			int m_Data;
		};

		constexpr X x(42);
		constexpr auto Sq = [](int x) {return x * x; };		// even a lambda can be constexpr
		vector<X> xs(10);		// X is fully available at runtime
		static_assert(Sq(x.Get()) >= 0, "Square must be >= 0");  // and also at compile time
		char Array[Sq(x.Get())];
		cout << "ImagSize=" << sizeof(Array) << " Xs=" << xs.size() << endl;
	}


	cout << " ---  Alignment  --- " << endl;
	//////////////////////////////////////////////////////////////////////////
	struct alignas(64) AlignedClass
	{
		char c;
		char d;
	};

	AlignedClass Data[2];
	cout << "AlignOf=" << alignof(AlignedClass) << " &Data[1] - &Data[0] = " << (char*)&Data[1] - (char*)&Data[0] << endl;

	cout << "--- Literals --- " << endl;
	//////////////////////////////////////////////////////////////////////////

	// Binary literals & digit separators
	int BinNum = 0b0'1000'0001;
	cout << "Bin=" << BinNum << endl;

	// hexadecimal floating point literals
	cout << "HexFloats = " << 0x3.Ap+4 << endl;

	// UTF8 literals, char32_t literals
	auto Result = U"Mít";
	cout << u8"Mít " << typeid(Result).name() << endl;
	// Raw string literals
	cout << "Raw string literals : a\\b" << " " << R"(a\b)" << endl;

	// User defined literals - Hexadecimal strings
	cout << "3031"_bs << endl;

	// std::chrono UDLs
	this_thread::sleep_for(10ms);

	cout << " --- Lambdas --- " << endl;
	//////////////////////////////////////////////////////////////////////////

	CMutableLambda t;
	t.CaptureByValue();
	t.CaptureByRef();

	
	cout << " --- Classes with value semantics  --- " << endl;
	//////////////////////////////////////////////////////////////////////////
	
	// std::variant class
	variant<int, string> vd;		// used in descent parser for token
	//vd = 42;
	vd = "Test";

	struct CA {};
	struct CB {};
	variant<CA, CB> vd2;
	vd2 = CA{};

	struct FVisitor
	{
		// shape intersection - double dispatch
		bool operator()(int i, CA)
		{
			cout << "CA:" << i << endl;
			return true;
		}

		bool operator()(int i, CB)
		{
			cout << "CB:" << i << endl;
			return false;
		}

		bool operator()(const string &s, CA)
		{
			cout << "CA:" << s << endl;
			return false;
		}

		bool operator()(const string& s, CB)
		{
			cout << "CB:" << s << endl;
			return false;
		}
	};

	auto res = std::visit(FVisitor{}, vd, vd2);
	cout << "Res=" << res << " VD1 Index=" << vd.index() << " VD2 Index=" << vd2.index() << endl;

	// std::optional represents that an element may  not be found in the collection
	optional<int> oTest1, oTest2(42);
	cout << "T1: " << oTest1.has_value() << " T2:" << *oTest2 << endl;

	// std::any
	any a;
	cout << "Type of default any:" << a.type().name() << endl;
	a = spMain;
	a = 5;
	int v = std::any_cast<int>(a);
	cout << "Int in any =" << v << endl;
	// what is it good for ? Storing "context" types e.g. for connection

	cout << " --- Filesystem  --- " << endl;
	//////////////////////////////////////////////////////////////////////////

	namespace fi = std::filesystem;
	fi::path pCur = fi::current_path();
	for (fi::directory_iterator i{ pCur }; i != fi::directory_iterator{}; ++i)
	{
		const auto &p = i->path();
		if (p.extension()==".h")
			cout << "Header=" << p << " Ext=" << p.extension() << endl;
	}

	cout << " --- parallel & vectorized Algorithms - OpenMP  --- " << endl;
	//////////////////////////////////////////////////////////////////////////
	
	std::array<int, IntPow(2, 10)> Arr;
	//int Arr[IntPow(2, 10)];		// Don't use this - no begin, end, ... just use std::array

	// explicitly sequential sort
	std::sort(std::execution::seq, begin(Arr), end(Arr));

	// permitting parallel execution
	std::sort(std::execution::par, Arr.begin(), Arr.end());

	// permitting vectorization as well - using GPU
	std::sort(std::execution::par_unseq, Arr.begin(), Arr.end());

	/// Works for most of the standard algorithms
	// partition, search, mismatch, is_sorted, nth_element, minmax_element, reduce, transform_reduce,
	// *_n, copy_*, ... 

	cout << " --- Containers --- " << endl;
	//////////////////////////////////////////////////////////////////////////
	
	map<string, string> Vals = { 
		{"a", "1"}, 
		{ "b", "2" },
	};

	// VSBUG: FALSE positive warnings:  https://developercommunity.visualstudio.com/content/problem/215975/c-code-analysis-c26444-false-positive.html
	[[gsl::suppress(es.84)]]
	Vals.insert_or_assign("b"s, "5"s);
	
	Vals.try_emplace("a", "3");

	// template guides - What is the type of vector and pair ? 
	std::vector v1{ 1,2,3,4 };
	std::pair p1{ 1, "Test", };
	cout << "Template guides: " << typeid(v1).name() << " " << typeid(p1).name() << endl;

	/// std::data, std::size, std::empty, std::begin, std::end, ... 
	{
		std::string s{ "--- Global functions data, size, and empty --- " };
		cout << std::data(s) << endl;
		cout << std::data(v1) << endl;
		cout << "Size= " << std::size(Vals) << " Empty=" << std::empty(Vals) << endl;
	}

	cout << " --- Multiple variables --- " << endl;
	//////////////////////////////////////////////////////////////////////////
	auto[ta, tb] = TupleTest();
	cout << "a=" << ta << " b=" << tb << endl;

	auto[ma, mb, mc] = Return();
	cout << "ma=" << ma << " mb=" << mb << " mc=" << mc << endl;

	cout << " --- Declaration in if and switch statements --- " << endl;
	//////////////////////////////////////////////////////////////////////////
	
	if (auto[iter, IsInserted] = Vals.emplace("c", "3"); IsInserted)
	{
		cout << "c was inserted:" << IsInserted << endl;
	}
	else
	{
		cout << " c was not inserted : " << IsInserted << endl;
	}

	struct SwitchTest
	{
		SwitchTest() = default;

		int Get()
		{
			return 42;
		}
	};

	switch (SwitchTest s; s.Get())
	{
	case 42: 
		cout << "OK " << endl;
		break;
	default:		
		cout << "???" << endl;
	}

	//////////////////////////////////////////////////////////////////////////
	cout << " --- Ranges --- " << endl;
	{
		// https://ericniebler.github.io/range-v3/
/*
		using namespace ranges;
		
		vector v{ 3,2,1,4 };
		action::sort(v);		// this modifies the container
		auto r = view::all(v);		// all views are lazy
		cout << r << endl;
		auto r2 = r | view::filter([](int i) {return i % 2; });
		cout << r2 << endl;
		auto r3 = r | view::intersperse(0);
		cout << r3 << endl;

		map<int, string> Data{
			{1, "one"},
			{2, "two"},
		};

		auto r4 = Data | view::keys;
		cout << r4 << endl;
		action::remove_if(v, [](int i) {return i < 2; });
		cout << "V=" << view::all(v) << endl;
		auto i = view::ints(4, 8);
		cout << "I=" << i << endl;
		action::push_back(v, i);
		cout << "VPushed = " << view::all(v) << endl;
*/


 	}

	//////////////////////////////////////////////////////////////////////////
	cout << " --- StringView --- " << endl;
	{
		auto l = [](std::string_view sv)	// non-owning reference to string
		{
			cout << sv << endl;
			cout << sv.substr(0, 10) << endl;
		};

		string sTest = "SVTest";
		const char* pC = sTest.c_str();
		l(sTest);
		l(pC);
		l("VeryLongString");

	}


	//////////////////////////////////////////////////////////////////////////
	{
		cout << " --- Constexpr if --- " << endl;
		int cv = Convert<int>("35");
		string Test = Convert<string>("Test");
		cout << cv << " " << Test << endl;
	}

	cout << "Inline variables " << iv << endl;

	{
		cout << " -- Fold expressions -- " << endl;
		TestFold(cout, 1, "Test", 3, "X");		
		cout << endl;
	}



	cout << "--- Miscellaneous ---" << endl;
	// std::byte - opaque byte type - with no arithmetic conversions
	vector<byte> Bytes(10);// = { 1,2,3,4 };
	memset(Bytes.data(), 1, 10);
	auto x = std::to_integer<int>(Bytes[0]);
	cout << "Byte x=" << x << endl;
	// shift and bitwise operators are defined
	//Bytes[0] = 5;  // cannot assign number to it

	FallThrough(2);

	NoDiscardTest();

	/// TMP: 
	// Coroutines
	// Template meta programming - variadic templates, parameter packs, UDLs

    return 0;
}

