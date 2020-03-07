#include <UDP/Visitor.h>

#include <iostream>
#include <memory>

using namespace Ubpa;
using namespace std;

struct A { virtual ~A() = default; };
struct B : A {};
struct C : A {};

struct D {
	virtual ~D() = default;
	A* a;
};
struct E : D {};
struct F : D {};

struct AD_Visitor : 
	Visitor<AD_Visitor, A>,
	Visitor<AD_Visitor, D>
{
	AD_Visitor() {
		Visitor<AD_Visitor, D>::Regist<D, E, F>();
		Visitor<AD_Visitor, A>::Regist<A, B, C>();
	}

	using Visitor<AD_Visitor, A>::Visit;
	using Visitor<AD_Visitor, D>::Visit;

private:
	friend class Visitor<AD_Visitor, D>;
	friend class Visitor<AD_Visitor, A>;

	void ImplVisit(D* d) {
		cout << "Obj::ImplVisit(D*)" << endl;
		cout << "  - ";
		Visit(d->a);
	}

	void ImplVisit(E* e) {
		cout << "Obj::ImplVisit(E*)" << endl;
		cout << "  - ";
		Visit(e->a);
	}

	void ImplVisit(F* f) {
		cout << "Obj::ImplVisit(F*)" << endl;
		cout << "  - ";
		Visit(f->a);
	}

	void ImplVisit(A*) {
		cout << "Obj::ImplVisit(A*)" << endl;
	}

	void ImplVisit(B*) {
		cout << "Obj::ImplVisit(B*)" << endl;
	}

	void ImplVisit(C*) {
		cout << "Obj::ImplVisit(C*)" << endl;
	}
};

int main() {
	AD_Visitor v;

	A a;
	B b;
	C c;
	D d;
	E e;
	F f;

	d.a = &a;
	e.a = &b;
	f.a = &c;

	A* ptrA[3] = { &a,&b,&c };
	D* ptrD[3] = { &d,&e,&f };

	v.Visit(ptrA[0]);
	v.Visit(ptrA[1]);
	v.Visit(ptrA[2]);
	v.Visit(ptrD[0]);
	v.Visit(ptrD[1]);
	v.Visit(ptrD[2]);
	v.Visit(&e);
}
