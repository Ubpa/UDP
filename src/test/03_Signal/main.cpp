#include <UDP/Signal/Signal.h>

#include <iostream>

using namespace Ubpa;
using namespace std;

int main() {
	Signal<float, int, int> s0;
	s0.Connect([](int, int n) {
		cout << n << endl;
		});
	s0.Emit(1.f, 2, 3);
}
