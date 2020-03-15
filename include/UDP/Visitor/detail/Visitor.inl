#pragma once

#include "../../vtable.h"

#include <UTemplate/Typelist.h>
#include <cassert>

#ifndef NDEBUG
#include <iostream>
#endif

namespace Ubpa::detail::Visitor_ {
	template<typename Pointer>
	using RemovePtr = std::decay_t<decltype(*std::decay_t<Pointer>{ nullptr })>;

	struct VoidImpl final {};
}

namespace Ubpa {
	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	template<typename DerivedPtr>
	void Visitor<Impl, AddPointer, PointerCaster, Base>::RegistVFPtr(DerivedPtr&& ptrDerived) noexcept {
		using Derived = detail::Visitor_::RemovePtr<DerivedPtr>;
		static_assert(std::is_base_of_v<Base, Derived>);
		assert(vtable_of<Derived>::get() == nullptr);
		vtable_of<Derived>::regist(ptrDerived);
	}

	// ref: https://stackoverflow.com/questions/8523762/crtp-with-protected-derived-member
	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	struct Visitor<Impl, AddPointer, PointerCaster, Base>::Accessor : public Impl {
		template<typename Derived>
		inline static void ImplVisitOf(Impl* const impl, AddPointer<Base> ptrBase) noexcept {
			constexpr void(Impl:: * f)(AddPointer<Derived>) = &Impl::ImplVisit;
			(impl->*f)(PointerCaster::template run<Derived, Base>(ptrBase));
		}
	};

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	void Visitor<Impl, AddPointer, PointerCaster, Base>::Visit(BasePointer& ptrBase) const noexcept {
		auto target = callbacks.find(vtable(ptrBase));
		if (target != callbacks.end())
			target->second(ptrBase);
#ifndef NDEBUG
		else {
			std::cout << "WARNING::" << typeid(Impl).name() << "::Visit:" << std::endl
				<< "\t" << "hasn't regist " << typeid(*ptrBase).name() << std::endl;
		}
#endif // !NDEBUG
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	void Visitor<Impl, AddPointer, PointerCaster, Base>::Visit(BasePointer&& ptrBase) const noexcept {
		auto target = callbacks.find(vtable(ptrBase));
		if (target != callbacks.end())
			target->second(std::forward<BasePointer>(ptrBase));
#ifndef NDEBUG
		else {
			std::cout << "WARNING::" << typeid(Impl).name() << "::Visit:" << std::endl
				<< "\t" << "hasn't regist " << typeid(*ptrBase).name() << std::endl;
		}
#endif // !NDEBUG
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	template<typename Func>
	void Visitor<Impl, AddPointer, PointerCaster, Base>::RegistOne(Func&& func) noexcept {
		using DerivedPointer = Front_t<typename FuncTraits<Func>::ArgList>;
		using Derived = detail::Visitor_::RemovePtr<DerivedPointer>;
		static_assert(std::is_same_v<DerivedPointer, AddPointer<Derived>>);
		static_assert(std::is_base_of_v<Base, Derived>);

		const void* p = vtable_of<Derived>::get();
		assert(p != nullptr);

#ifndef NDEBUG
		if (callbacks.find(p) != callbacks.end()) {
			std::cout << "WARNING::" << typeid(Impl).name() << "::Visit:" << std::endl
				<< "\t" << "repeatedly regist " << typeid(Derived).name() << std::endl;
		}
#endif // !NDEBUG

		callbacks[p] = [func = std::forward<Func>(func)](BasePointer ptrBase) {
			func(PointerCaster::template run<Derived, Base>(ptrBase));
		};
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	template<typename... Funcs>
	void Visitor<Impl, AddPointer, PointerCaster, Base>::Regist(Funcs&&... func) noexcept {
		static_assert(std::is_polymorphic_v<Base>);
		//static_assert(std::is_final_v<Impl>);
		static_assert(IsSet_v<TypeList<detail::Visitor_::RemovePtr<Front_t<typename FuncTraits<Funcs>::ArgList>>...>>);
		(RegistOne<Funcs>(std::forward<Funcs>(func)), ...);
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	template<typename Derived>
	void Visitor<Impl, AddPointer, PointerCaster, Base>::RegistOne() noexcept {
		RegistOne<Derived>(static_cast<Impl*>(this));
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	template<typename Derived>
	inline void Visitor<Impl, AddPointer, PointerCaster, Base>::RegistOne(Impl* impl) noexcept {
		const void* p = vtable_of<Derived>::get();
		assert(p != nullptr);

#ifndef NDEBUG
		if (callbacks.find(p) != callbacks.end()) {
			std::cout << "WARNING::" << typeid(Impl).name() << "::Visit:" << std::endl
				<< "\t" << "repeatedly regist " << typeid(Derived).name() << std::endl;
		}
#endif // !NDEBUG

		callbacks[p] = [impl](BasePointer ptrBase) {
			Accessor::template ImplVisitOf<Derived>(impl, ptrBase);
		};
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	template<typename... Deriveds>
	inline void Visitor<Impl, AddPointer, PointerCaster, Base>::Regist() noexcept {
		static_assert(std::is_polymorphic_v<Base>);
		static_assert(IsSet_v<TypeList<Deriveds...>>);
		(RegistOne<Deriveds>(), ...);
	}

	template<typename Impl, typename Base>
	class RawPtrVisitor
		: public Visitor<Impl, std::add_pointer_t, detail::Visitor_::PointerCaster<std::add_pointer_t>, Base> {};

	template<typename Impl, typename Base>
	class SharedPtrVisitor
		: public Visitor<Impl, std::shared_ptr, detail::Visitor_::PointerCaster<std::shared_ptr>, Base> {};

	template<typename Base>
	class BasicSharedPtrVisitor : public SharedPtrVisitor<detail::Visitor_::VoidImpl, Base> {};

	template<typename Base>
	class BasicRawPtrVisitor : public RawPtrVisitor<detail::Visitor_::VoidImpl, Base> {};
}

namespace Ubpa::detail::Visitor_ {
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
