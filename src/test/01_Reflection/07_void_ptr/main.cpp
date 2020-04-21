#include <UDP/Reflection/Reflection.h>

#include <string>
#include <iostream>

using namespace std;
using namespace Ubpa;

struct Point {
	virtual ~Point() = default;

	Point(float x = 1.f, float y = 1.f) : x{ x }, y{ y } {}
	
	float x;
	float y;
};

int main() {
	Reflection<Point>::Instance()
		.Regist(&Point::x, "x")
		.Regist("x::info", "hello")
		.Regist(&Point::y, "y")
		.Regist("y::info", "hello")
		.RegistConstructor([](float x, float y) {
			return new Point(x, y);
		});

	auto ptr = ReflectionMngr::Instance().Create(NAMEOF_TYPE(Point), 1.f, 2.f);
	auto refl = ReflectionMngr::Instance().GetReflction(ptr);

	for (auto [n, v] : refl->VarPtrs(ptr))
		cout << n << endl;
	for (auto [k, v] : refl->Metas())
		cout << k << ": " << v;
}
