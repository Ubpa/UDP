#include <UDP/Reflection/Reflection.h>

#include <string>
#include <iostream>

using namespace std;
using namespace Ubpa;

class C {
public:
	void Foo() {
		cout << cn << ", " << n << " non-const Foo" << endl;
	}
	void Bar() const {
		cout << cn << ", " << n << " const Bar" << endl;
	}
	void Car(float d) const {
		cout << cn << ", " << d << " const Car" << endl;
	}
	int n{ 0 };
	const int* cn{ nullptr };
};

int main() {
	Reflection<C>::Instance()
		.Regist(&C::n, "n")
		.Regist(&C::cn, "cn")
		.Regist(&C::Foo, "Foo")
		.Regist(&C::Bar, "Bar")
		.Regist(&C::Car, "Car");

	C c{ 1 };
	const C cc{ 3 };

	Reflection<C>::Instance().Call("Foo", c);
	Reflection<C>::Instance().Call("no", c);
	Reflection<C>::Instance().Call("Foo", &c);
	Reflection<C>::Instance().Call("Bar", cc);
	// Reflection<C>::Instance().Call("Car", c, 1); // error
	Reflection<C>::Instance().Call("Car", c, 1.f);
}
