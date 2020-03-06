#include <UDP/Visitor.h>

#include <iostream>

using namespace Ubpa;
using namespace std;

struct A { virtual ~A() = default; };
struct B : A {};
struct C : A {};

struct GetName : public Visitor<GetName, A> {
	GetName() {
		Regist<A>();
		Regist<B>();
	}

private:
	friend class VisitorType;
	void ImplVisit(A*) {
		cout << "GetName::ImplVisit(A*)" << endl;
	}
	void ImplVisit(B*) {
		cout << "GetName::ImplVisit(B*)" << endl;
	}
	string rst;
};

int main() {
	GetName v;
	v.Regist([](C*) {
		cout << "Lambda(C*)" << endl;
		});

	A a;
	B b;
	C c;
	A* ptrA[3] = { &a,&b,&c };

	v.Visit(ptrA[0]);
	v.Visit(ptrA[1]);
	v.Visit(ptrA[2]);

	return 0;
}
