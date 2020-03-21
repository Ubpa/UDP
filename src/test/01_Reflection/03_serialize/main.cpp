#include <UDP/Reflection/Reflection.h>
#include <UDP/Reflection/MemVarVisitor.h>
#include <UDP/Visitor/MultiVisitor.h>

#include <string>
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

template<typename T>
ostream& operator<<(ostream& os, const std::shared_ptr<VarPtr<T>>& p) {
	os << p->get();
	return os;
}

ostream& operator<<(ostream& os, const std::shared_ptr<VarPtr<string>>& p) {
	os << "\"" << p->get() << "\"";
	return os;
}

template<typename T, size_t N>
ostream& operator<<(ostream& os, const std::shared_ptr<VarPtr<array<T, N>>>& p) {
	for (auto var : p->get())
		os << var << ", ";
	return os;
}

struct VarSerializer : SharedPtrMultiVisitor<VarSerializer, VarPtrBase> {
	VarSerializer() {
		this->Regist<
			VarPtr<float>,
			VarPtr<int>,
			VarPtr<string>,
			VarPtr<array<float, 3>>>();
	}

	template<typename T>
	void ImplVisit(std::shared_ptr<VarPtr<T>> p) { cout << p; }

	//virtual void ImplVisit(std::shared_ptr<VarPtr<string>> p) { cout << "\"" << p->get() << "\""; }
};

struct Serializer : RawPtrMultiVisitor<Serializer, Figure, Cosmetics> {
	Serializer() { Regist<Sphere, Square, Lipstick, Lipglaze>(); }

	template<typename T>
	void ImplVisit(T* e) {
		cout << "{" << endl;
		cout << "\"type\": \"" << Reflection<T>::Instance().GetName() << "\"" << endl;
		for (auto nv : Reflection<T>::Instance().Vars()) {
			cout << "\"" << nv.first << "\"" << ": ";
			vs.Visit(nv.second->PtrOf(e));
			cout << endl;
		}
		cout << "}" << endl;
	}

	VarSerializer vs;
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

	Serializer serializer;
	Sphere a;
	Square b;
	Figure* figures[2] = { &a,&b };
	Lipstick c;
	Lipglaze d;
	Cosmetics* cosmetics[2] = { &c,&d };

	for (auto f : figures)
		serializer.Visit(f);
	for (auto c : cosmetics)
		serializer.Visit(c);
}
