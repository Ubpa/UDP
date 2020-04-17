#pragma once

#include "../../Basic/vtable.h"

#include <UTemplate/Func.h>

#ifndef NDEBUG
#include <iostream>
#endif

namespace Ubpa::detail::InfVisitor_ {
	template<typename Pointer>
	using RemovePtr = std::remove_reference_t<decltype(*std::remove_reference_t<Pointer>{ nullptr }) > ;

	struct VoidImpl final {};
}

namespace Ubpa {
	// ref: https://stackoverflow.com/questions/8523762/crtp-with-protected-derived-member
	template<typename Impl>
	struct InfVisitor<Impl>::Accessor : public Impl {
		static_assert(!std::is_final<Impl>, "InfVisitor: 'Impl' must be non-final");

		template<typename Derived>
		inline static void ImplVisitOf(Impl* const impl, Derived* ptr) noexcept {
			constexpr void(Impl:: * f)(Derived*) = &Impl::ImplVisit;
			(impl->*f)(ptr);
		}

		template<typename Derived>
		inline static void ImplVisitOfC(Impl* const impl, const Derived* ptr) noexcept {
			constexpr void(Impl:: * f)(const Derived*) = &Impl::ImplVisit;
			(impl->*f)(ptr);
		}
	};

	template<typename Impl>
	template<typename T>
	void InfVisitor<Impl>::Visit(T* ptr) {
		static_assert(std::is_polymorphic_v<T>, "InfVisitor: 'T' must be polymorphic");
		Visit(reinterpret_cast<void*>(ptr));
	}

	template<typename Impl>
	void InfVisitor<Impl>::Visit(void* ptr) {
		auto vt = vtable(ptr);
		auto target = callbacks.find(vt);
		if (target != callbacks.end())
			target->second(ptr);
		else {
			auto target = const_callbacks.find(vt);
			if (target != const_callbacks.end())
				target->second(ptr);
#ifndef NDEBUG
			else {
				std::cout << "WARNING::" << typeid(Impl).name() << "::Visit:" << std::endl
					<< "\t" << "hasn't regist" << std::endl;
			}
#endif // !NDEBUG
		}
	}

	template<typename Impl>
	template<typename T>
	void InfVisitor<Impl>::Visit(const T* ptr) {
		static_assert(std::is_polymorphic_v<T>, "InfVisitor: 'T' must be polymorphic");
		Visit(reinterpret_cast<const void*>(ptr));
	}

	template<typename Impl>
	void InfVisitor<Impl>::Visit(const void* ptr) {
		auto vt = vtable(ptr);
		auto target = const_callbacks.find(vt);
		if (target != const_callbacks.end())
			target->second(ptr);
#ifndef NDEBUG
		else {
			std::cout << "WARNING::" << typeid(Impl).name() << "::Visit:" << std::endl
				<< "\t" << "hasn't regist" << std::endl;
		}
#endif // !NDEBUG
	}

	template<typename Impl>
	template<typename... Deriveds>
	void InfVisitor<Impl>::Regist() {
		static_assert(IsSet_v<TypeList<Deriveds...>>);
		(RegistOne<Deriveds>(), ...);
	}

	template<typename Impl>
	template<typename... Deriveds>
	void InfVisitor<Impl>::RegistC() {
		static_assert(IsSet_v<TypeList<Deriveds...>>);
		(RegistOneC<Deriveds>(), ...);
	}

	template<typename Impl>
	template<typename Derived>
	void InfVisitor<Impl>::RegistOne() {
		static_assert(std::is_polymorphic_v<Derived>, "InfVisitor: 'Derived' must be polymorphic");
		const void* vt = vtable_of<Derived>::get();
		assert(vt != nullptr);
#ifndef NDEBUG
		if (callbacks.find(vt) != callbacks.end()) {
			std::cout << "WARNING::" << typeid(Impl).name() << "::Visit:" << std::endl
				<< "\t" << "repeatedly regist " << typeid(Derived).name() << std::endl;
		}
#endif // !NDEBUG
		callbacks[vt] = [impl = static_cast<Impl*>(this)](void* p) {
			Accessor::template ImplVisitOf<Derived>(impl, reinterpret_cast<Derived*>(p));
		};
	}

	template<typename Impl>
	template<typename Derived>
	void InfVisitor<Impl>::RegistOneC() {
		static_assert(std::is_polymorphic_v<Derived>, "InfVisitor: 'Derived' must be polymorphic");
		const void* vt = vtable_of<Derived>::get();
		assert(vt != nullptr);
#ifndef NDEBUG
		if (const_callbacks.find(vt) != const_callbacks.end()) {
			std::cout << "WARNING::" << typeid(Impl).name() << "::Visit:" << std::endl
				<< "\t" << "repeatedly regist " << typeid(Derived).name() << std::endl;
		}
#endif // !NDEBUG
		const_callbacks[vt] = [impl = static_cast<Impl*>(this)](const void* p) {
			Accessor::template ImplVisitOfC<Derived>(impl, reinterpret_cast<const Derived*>(p));
		};
	}

	template<typename Impl>
	template<typename... Funcs>
	void InfVisitor<Impl>::Regist(Funcs&&... funcs) {
		static_assert(IsSet_v<TypeList<detail::InfVisitor_::RemovePtr<Front_t<typename FuncTraits<Funcs>::ArgList>>...>>,
			"Funcs's auguments must be different");
		(RegistOne<Funcs>(std::forward<Funcs>(funcs)), ...);
	}

	template<typename Impl>
	template<typename Func>
	void InfVisitor<Impl>::RegistOne(Func&& func) {
		using DerivedPointer = Front_t<typename FuncTraits<Func>::ArgList>;
		using Derived = detail::InfVisitor_::RemovePtr<DerivedPointer>; // [const] T
		static_assert(std::is_same_v<DerivedPointer, Derived*>); // [const] T*

		const void* p = vtable_of<std::decay_t<Derived>>::get();
		assert(p != nullptr);

#ifndef NDEBUG
		if constexpr (std::is_const_v<Derived>) {
			if (const_callbacks.find(p) != const_callbacks.end()) {
				std::cout << "WARNING::" << typeid(Impl).name() << "::Visit:" << std::endl
					<< "\t" << "repeatedly regist " << typeid(Derived).name() << std::endl;
			}
		}
		else {
			if (callbacks.find(p) != callbacks.end()) {
				std::cout << "WARNING::" << typeid(Impl).name() << "::Visit:" << std::endl
					<< "\t" << "repeatedly regist " << typeid(Derived).name() << std::endl;
			}
		}
#endif // !NDEBUG

		if constexpr (std::is_const_v<Derived>) {
			const_callbacks[p] = [func = std::forward<Func>(func)](const void* p) {
				func(reinterpret_cast<const Derived*>(p));
			};
		}
		else {
			callbacks[p] = [func = std::forward<Func>(func)](void* p) {
				func(reinterpret_cast<Derived*>(p));
			};
		}
	}

	template<typename Impl>
	template<typename T>
	bool InfVisitor<Impl>::IsRegisted() const {
		if constexpr (!std::is_polymorphic_v<T>)
			return false;
		else {
			const void* p = vtable_of<T>::get();
			return p != nullptr && (
				callbacks.find(p) != callbacks.end()
				|| const_callbacks.find(p) != const_callbacks.end());
		}
	}

	template<typename Impl>
	bool InfVisitor<Impl>::IsRegisted(void* ptr) const {
		const void* p = vtable(ptr);
		return p != nullptr && (
			callbacks.find(p) != callbacks.end()
			|| const_callbacks.find(p) != const_callbacks.end());
	}

	template<typename Impl>
	bool InfVisitor<Impl>::IsRegisted(const void* ptr) const {
		const void* p = vtable(ptr);
		return p != nullptr && const_callbacks.find(p) != const_callbacks.end();
	}
}
