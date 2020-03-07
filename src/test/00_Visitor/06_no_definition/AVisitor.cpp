#include "AVisitor.h"

#include "A.h"

#include <iostream>

using namespace Ubpa;
using namespace std;

AVisitor::AVisitor() {
	Regist<A, B, C>();
}

void AVisitor::ImplVisit(A*) {
	cout << "AVisitor::ImplVisit(A*)" << endl;
}

void AVisitor::ImplVisit(B*) {
	cout << "AVisitor::ImplVisit(B*)" << endl;
}

void AVisitor::ImplVisit(C*) {
	cout << "AVisitor::ImplVisit(C*)" << endl;
}