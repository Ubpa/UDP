#include <UDP/Visitor/InfVisitor.h>

#include <iostream>
#include <memory>

using namespace Ubpa;
using namespace std;

struct A { virtual ~A() = default; };
struct B : A {};
struct C : A {};

struct D {
	D(A* a = nullptr) : a(a) {}
	virtual ~D() = default;
	A* a;
};
struct E : D { using D::D; };
struct F : D { using D::D; };

class AD_Visitor : public InfVisitor<AD_Visitor> {
public:
	AD_Visitor() {
		RegistC<A, B, D, E>();
		Regist<C, F>();
	}

protected:
	void ImplVisit(const A*) { cout << "Obj::ImplVisit(const A*)" << endl; }
	void ImplVisit(const B*) { cout << "Obj::ImplVisit(const B*)" << endl; }
	void ImplVisit(C*) { cout << "Obj::ImplVisit(C*)" << endl; }
	void ImplVisit(const D* d) { cout << "Obj::ImplVisit(const D*), "; Visit(d->a); }
	void ImplVisit(const E* e) { cout << "Obj::ImplVisit(const E*), "; Visit(e->a); }
	void ImplVisit(F* f) { cout << "Obj::ImplVisit(F*), "; Visit(f->a); }
};

int main() {
	AD_Visitor v;
	v.Regist([](B*) { cout << "lambda(B*)" << endl; },
		[](const C*) {cout << "lambda(const C*)" << endl; },
		[](E* e) { cout << "lambda(E*)" << endl; },
		[](const F* f) { cout << "lambda(const F*)" << endl; }
	);

	A a;
	B b;
	C c;
	const A ca;
	const B cb;
	const C cc;
	D d(&a);
	E e(&b);
	F f(&c);
	const D cd(&a);
	const E ce(&b);
	const F cf(&c);

	v.Visit(&a);
	v.Visit(&b);
	v.Visit(&c);
	v.Visit(&ca);
	v.Visit(&cb);
	v.Visit(&cc);
	v.Visit(&d);
	v.Visit(&e);
	v.Visit(&f);
	v.Visit(&cd);
	v.Visit(&ce);
	v.Visit(&cf);

	v.Visit(reinterpret_cast<const void*>(&cc));
	v.Visit(reinterpret_cast<void*>(&f));
}
