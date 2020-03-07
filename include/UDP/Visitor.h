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
	public:
		// dynamic double dispatch
		inline void Visit(BasePointer& ptrBase) const noexcept;
		inline void Visit(BasePointer&& ptrBase) const noexcept;

		// regist : lambda function, callable object, raw function (pointer)
		template<typename... Funcs>
		inline void Regist(Funcs&&... func) noexcept;

		// regist : callable object
		/*template<typename... Deriveds, typename FuncObj>
		inline void RegistOverload(FuncObj&& funcObj) noexcept;*/

	protected:
		using VisitorType = Visitor;

		// regist menber function with
		// - name     : ImplVisit
		// - argument : AddPointer<Deriveds>
		template<typename... Deriveds>
		inline void Regist() noexcept;

	private:
		template<typename Func>
		inline void RegistOne(Func&& func) noexcept;
		template<typename Derived>
		inline void RegistOne() noexcept;
		/*template<typename Derived, typename FuncObj>
		void RegistOverloadOne(FuncObj& funcObj) noexcept;
		template<typename Derived, typename FuncObj>
		void RegistOverloadOne(FuncObj&& funcObj) noexcept;*/

	private:
		detail::TypeMap<std::function<void(BasePointer)>> visitOps;
	};
}

#include "detail/Visitor.inl"
