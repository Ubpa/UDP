#include <UDP/Reflection/Reflection.h>

#include <string>
#include <iostream>

using namespace std;
using namespace Ubpa;

class C {
public:
	void Foo() {
		cout << n << " non-const Foo" << endl;
	}
	void Bar() const {
		cout << n << " const Bar" << endl;
	}
	void Car(float d) const {
		cout << d << " const Car" << endl;
	}
	int n{ 0 };
};

int main() {
	Reflection<C>::Instance()
		.Regist(&C::n, "n")
		.Regist(&C::Foo, "Foo")
		.Regist(&C::Bar, "Bar")
		.Regist(&C::Car, "Car");

	C c{ 3 };
	const C cc{ 4 };

	Reflection<C>::Instance().Call("Foo", c);
	Reflection<C>::Instance().Call("no", c);
	Reflection<C>::Instance().Call("Foo", &c);
	Reflection<C>::Instance().Call("Bar", cc);
	// Reflection<C>::Instance().Call("Car", c, 1); // error
	Reflection<C>::Instance().Call("Car", c, 1.f);
}
