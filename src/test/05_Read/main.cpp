#include <UDP/Basic/Read.h>

#include <UDP/Reflection/Reflection.h>

#include <iostream>

#include <array>

using namespace Ubpa;
using namespace std;

struct A {
	Read<A, float> f{ 2.f };
};

struct B {
	const float f{ 1.f };
};

struct C {
	float* pf{ nullptr };
};

struct D {
	Read<A, float*> pf;
};

struct Int {
	int value{ 2 };
};

struct Float {
	Read<Float, float> value{ 1.f };
	float operator+(const Int& i) const noexcept {
		return value + i.value;
	}
};

int main() {
	A a;
	
	Reflection<A>::Instance()
		.Regist(&A::f, "f");

	cout << Reflection<A>::Instance().Var<float>("f").Of(a) << endl;
	Reflection<A>::Instance().Var<float>("f").Of(a) = 3.f;
	cout << Reflection<A>::Instance().Var<float>("f").Of(a) << endl;

	Float f;
	f + Int{ 2 };
	return 0;
}
