#include <UDP/Visitor/MultiVisitor.h>

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

class AD_Visitor : public RawPtrMultiVisitor<AD_Visitor, A, D> {
public:
	AD_Visitor() { Regist<A, /*B, C, */D, E, F>(); }

protected:
	void ImplVisit(A*) { cout << "Obj::ImplVisit(A*)" << endl; }
	//void ImplVisit(B*) { cout << "Obj::ImplVisit(B*)" << endl; }
	//void ImplVisit(C*) { cout << "Obj::ImplVisit(C*)" << endl; }
	void ImplVisit(D* d) { cout << "Obj::ImplVisit(D*), "; Visit(d->a); }
	void ImplVisit(E* e) { cout << "Obj::ImplVisit(E*), "; Visit(e->a); }
	void ImplVisit(F* f) { cout << "Obj::ImplVisit(F*), "; Visit(f->a); }
};

int main() {
	AD_Visitor v;
	v.Regist([](B*) { cout << "lambda(B*)" << endl; },
		[](C*) {cout << "lambda(C*)" << endl; });

	A a;
	B b;
	C c;
	D d(&a);
	E e(&b);
	F f(&c);

	A* ptrA[3] = { &a,&b,&c };
	D* ptrD[3] = { &d,&e,&f };

	v.Visit(ptrA[0]);
	v.Visit(ptrA[1]);
	v.Visit(ptrA[2]);
	v.Visit(ptrD[0]);
	v.Visit(ptrD[1]);
	v.Visit(ptrD[2]);

	v.Visit(reinterpret_cast<void*>(&e));
	v.Visit(reinterpret_cast<void*>(&b));
}
