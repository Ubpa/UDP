#include "Point.h"

#include <string>
#include <iostream>

using namespace std;
using namespace Ubpa;

int main() {
	Point p;
	AutoRefl::Register_Ubpa_Point();

	Reflection<Point>::Instance().Var<float>("x").of(p) = 3;
	Reflection<Point>::Instance().Var<float>("y").of(p) = 4;

	for (auto nv : Reflection<Point>::Instance().Vars()) {
		cout << nv.first << ": ";
		cout << nv.second->As<float>().of(p) << endl;
	}

	for (auto [name, var] : Reflection<Point>::Instance().Vars()) {
		cout << name << ": ";
		cout << var->As<float>().of(p) << endl;
	}

	for (auto [key, value] : Reflection<Point>::Instance().Metas()) {
		cout << key << ": ";
		cout << value << endl;
	}

	for (auto [field, t] : Reflection<Point>::Instance().FieldMetas()) {
		cout << field << endl;
		for (const auto& [key, value] : t)
			cout << "- " << key << " : " << value << endl;
	}
}
