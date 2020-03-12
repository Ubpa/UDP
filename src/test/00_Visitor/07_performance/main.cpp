#include <UDP/Visitor/MultiVisitor.h>

#include <iostream>

#include <chrono>

using namespace Ubpa;
using namespace std;

struct IFigure;
struct Sphere;
struct Ellipse;
struct Square;
struct Freehand;
struct Polygon;
struct Triangle;

struct IFigureVisitor {
	virtual void visit(Sphere*) = 0;
	virtual void visit(Ellipse*) = 0;
	virtual void visit(Square*) = 0;
	virtual void visit(Freehand*) = 0;
	virtual void visit(Polygon*) = 0;
	virtual void visit(Triangle*) = 0;
};

struct IFigure {
	virtual ~IFigure() = default;
	virtual void accept(IFigureVisitor*) = 0;
};

struct Sphere : IFigure {
	virtual void accept(IFigureVisitor* v) {
		return v->visit(this);
	}
};

struct Ellipse : IFigure {
	virtual void accept(IFigureVisitor* v) {
		return v->visit(this);
	}
};

struct Square : IFigure {
	virtual void accept(IFigureVisitor* v) {
		return v->visit(this);
	}
};

struct Freehand : IFigure {
	virtual void accept(IFigureVisitor* v) {
		return v->visit(this);
	}
};

struct Polygon : IFigure {
	virtual void accept(IFigureVisitor* v) {
		return v->visit(this);
	}
};

struct Triangle : IFigure {
	virtual void accept(IFigureVisitor* v) {
		return v->visit(this);
	}
};

struct Drawer : IFigureVisitor {
	inline void core() {
		for (size_t j = 0; j < 64; j++)
			i *= i;
	}
	virtual void visit(Sphere*) override { core(); }
	virtual void visit(Ellipse*) override { core(); }
	virtual void visit(Square*) override { core(); }
	virtual void visit(Freehand*) override { core(); }
	virtual void visit(Polygon*) override { core(); }
	virtual void visit(Triangle*) override { core(); }
	float i{0.9f};
};

struct VFigureVisitor : RawPtrVisitor<VFigureVisitor, IFigure> {
	VFigureVisitor() {
		Regist<Sphere, Ellipse, Square, Freehand, Polygon, Triangle>();
	}

	virtual void ImplVisit(Sphere*) = 0;
	virtual void ImplVisit(Ellipse*) = 0;
	virtual void ImplVisit(Square*) = 0;
	virtual void ImplVisit(Freehand*) = 0;
	virtual void ImplVisit(Polygon*) = 0;
	virtual void ImplVisit(Triangle*) = 0;
};

struct VDrawer : VFigureVisitor {
	inline void core() {
		for (size_t j = 0; j < 64; j++)
			i *= i;
	}
	virtual void ImplVisit(Sphere*) override { core(); }
	virtual void ImplVisit(Ellipse*) override { core(); }
	virtual void ImplVisit(Square*) override { core(); }
	virtual void ImplVisit(Freehand*) override { core(); }
	virtual void ImplVisit(Polygon*) override { core(); }
	virtual void ImplVisit(Triangle*) override { core(); }

	float i{ 0.9f };
};

int main() {
	Sphere a;
	Ellipse b;
	Square c;
	Freehand d;
	Polygon e;
	Sphere aa;
	Triangle f;

	IFigure* figures[6] = { &a,&b,&c,&d,&e,&f };
	Drawer drawer;
	VDrawer vdrawer;
	auto pvdrawer = &vdrawer;

	size_t N = 1 << 12;
	double sumT[3] = { 0. };
	for (size_t k = 0; k < 1 << 10; k++) {
		auto t0 = chrono::steady_clock::now();
		for (size_t i = 0; i < N; i++) {
			drawer.visit(&a);
			drawer.visit(&b);
			drawer.visit(&c);
			drawer.visit(&d);
			drawer.visit(&e);
			drawer.visit(&f);
		}
		auto t1 = chrono::steady_clock::now();
		for (size_t i = 0; i < N; i++) {
			for (auto figure : figures)
				figure->accept(&drawer);
		}
		auto t2 = chrono::steady_clock::now();
		for (size_t i = 0; i < N; i++) {
			for (auto figure : figures)
				pvdrawer->Visit(figure);
		}
		auto t3 = chrono::steady_clock::now();
		chrono::duration<double> t01 = chrono::duration_cast<chrono::duration<double>>(t1 - t0);
		chrono::duration<double> t12 = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
		chrono::duration<double> t23 = chrono::duration_cast<chrono::duration<double>>(t3 - t2);
		sumT[0] += t01.count();
		sumT[1] += t12.count();
		sumT[2] += t23.count();
	}

	for (auto t : sumT)
		cout << t << endl;
}
