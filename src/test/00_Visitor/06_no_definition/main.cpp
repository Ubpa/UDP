#include "AVisitor.h"
#include "A.h"

#include <iostream>

using namespace Ubpa;
using namespace std;

int main() {
	AVisitor v;

	A a;
	B b;
	C c;
	A* ptrA[3] = { &a,&b,&c };

	v.Visit(ptrA[0]);
	v.Visit(ptrA[1]);
	v.Visit(ptrA[2]);
}
