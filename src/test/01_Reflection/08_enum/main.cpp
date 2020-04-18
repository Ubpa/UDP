#include <UDP/Basic/Read.h>

#include <UDP/Reflection/Reflection.h>

#include <iostream>

using namespace Ubpa;
using namespace std;

enum class E {
	eA, eB, eC, eD
};

struct A {
	E e;
};

int main() {
	
	Reflection<A>::Instance()
		.Regist(&A::e, "e");

	A a;
	a.e = E::eA;
	Reflection<A>::Instance().Var<int>("e").Of(&a) = static_cast<int>(E::eB);
	cout << (a.e == E::eB ? "eB" : "not eB") << endl;
	return 0;
}
