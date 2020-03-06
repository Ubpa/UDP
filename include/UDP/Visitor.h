#pragma once

#include "detail/TypeMap.h"

#include <UTemplate/FuncTraits.h>

#include <functional>
#include <memory>

namespace Ubpa::detail::Visitor_ {
	template<template<typename>class AddPointer>
	struct PointerCaster;

	template<>
	struct PointerCaster<std::add_pointer_t> {
		template<typename To, typename From>
		static std::add_pointer_t<To> run(std::add_pointer_t<From> from) {
			return static_cast<std::add_pointer_t<To>>(from);
		}
	};

	template<>
	struct PointerCaster<std::shared_ptr> {
		template<typename To, typename From>
		static std::shared_ptr<To> run(const std::shared_ptr<From>& from) {
			return std::static_pointer_cast<To>(from);
		}
		template<typename To, typename From>
		static std::shared_ptr<To> run(std::shared_ptr<From>&& from) {
			return std::static_pointer_cast<To>(std::move(from));
		}
	};
}

namespace Ubpa {
	// 访问者模式是一种将数据操作和数据结构分离的设计模式
	template<typename Impl, typename Base, template<typename>class AddPointer = std::add_pointer_t,
		typename PointerCaster = detail::Visitor_::PointerCaster<AddPointer>>
	class Visitor {
		using BasePointer = AddPointer<Base>;
		static_assert(std::is_polymorphic_v<Base>);
	protected:
		using VisitorType = Visitor;
	public:
		// 动态双分派
		inline void Visit(BasePointer& ptr_base) const noexcept {
			// 不是用 typeid(T)，因为可能是多态类
			auto target = visitOps.find(typeid(*ptr_base));
			if (target != visitOps.end())
				target->second(ptr_base);
		}
		inline void Visit(BasePointer&& ptr_base) const noexcept {
			auto target = visitOps.find(typeid(*ptr_base));
			if (target != visitOps.end())
				target->second(std::move(ptr_base));
		}

		// 可调用对象，函数指针，lambda 函数
		template<typename Func>
		void Reg(Func&& func) {
			using DerivedPointer = Front_t<typename FuncTraits<Func>::ArgList>;
			using Derived = std::decay_t<decltype(*DerivedPointer{ nullptr })>;
			static_assert(std::is_same_v<DerivedPointer, AddPointer<Derived>>);
			static_assert(std::is_base_of_v<Base, Derived>);

			visitOps[typeid(Derived)] = [func = std::forward<Func>(func)](BasePointer ptr_base) {
				func(PointerCaster::template run<Derived, Base>(ptr_base));
			};
		}

	protected:
		template<typename Derived>
		void Reg() {
			using DerivedPointer = AddPointer<Derived>;
			//using Func = void(Impl::*)(DerivedPointer);
			//constexpr Func f = &Impl::_Visit;
			//using Derived = std::decay_t<decltype(*(DerivedPointer{ nullptr })) > ;
			// auto impl = static_cast<Impl*>(this)
			visitOps[typeid(Derived)] = [impl = static_cast<Impl*>(this)](BasePointer ptr_base) {
				using Func = void(Impl::*)(DerivedPointer);
				constexpr Func f = &Impl::ImplVisit;
				// constexpr void(Impl:: * f)(DerivedPointer) = &Impl::ImplVisit;
				(impl->*f)(PointerCaster::template run<Derived, Base>(ptr_base));
			};
		}

	private:
		detail::TypeMap<std::function<void(BasePointer)>> visitOps;
	};
}

#include "detail/Visitor.inl"
