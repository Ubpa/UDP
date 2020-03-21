#pragma once

#include <UTemplate/Func.h>

#include <functional>
#include <memory>

namespace Ubpa::detail::Visitor_ {
	template<template<typename>class AddPointer>
	struct PointerCaster;
}

namespace Ubpa {
	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename... Bases>
	class MultiVisitor;

	template<typename Impl, typename Base> class RawPtrVisitor;
	template<typename Impl, typename Base> class SharedPtrVisitor;
	template<typename Base> class BasicRawPtrVisitor;
	template<typename Base> class BasicSharedPtrVisitor;

	// non-invasive visitor pattern
	// AddPointer: std::add_pointer_t, std::shared_ptr, ...
	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	class Visitor {
		// check it in Regist
		// static_assert(std::is_polymorphic_v<Base>);
		using BasePointer = AddPointer<Base>;
		using CBasePointer = AddPointer<const Base>;
	public:
		// dynamic double dispatch
		inline void Visit(void* ptr) const noexcept;
		inline void Visit(const void* ptr) const noexcept;
		inline void Visit(CBasePointer& ptrCBase) const noexcept;
		inline void Visit(CBasePointer&& ptrCBase) const noexcept;
		inline void Visit(BasePointer& ptrBase) const noexcept;
		inline void Visit(BasePointer&& ptrBase) const noexcept;

		// regist : lambda function, callable object, raw function (pointer)
		template<typename... Funcs>
		inline void Regist(Funcs&&... func) noexcept;

		// for Derived without default constructor
		template<typename DerivedPtr>
		static void RegistVFPtr(DerivedPtr&& ptrDerived) noexcept;

	protected:
		using VisitorType = Visitor;

		// regist menber function with
		// - name : ImplVisit
		// - argument : AddPointer<Deriveds>
		template<typename... Deriveds>
		inline void Regist() noexcept;
		template<typename... Deriveds>
		inline void RegistC() noexcept;

	private:
		template<typename Func>
		inline void RegistOne(Func&& func) noexcept;
		template<typename Derived>
		inline void RegistOne() noexcept;
		template<typename Derived>
		inline void RegistOneC() noexcept;
		template<typename Derived>
		inline void RegistOne(Impl* impl) noexcept; // for MultiVisitor
		template<typename Derived>
		inline void RegistOneC(Impl* impl) noexcept; // for MultiVisitor

		template<typename Base, typename Derived>
		static size_t offset() noexcept {
			return reinterpret_cast<size_t>(static_cast<Base*>(reinterpret_cast<Derived*>(1))) - 1;
		}

	private:
		// vtable to callbacks
		std::unordered_map<const void*, std::function<void(BasePointer)>> callbacks;
		std::unordered_map<const void*, std::function<void(CBasePointer)>> const_callbacks;
		std::unordered_map<const void*, size_t> offsets;

	private:
		struct Accessor;

		template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename... Bases>
		friend class MultiVisitor;
	};
}

#include "detail/Visitor.inl"
