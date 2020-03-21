#include <UDP/Basic/Dirty.h>

#include <iostream>

using namespace Ubpa;
using namespace std;

int main() {
	Dirty<float> height([](float& h) {
		h = 3.f;
		});
	auto h0 = height.Get();
	auto h1 = height.Get();
	cout << h1 << endl;
}
