#include <UDP/Reflection/Reflection.h>

#include <string>
#include <iostream>

using namespace std;
using namespace Ubpa;

struct Point {
	float test(float n) {
		cout << n << endl;
		return x + y + n;
	}
	float x;
	float y;
};

int main() {
	Reflection<Point>::Instance()
		.SetName("Point")
		.Regist(&Point::x, "x")
		.Regist(&Point::y, "y")
		.Regist(&Point::test, "test");

	Point p;
	Reflection<Point>::Instance().Var<float>("x").Of(p) = 3;
	Reflection<Point>::Instance().Var<float>("y").Of(p) = 4;
	for (auto nv : Reflection<Point>::Instance().Vars()) {
		cout << nv.first << ": ";
		cout << nv.second.As<float>().Of(p) << endl;
	}
	cout << Reflection<Point>::Instance().Call<float>("test", p, 1.f) << endl;

	for (auto nv : Reflection<Point>::Instance().Vars()) {
		cout << nv.first << ": ";
		cout << nv.second.As<float>().Of(p) << endl;
	}

	for (auto nf : Reflection<Point>::Instance().Funcs())
		cout << Reflection<Point>::Instance().GetName() << "::" << nf.first;
}
