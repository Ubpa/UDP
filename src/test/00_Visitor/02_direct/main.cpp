#include <UDP/Visitor.h>

#include <iostream>
#include <memory>

using namespace Ubpa;
using namespace std;

struct A {
	virtual ~A() {}
};

struct B : A {};
struct C : A {};

int main() {
	{
		RawPtrVisitor<A> v;
		v.Reg([](A*) {
			cout << "Lambda(A*)" << endl;
			});
		v.Reg([](B*) {
			cout << "Lambda(B*)" << endl;
			});
		v.Reg([](C*) {
			cout << "Lambda(C*)" << endl;
			});
		A a;
		B b;
		C c;
		A* ptrA[3] = { &a,&b,&c };

		v.Visit(ptrA[0]);
		v.Visit(ptrA[1]);
		v.Visit(ptrA[2]);
	}

	{
		SharedPtrVisitor<A> v;
		v.Reg([](shared_ptr<C>) {
			cout << "Lambda(shared_ptr<C>)" << endl;
			});

		auto a = make_shared<A>();
		auto b = make_shared<B>();
		auto c = make_shared<C>();
		shared_ptr<A> ptrA[3] = { a,b,c };

		v.Visit(ptrA[0]);
		v.Visit(ptrA[1]);
		v.Visit(ptrA[2]);
	}

	return 0;
}
