#pragma once

#include "Visitor.h"

namespace Ubpa {
	template<typename Impl, typename... Bases> class RawPtrMultiVisitor;
	template<typename Impl, typename... Bases> class SharedPtrMultiVisitor;
	template<typename... Bases> class BasicRawPtrMultiVisitor;
	template<typename... Bases> class BasicSharedPtrMultiVisitor;

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename... Bases>
	class MultiVisitor : private Visitor<Impl, AddPointer, PointerCaster, Bases>... {
	public:
		// regist : lambda function, callable object, raw function (pointer)
		template<typename... Funcs>
		inline void Regist(Funcs&&... func) noexcept;

		// dynamic double dispatch
		template<typename DerivedPtr>
		inline void Visit(DerivedPtr&& ptr_derived) const noexcept;

		// for Derived without default constructor
		template<typename DerivedPtr>
		static void RegistVFPtr(DerivedPtr&& ptrDerived) noexcept;

	protected:
		// regist menber function with
		// - name : ImplVisit
		// - argument : AddPointer<Deriveds>
		template<typename... Deriveds>
		inline void Regist() noexcept;

	private:
		template<typename Func>
		inline void RegistOne(Func&& func) noexcept;
		template<typename Derived>
		inline void RegistOne() noexcept;

		template<typename Base>
		using VisitorOf = Visitor<Impl, AddPointer, PointerCaster, Base>;
	};
}

#include "detail/MultiVisitor.inl"
