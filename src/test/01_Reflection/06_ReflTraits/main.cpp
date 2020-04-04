#include <UDP/Reflection/Reflection.h>
#include <UDP/Reflection/VarPtrVisitor.h>

#include <iostream>
#include <array>

using namespace std;
using namespace Ubpa;

struct Figure { virtual ~Figure() = default; };
struct Sphere : Figure { int radius{ 1 }; };
struct Square : Figure { float sideLength{ 2.f }; };

struct Cosmetics { virtual ~Cosmetics() = default; };
struct Lipstick : Cosmetics { string name{ "mac" }; };
struct Lipglaze : Cosmetics { std::array<float, 3> color{ 0.9f,0.1f,0.1f }; };

class VarSerializer : public VarPtrVisitor<VarSerializer>, public ReflTraitsVisitor {
public:
	VarSerializer() {
		VarPtrVisitor<VarSerializer>::RegistC<
			float,
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
	virtual void Receive(const void* obj, const std::string& name, const std::map<std::string, std::shared_ptr<const VarPtrBase>>& vars) override {
		cout << "{" << endl;
		cout << "  \"type\": \"" << name << "\"" << endl;
		for (auto [n, v] : vars) {
			cout << "  \"" << n << "\"" << ": ";
			VarPtrVisitor<VarSerializer>::Visit(v);
			cout << endl;
		}
		cout << "}" << endl;
	}
};

int main() {
	Reflection<Sphere>::Instance()
		.SetName("Sphere")
		.Regist(&Sphere::radius, "radius");

	Reflection<Square>::Instance()
		.SetName("Square")
		.Regist(&Square::sideLength, "sideLength");

	Reflection<Lipstick>::Instance()
		.SetName("Lipstick")
		.Regist(&Lipstick::name, "name");

	Reflection<Lipglaze>::Instance()
		.SetName("Lipglaze")
		.Regist(&Lipglaze::color, "color");

	VarSerializer vs;
	ReflTraitsIniter::Instance().Init(vs);

	vs.Regist([](const int& v) {
		cout << v;
		});

	Sphere a;
	Square b;
	Figure* figures[2] = { &a,&b };
	Lipstick c;
	Lipglaze d;
	Cosmetics* cosmetics[2] = { &c,&d };

	for (auto f : figures)
		vs.Visit(f);
	for (auto c : cosmetics)
		vs.Visit(c);
}
