#include <UDP/Visitor.h>

#include <iostream>
#include <memory>

using namespace Ubpa;
using namespace std;

struct IShape { virtual ~IShape() = default; };

struct Triangle : IShape {
	size_t indices[3]{ 1,2,3 };
};

struct Square : IShape {
	float size_len{ 1.f };
};

struct Sphere : IShape {
	float radius{ 1.f };
};

struct Serializer : Visitor<bool(Serializer::*)(float)> {
	Serializer() {
		Regist<Sphere>();
	}

	bool ImplVisit(const Sphere* sphere, float k) const {
		cout << k * sphere->radius << endl;
		return true;
	}
};

int main() {
	Square square;
	Triangle tri;
	Sphere sphere;
	Visitor<bool()> visitor;
	visitor.Regist([](Square* square) -> bool {
		cout << square->size_len << endl;
		return true;
		}, [](Triangle* tri) -> bool {
			cout << tri->indices[0] << ", "
				<< tri->indices[1] << ", "
				<< tri->indices[2] << endl;
			return false;
		});

	visitor.Visit(&square);
	visitor.Visit(&tri);
	// visitor.Visit(&sphere); // assert

	visitor.Visit(reinterpret_cast<void*>(&square));

	const Serializer serializer;
	serializer.Visit(&sphere, 2.f);
}
