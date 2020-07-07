#pragma once

namespace Ubpa {
	struct [[size("8")]] Point {
		[[not_serialize]]
		float x;
		[[info("hello")]]
		float y;
	};
}

#include "_deps/AutoRefl_Point.inl"
