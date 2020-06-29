#include <UDP/Reflection/Reflection.h>

#include <string>
#include <iostream>

using namespace std;
using namespace Ubpa;

struct [[size("8")]] Point {
	float test(float n) {
		cout << n << endl;
		return x + y + n;
	}
	[[not_serialize]]
	float x;
	[[info("hello")]]
	float y;
};

int main() {
	Reflection<Point>::Instance()
		.Register("size", "8")
		.Register(&Point::x, "x")
		.Register(&Point::y, "y")
		.Register("x", "not_serialize", "")
		.Register("y", "info", "hello")
		.Register(&Point::test, "test");

	Point p;
	Reflection<Point>::Instance().Var<float>("x").Of(p) = 3;
	Reflection<Point>::Instance().Var<float>("y").Of(p) = 4;
	for (auto nv : Reflection<Point>::Instance().Vars()) {
		cout << nv.first << ": ";
		cout << nv.second->As<float>().Of(p) << endl;
	}
	cout << Reflection<Point>::Instance().Call<float>("test", p, 1.f) << endl;

	for (auto [name, var] : Reflection<Point>::Instance().Vars()) {
		cout << name << ": ";
		cout << var->As<float>().Of(p) << endl;
	}

	for (auto [key, value] : Reflection<Point>::Instance().Metas()) {
		cout << key << ": ";
		cout << value << endl;
	}

	for (auto [field, t] : Reflection<Point>::Instance().FieldMetas()) {
		cout << field << endl;
		for (const auto& [key, value] : t)
			cout << "- " << key << " : " << value << endl;
	}

	for (auto nf : Reflection<Point>::Instance().Funcs())
		cout << Reflection<Point>::StaticName() << "::" << nf.first;
}
