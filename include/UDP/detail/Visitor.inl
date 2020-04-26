#pragma once

#include "../Basic/vtable.h"

#include <UTemplate/Func.h>
#include <UTemplate/Concept.h>

namespace Ubpa::detail::Visitor_ {
	template<typename Impl, typename Func>
	Concept(HaveImplVisit, MemFuncOf<Func>::template run<Impl>(&Impl::ImplVisit));
}

namespace Ubpa {
	template<typename Ret, typename... Args>
	Ret Visitor<Ret(Args...)>::Visit(void* ptr, Args... args) const {
		assert("ERROR::Visitor::Visit: not registed" && IsRegisted(ptr));
		return callbacks.find(vtable(ptr))->second(ptr, std::forward<Args>(args)...);
	}

	template<typename Ret, typename... Args>
	Ret Visitor<Ret(Args...)>::Visit(const void* ptr, Args... args) const {
		return Visit(const_cast<void*>(ptr), std::forward<Args>(args)...);
	}

	template<typename Ret, typename... Args>
	template<typename T>
	Ret Visitor<Ret(Args...)>::Visit(T* ptr, Args... args) const {
		static_assert(std::is_polymorphic_v<T>, "Visitor: 'T' must be polymorphic");
		return Visit(reinterpret_cast<void*>(ptr), std::forward<Args>(args)...);
	}

	template<typename Ret, typename... Args>
	template<typename T>
	Ret Visitor<Ret(Args...)>::Visit(const T* ptr, Args... args) const {
		return Visit(const_cast<T*>(ptr), std::forward<Args>(args)...);
	}

	template<typename Ret, typename... Args>
	template<typename... Funcs>
	void Visitor<Ret(Args...)>::Regist(Funcs&&... funcs) {
		(RegistOne(std::forward<Funcs>(funcs)), ...);
	}

	template<typename Ret, typename... Args>
	template<typename Func>
	void Visitor<Ret(Args...)>::RegistOne(Func&& func) {
		using ArgList = FuncTraits_ArgList<Func>;
		using DerivedPointer = Front_t<ArgList>;
		static_assert(std::is_same_v<TypeList<DerivedPointer, Args...>, ArgList>,
			"Visitor::RegistOne: arguments must be ([const] T*, Args...)");

		using Derived = std::remove_pointer_t<DerivedPointer>; // [const] T

		const void* key = vtable_of<std::remove_const_t<Derived>>::get();

		callbacks[key] = [func = std::forward<Func>(func)](void* p, Args... args) {
			return func(reinterpret_cast<Derived*>(p), std::forward<Args>(args)...);
		};
	}

	template<typename Ret, typename... Args>
	template<typename T>
	bool Visitor<Ret(Args...)>::IsRegisted() const {
		static_assert(std::is_polymorphic_v<std::decay_t<T>>,
			"Visitor::IsRegisted: T isn't polymorphic");

		const void* key = vtable_of<std::decay_t<T>>::get();
		return callbacks.find(key) != callbacks.end();
	}

	template<typename Ret, typename... Args>
	bool Visitor<Ret(Args...)>::IsRegisted(const void* ptr) const {
		const void* key = vtable(ptr);
		return callbacks.find(key) != callbacks.end();
	}

	// ============================================================================================

	template<typename Impl, typename Ret, typename... Args>
	struct Visitor<Ret(Impl::*)(Args...)>::Accessor : Impl {
		template<typename Func, typename Derived>
		static Ret run(Impl* impl, void* obj, Args... args) {
			return (impl->*MemFuncOf<Func>::template run<Impl>(&Impl::ImplVisit))
				(reinterpret_cast<Derived*>(obj), std::forward<Args>(args)...);
		}

		static const Impl* cast(const Visitor<Ret(Impl::*)(Args...)>* ptr) noexcept {
			return static_cast<const Impl*>(ptr);
		}
	};

	template<typename Impl, typename Ret, typename... Args>
	inline Ret Visitor<Ret(Impl::*)(Args...)>::Visit(void* ptr, Args... args) const {
		assert("ERROR::Visitor::Visit: not registed" && IsRegisted(ptr));
		auto key = vtable(ptr);
		auto target = impl_callbacks.find(key);
		if (target != impl_callbacks.end())
			return target->second(const_cast<Impl*>(Accessor::cast(this)), ptr, std::forward<Args>(args)...);
		else
			return this->callbacks.find(key)->second(ptr, std::forward<Args>(args)...);
	}
	template<typename Impl, typename Ret, typename... Args>
	inline Ret Visitor<Ret(Impl::*)(Args...)>::Visit(const void* ptr, Args... args) const {
		return Visit(const_cast<void*>(ptr), std::forward<Args>(args)...);
	}

	template<typename Impl, typename Ret, typename... Args>
	template<typename T>
	inline Ret Visitor<Ret(Impl::*)(Args...)>::Visit(T* ptr, Args... args) const {
		static_assert(std::is_polymorphic_v<T>, "Visitor: 'T' must be polymorphic");
		return Visit(reinterpret_cast<void*>(ptr), std::forward<Args>(args)...);
	}
	template<typename Impl, typename Ret, typename... Args>
	template<typename T>
	inline Ret Visitor<Ret(Impl::*)(Args...)>::Visit(const T* ptr, Args... args) const {
		return Visit(const_cast<T*>(ptr), std::forward<Args>(args)...);
	}

	template<typename Impl, typename Ret, typename... Args>
	template<typename... Deriveds>
	void Visitor<Ret(Impl::*)(Args...)>::Regist() {
		(RegistOne<Deriveds>(), ...);
	}

	template<typename Impl, typename Ret, typename... Args>
	template<typename Derived>
	void Visitor<Ret(Impl::*)(Args...)>::RegistOne() {
		auto key = vtable_of<Derived>::get();
		if constexpr (Require<detail::Visitor_::HaveImplVisit, Impl, Ret(Derived*, Args...)>) {
			impl_callbacks[key] = [](Impl* impl, void* ptr, Args... args) {
				return Accessor::template run<Ret(Derived*, Args...), Derived>(impl, ptr, std::forward<Args>(args)...);
			};
		}
		else if constexpr (Require<detail::Visitor_::HaveImplVisit, Impl, Ret(const Derived*, Args...)>) {
			impl_callbacks[key] = [](Impl* impl, void* ptr, Args... args) {
				return Accessor::template run<Ret(const Derived*, Args...), Derived>(impl, ptr, std::forward<Args>(args)...);
			};
		}
		else if constexpr (Require<detail::Visitor_::HaveImplVisit, Impl, Ret(Derived*, Args...) const>) {
			impl_callbacks[key] = [](Impl* impl, void* ptr, Args... args) {
				return Accessor::template run<Ret(Derived*, Args...)const, Derived>(impl, ptr, std::forward<Args>(args)...);
			};
		}
		else if constexpr (Require<detail::Visitor_::HaveImplVisit, Impl, Ret(const Derived*, Args...) const>) {
			impl_callbacks[key] = [](Impl* impl, void* ptr, Args... args) {
				return Accessor::template run<Ret(const Derived*, Args...)const, Derived>(impl, ptr, std::forward<Args>(args)...);
			};
		}
		else {
			static_assert(false, "Visitor::RegistOne: no correct member function ImplVisit");
		}
	}

	template<typename Impl, typename Ret, typename... Args>
	template<typename T>
	inline bool Visitor<Ret(Impl::*)(Args...)>::IsRegisted() const {
		auto key = vtable_of<T>::get();
		return impl_callbacks.find(key) != impl_callbacks.end()
			|| this->callbacks.find(key) != this->callbacks.end();
	}

	template<typename Impl, typename Ret, typename... Args>
	inline bool Visitor<Ret(Impl::*)(Args...)>::IsRegisted(const void* ptr) const {
		auto key = vtable(ptr);
		return impl_callbacks.find(key) != impl_callbacks.end()
			|| this->callbacks.find(key) != this->callbacks.end();
	}
}
