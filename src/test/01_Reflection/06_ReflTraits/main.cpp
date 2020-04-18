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

class VarSerializer : public VarPtrVisitor<VarSerializer>, public ReflTraitsVisitor {
public:
	VarSerializer() {
		VarPtrVisitor<VarSerializer>::RegistC<
			float,
			const int,
			//const int*,
			string,
			array<float, 3>>();
	}

	using VarPtrVisitor<VarSerializer>::Regist;
	using ReflTraitsVisitor::Visit;

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

private:
	virtual void Receive(const void* obj, std::string_view name, const xMap<std::string, std::shared_ptr<const VarPtrBase>>& vars) override {
		cout << "{" << endl;
		cout << "  \"type\": \"" << name << "\"" << endl;
		for (auto [n, v] : vars) {
			if (VarPtrVisitor<VarSerializer>::IsRegisted(v)) {
				cout << "  \"" << n << "\"" << ": ";
				VarPtrVisitor<VarSerializer>::Visit(v);
				cout << endl;
			}
		}
		cout << "}" << endl;
	}
};

int main() {
	Reflection<Sphere>::Instance()
		.Regist(&Sphere::radius, NAMEOF(&Sphere::radius).c_str())
		.Regist(&Sphere::pi, NAMEOF(&Sphere::pi).c_str());

	Reflection<Square>::Instance()
		.Regist(&Square::sideLength, NAMEOF(&Square::sideLength).c_str());

	Reflection<Lipstick>::Instance()
		.Regist(&Lipstick::name, NAMEOF(&Lipstick::name).c_str());

	Reflection<Lipglaze>::Instance()
		.Regist(&Lipglaze::color, NAMEOF(&Lipglaze::color).c_str());

	VarSerializer vs;
	ReflTraitsIniter::Instance().InitC(vs);

	vs.Regist([](const int& v) {
		cout << v;
	});

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
