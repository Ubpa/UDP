#include <UDP/Reflection/Reflection.h>
#include <UDP/Reflection/VarPtrVisitor.h>

#include <_deps/nameof.hpp>

#include <iostream>
#include <array>

using namespace std;
using namespace Ubpa;

struct Figure { virtual ~Figure() = default; };
struct Sphere : Figure { const int radius{ 1 }; const int* pi{ nullptr }; };
struct Square : Figure { float sideLength{ 2.f }; };

struct Cosmetics { virtual ~Cosmetics() = default; };
struct Lipstick : Cosmetics { string name{ "mac" }; };
struct Lipglaze : Cosmetics { std::array<float, 3> color{ 0.9f,0.1f,0.1f }; };

class VarSerializer : public VarPtrVisitor<void(VarSerializer::*)()> {
public:
	VarSerializer() {
		VarPtrVisitor<void(VarSerializer::*)()>::Register<
			float,
			const int,
			//const int*,
			string,
			array<float, 3>>();
	}

	using VarPtrVisitor<void(VarSerializer::*)()>::Register;

protected:
	template<typename T>
	void ImplVisit(const T& p) { cout << p; }

	template<typename T, size_t N>
	void ImplVisit(const array<T, N>& p) {
		for (auto var : p)
			cout << var << ", ";
	}

	void ImplVisit(const string& p) {
		cout << "\"" << p << "\"";
	}

public:
	void Visit(void* obj) {
		auto* refl = ReflectionMngr::Instance().GetReflction(obj);
		cout << "{" << endl;
		cout << "  \"type\": \"" << refl->Name() << "\"" << endl;
		for (auto [n, v] : refl->VarPtrs(obj)) {
			if (VarPtrVisitor<void(VarSerializer::*)()>::IsRegistered(v)) {
				cout << "  \"" << n << "\"" << ": ";
				VarPtrVisitor<void(VarSerializer::*)()>::Visit(v);
				cout << endl;
			}
		}
		cout << "}" << endl;
	}
};

int main() {
	Reflection<Sphere>::Instance()
		.Register(&Sphere::radius, NAMEOF(&Sphere::radius).c_str())
		.Register(&Sphere::pi, NAMEOF(&Sphere::pi).c_str());

	Reflection<Square>::Instance()
		.Register(&Square::sideLength, NAMEOF(&Square::sideLength).c_str());

	Reflection<Lipstick>::Instance()
		.Register(&Lipstick::name, NAMEOF(&Lipstick::name).c_str());

	Reflection<Lipglaze>::Instance()
		.Register(&Lipglaze::color, NAMEOF(&Lipglaze::color).c_str());

	VarSerializer vs;

	/*vs.Register([](const int& v) {
		cout << v;
	});*/

	Sphere a;
	a.pi = new int;
	Square b;
	Figure* figures[2] = { &a,&b };
	Lipstick c;
	Lipglaze d;
	Cosmetics* cosmetics[2] = { &c,&d };

	for (auto f : figures)
		vs.Visit(f);
	for (auto c : cosmetics)
		vs.Visit(c);

	delete a.pi;

	return 0;
}
