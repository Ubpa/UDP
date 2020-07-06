#pragma once

namespace Ubpa {
	template<typename T>
	constexpr size_t CustomID() noexcept;
	template<typename T>
	constexpr size_t CustomID(const T* ptr) noexcept;
}

#include "detail/CustomID.inl"
