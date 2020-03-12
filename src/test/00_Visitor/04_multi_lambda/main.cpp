#include <UDP/Visitor/Visitor.h>

#include <iostream>
#include <memory>

using namespace Ubpa;
using namespace std;

struct A { virtual ~A() = default; };
struct B : A {};
struct C : A {};

int main() {
	BasicRawPtrVisitor<A> v;

	v.Regist([](A*) { cout << "overloaded::A*" << endl; },
		[](B*) { cout << "overloaded::B*" << endl; },
		[](C*) { cout << "overloaded::C*" << endl; });

	A a;
	B b;
	C c;
	A* ptrA[3] = { &a,&b,&c };

	v.Visit(ptrA[0]);
	v.Visit(ptrA[1]);
	v.Visit(ptrA[2]);
}
