#pragma once

namespace Ubpa {
	// [Func]
	// - Ret([const] void*, Args...)
	// - Ret(Impl::*)([const] void*, Args...)[const]
	// [ID] vtable, TypeID, customed ID
	template<typename Func>
	class Visitor;

	template<typename T>
	constexpr size_t Visitor_GetID() noexcept;
	template<typename T>
	constexpr size_t Visitor_GetID(const T* ptr) noexcept;
}

#include "detail/Visitor.inl"
