#include <UDP/Reflection/Reflection.h>

#include <string>
#include <iostream>

using namespace std;
using namespace Ubpa;

struct [[info("hello Point")]] Point {
	virtual ~Point() = default;

	Point(float x = 1.f, float y = 1.f) : x{ x }, y{ y } {}
	
	[[info("hello x")]]
	float x;
	[[info("hello y")]]
	float y;
};

int main() {
	Reflection<Point>::Instance()
		.Register("info", "hello Point")
		.Register(&Point::x, "x")
		.Register("x", "info", "hello")
		.Register(&Point::y, "y")
		.Register("y", "info", "hello")
		.RegisterConstructor([](float x, float y) {
			return new Point(x, y);
		});

	void* ptr = ReflectionMngr::Instance().Create(NAMEOF_TYPE(Point), 1.f, 2.f);
	ReflectionBase* refl = ReflectionMngr::Instance().GetReflction(ptr);

	for (const auto& [n, v] : refl->VarPtrs(ptr))
		cout << n << endl;
	for (const auto& [k, v] : refl->Metas())
		cout << k << ": " << v << endl;
	for (const auto& [f, t] : refl->FieldMetas()) {
		cout << f << endl;
		for (const auto& [k, v] : t)
			cout << "- " << k << " : " << v << endl;
	}
}
