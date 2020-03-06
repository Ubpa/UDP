#pragma once

#include "detail/TypeMap.h"

#include <UTemplate/FuncTraits.h>

#include <functional>
#include <memory>

namespace Ubpa::detail::Visitor_ {
	template<template<typename>class AddPointer>
	struct PointerCaster;
}

namespace Ubpa {
	template<typename Base> class SharedPtrVisitor; // final
	template<typename Base> class RawPtrVisitor;    // final

	// non-invasive visitor pattern
	// AddPointer: std::add_pointer_t, std::shared_ptr, ...
	template<typename Impl, typename Base,
		template<typename>class AddPointer = std::add_pointer_t,
		typename PointerCaster = detail::Visitor_::PointerCaster<AddPointer>>
	class Visitor {
		static_assert(std::is_polymorphic_v<Base>);
		using BasePointer = AddPointer<Base>;
	protected:
	public:
		// dynamic double dispatch
		inline void Visit(BasePointer& ptr_base) const noexcept;
		inline void Visit(BasePointer&& ptr_base) const noexcept;

		// regist: callable object, raw function (pointer), lambda function
		template<typename Func>
		inline void Regist(Func&& func) noexcept;

	protected:
		using VisitorType = Visitor;

		// regist menber function with
		// - name: ImplVisit
		// - argument: AddPointer<Derived>
		template<typename Derived>
		inline void Regist() noexcept;

	private:
		detail::TypeMap<std::function<void(BasePointer)>> visitOps;
	};
}

#include "detail/Visitor.inl"
