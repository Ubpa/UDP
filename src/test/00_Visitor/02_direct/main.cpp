#include <UDP/Visitor/Visitor.h>

#include <iostream>
#include <memory>

using namespace Ubpa;
using namespace std;

struct A { virtual ~A() = default; };
struct B : A {};
struct C : A {};

int main() {
	{// test RawPtrVisitor
		BasicRawPtrVisitor<A> v;
		v.Regist([](A*) {
			cout << "Lambda(A*)" << endl;
			});
		v.Regist([](B*) {
			cout << "Lambda(B*)" << endl;
			});
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
	}

	{// test SharedPtrVisitor
		BasicSharedPtrVisitor<A> v;
		v.Regist([](shared_ptr<A>) {
			cout << "Lambda(shared_ptr<A>)" << endl;
			});
		v.Regist([](shared_ptr<B>) {
			cout << "Lambda(shared_ptr<B>)" << endl;
			});
		v.Regist([](shared_ptr<C>) {
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
