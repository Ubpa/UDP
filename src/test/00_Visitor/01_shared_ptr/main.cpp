#include <UDP/Visitor/Visitor.h>

#include <iostream>
#include <memory>

using namespace Ubpa;
using namespace std;

struct A { virtual ~A() = default; };
struct B : A {};
struct C : A {};

struct CumtomVisitor : public SharedPtrVisitor<CumtomVisitor, A> {
	CumtomVisitor() {
		Regist<A, B>();
	}

protected:
	friend class VisitorType;
	void ImplVisit(shared_ptr<A>) {
		cout << "CumtomVisitor::ImplVisit(shared_ptr<A>)" << endl;
	}
	void ImplVisit(shared_ptr<B>) {
		cout << "CumtomVisitor::ImplVisit(shared_ptr<B>)" << endl;
	}
};

int main() {
	CumtomVisitor v;
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

	return 0;
}
