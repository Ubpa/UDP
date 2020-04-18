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
		.Regist(&Point::x, NAMEOF(Point::x).c_str())
		.Regist(&Point::y, NAMEOF(Point::y).c_str())
		.Regist(string(NAMEOF(Point::x).c_str()) + "::is_reflect", "false")
		.Regist(string(NAMEOF(Point::x).c_str()) + "::info", "haha")
		.Regist(&Point::test, NAMEOF(&Point::test).c_str());

	Point p;
	Reflection<Point>::Instance().Var<float>(NAMEOF(Point::x).c_str()).Of(p) = 3;
	Reflection<Point>::Instance().Var<float>(NAMEOF(Point::y).c_str()).Of(p) = 4;
	for (auto nv : Reflection<Point>::Instance().Vars()) {
		cout << nv.first << ": ";
		cout << nv.second->As<float>().Of(p) << endl;
	}
	cout << Reflection<Point>::Instance().Call<float>(NAMEOF(&Point::test).c_str(), p, 1.f) << endl;

	for (auto [name, var] : Reflection<Point>::Instance().Vars()) {
		cout << name << ": ";
		cout << var->As<float>().Of(p) << endl;
	}

	for (auto [key, value] : Reflection<Point>::Instance().Metas()) {
		cout << key << ": ";
		cout << value << endl;
	}

	for (auto nf : Reflection<Point>::Instance().Funcs())
		cout << Reflection<Point>::Instance().Name() << "::" << nf.first;
}
