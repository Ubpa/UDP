#include <UDP/Reflection/Reflection.h>

#include <string>
#include <iostream>

using namespace std;
using namespace Ubpa;

struct Point {
	virtual ~Point() = default;
	
	float x;
	float y;
};

int main() {
	Reflection<Point>::Instance()
		.SetName("Point")
		.Regist(&Point::x, "x")
		.Regist(&Point::y, "y");

	auto ptr = ReflectionMngr::Instance().Creat("Point");
	auto refl = ReflectionMngr::Instance().GetReflction(ptr);

	for (auto [n, v] : refl->VarPtrs(ptr))
		cout << n << endl;
}
