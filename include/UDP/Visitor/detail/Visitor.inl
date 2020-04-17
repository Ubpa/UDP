#pragma once

#include "../../Basic/vtable.h"

#include <UTemplate/Typelist.h>
#include <cassert>

#ifndef NDEBUG
#include <iostream>
#endif

namespace Ubpa::detail::Visitor_ {
	template<typename Pointer>
	using RemovePtr = std::remove_reference_t<decltype(*std::remove_reference_t<Pointer>{ nullptr })>;

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
		template<typename Derived>
		inline static void ImplVisitOfC(Impl* const impl, AddPointer<const Base> ptrCBase) noexcept {
			constexpr void(Impl:: * f)(AddPointer<const Derived>) = &Impl::ImplVisit;
			(impl->*f)(PointerCaster::template run<const Derived, const Base>(ptrCBase));
		}
	};

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	void Visitor<Impl, AddPointer, PointerCaster, Base>::Visit(void* ptr) const noexcept {
		const void* vt = vtable(ptr);
		auto offsetTarget = offsets.find(vt);
		if (offsetTarget != offsets.end()) {
			size_t offset = offsetTarget->second;
			Base* ptrBase = reinterpret_cast<Base*>(reinterpret_cast<size_t>(ptr) + offset);
			Visit(ptrBase);
		}
#ifndef NDEBUG
		else {
			std::cout << "WARNING::" << typeid(Impl).name() << "::Visit:" << std::endl
				<< "\t" << "hasn't regist" << std::endl;
		}
#endif // !NDEBUG
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	void Visitor<Impl, AddPointer, PointerCaster, Base>::Visit(const void* ptr) const noexcept {
		const void* vt = vtable(ptr);
		auto offsetTarget = offsets.find(vt);
		if (offsetTarget != offsets.end()) {
			size_t offset = offsetTarget->second;
			const Base* ptrCBase = reinterpret_cast<const Base*>(reinterpret_cast<size_t>(ptr) + offset);
			Visit(ptrCBase);
		}
#ifndef NDEBUG
		else {
			std::cout << "WARNING::" << typeid(Impl).name() << "::Visit:" << std::endl
				<< "\t" << "hasn't regist" << std::endl;
		}
#endif // !NDEBUG
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	void Visitor<Impl, AddPointer, PointerCaster, Base>::Visit(BasePointer& ptrBase) const noexcept {
		auto target = callbacks.find(vtable(ptrBase));
		if (target != callbacks.end())
			target->second(ptrBase);
		else {
			auto ctarget = const_callbacks.find(vtable(ptrBase));
			if (ctarget != const_callbacks.end())
				ctarget->second(ptrBase);
#ifndef NDEBUG
			else {
				std::cout << "WARNING::" << typeid(Impl).name() << "::Visit:" << std::endl
					<< "\t" << "hasn't regist " << typeid(*ptrBase).name() << std::endl;
			}
#endif // !NDEBUG
		}
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	void Visitor<Impl, AddPointer, PointerCaster, Base>::Visit(BasePointer&& ptrBase) const noexcept {
		auto target = callbacks.find(vtable(ptrBase));
		if (target != callbacks.end())
			target->second(std::forward<BasePointer>(ptrBase));
		else {
			auto ctarget = const_callbacks.find(vtable(ptrBase));
			if (ctarget != const_callbacks.end())
				ctarget->second(std::forward<BasePointer>(ptrBase));
#ifndef NDEBUG
			else {
			std::cout << "WARNING::" << typeid(Impl).name() << "::Visit:" << std::endl
				<< "\t" << "hasn't regist " << typeid(*ptrBase).name() << std::endl;
			}
#endif // !NDEBUG
		}
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	void Visitor<Impl, AddPointer, PointerCaster, Base>::Visit(CBasePointer& ptrCBase) const noexcept {
		auto ctarget = const_callbacks.find(vtable(ptrCBase));
		if (ctarget != const_callbacks.end())
			ctarget->second(ptrCBase);
#ifndef NDEBUG
		else {
			std::cout << "WARNING::" << typeid(Impl).name() << "::Visit:" << std::endl
				<< "\t" << "hasn't regist " << typeid(*ptrCBase).name() << std::endl;
		}
#endif // !NDEBUG
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	void Visitor<Impl, AddPointer, PointerCaster, Base>::Visit(CBasePointer&& ptrCBase) const noexcept {
		auto ctarget = const_callbacks.find(vtable(ptrCBase));
		if (ctarget != const_callbacks.end())
			ctarget->second(std::forward<CBasePointer>(ptrCBase));
#ifndef NDEBUG
		else {
			std::cout << "WARNING::" << typeid(Impl).name() << "::Visit:" << std::endl
				<< "\t" << "hasn't regist " << typeid(*ptrCBase).name() << std::endl;
		}
#endif // !NDEBUG
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	template<typename Func>
	void Visitor<Impl, AddPointer, PointerCaster, Base>::RegistOne(Func&& func) noexcept {
		using DerivedPointer = Front_t<typename FuncTraits<Func>::ArgList>;
		using Derived = detail::Visitor_::RemovePtr<DerivedPointer>;
		static_assert(std::is_same_v<DerivedPointer, AddPointer<Derived>>);
		static_assert(std::is_base_of_v<Base, std::decay_t<Derived>>);

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
			const_callbacks[p] = [func = std::forward<Func>(func)](CBasePointer ptrCBase) {
				func(PointerCaster::template run<Derived, const Base>(ptrCBase));
			};
		}
		else {
			callbacks[p] = [func = std::forward<Func>(func)](BasePointer ptrBase) {
				func(PointerCaster::template run<Derived, Base>(ptrBase));
			};
		}

		offsets[p] = offset<Base, std::decay_t<Derived>>();
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	template<typename... Funcs>
	void Visitor<Impl, AddPointer, PointerCaster, Base>::Regist(Funcs&&... funcs) noexcept {
		static_assert(std::is_polymorphic_v<Base>);
		static_assert(IsSet_v<TypeList<detail::Visitor_::RemovePtr<Front_t<typename FuncTraits<Funcs>::ArgList>>...>>);
		(RegistOne<Funcs>(std::forward<Funcs>(funcs)), ...);
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	template<typename Derived>
	void Visitor<Impl, AddPointer, PointerCaster, Base>::RegistOne() noexcept {
		RegistOne<Derived>(static_cast<Impl*>(this));
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	template<typename Derived>
	void Visitor<Impl, AddPointer, PointerCaster, Base>::RegistOneC() noexcept {
		RegistOneC<Derived>(static_cast<Impl*>(this));
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	template<typename Derived>
	inline void Visitor<Impl, AddPointer, PointerCaster, Base>::RegistOne(Impl* impl) noexcept {
		static_assert(!std::is_const_v<Derived>);

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

		offsets[p] = offset<Base, Derived>();
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	template<typename Derived>
	inline void Visitor<Impl, AddPointer, PointerCaster, Base>::RegistOneC(Impl* impl) noexcept {
		static_assert(!std::is_const_v<Derived>);

		const void* p = vtable_of<Derived>::get();
		assert(p != nullptr);

#ifndef NDEBUG
		if (const_callbacks.find(p) != const_callbacks.end()) {
			std::cout << "WARNING::" << typeid(Impl).name() << "::Visit:" << std::endl
				<< "\t" << "repeatedly regist " << typeid(Derived).name() << std::endl;
		}
#endif // !NDEBUG

		const_callbacks[p] = [impl](CBasePointer ptrCBase) {
			Accessor::template ImplVisitOfC<Derived>(impl, ptrCBase);
		};

		offsets[p] = offset<Base, Derived>();
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	template<typename... Deriveds>
	inline void Visitor<Impl, AddPointer, PointerCaster, Base>::Regist() noexcept {
		static_assert(std::is_polymorphic_v<Base>);
		static_assert(IsSet_v<TypeList<Deriveds...>>);
		(RegistOne<Deriveds>(), ...);
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	template<typename... Deriveds>
	inline void Visitor<Impl, AddPointer, PointerCaster, Base>::RegistC() noexcept {
		static_assert(std::is_polymorphic_v<Base>);
		static_assert(IsSet_v<TypeList<Deriveds...>>);
		(RegistOneC<Deriveds>(), ...);
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	template<typename T>
	bool Visitor<Impl, AddPointer, PointerCaster, Base>::IsRegisted() const {
		if constexpr (!std::is_polymorphic_v<T>)
			return false;
		else {
			const void* p = vtable_of<T>::get();
			return p != nullptr && (
				callbacks.find(p) != callbacks.end()
				|| const_callbacks.find(p) != const_callbacks.end());
		}
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	bool Visitor<Impl, AddPointer, PointerCaster, Base>::IsRegisted(void* ptr) const {
		const void* p = vtable(ptr);
		return p != nullptr && (
			callbacks.find(p) != callbacks.end()
			|| const_callbacks.find(p) != const_callbacks.end());
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	bool Visitor<Impl, AddPointer, PointerCaster, Base>::IsRegisted(const void* ptr) const {
		const void* p = vtable(ptr);
		return p != nullptr && const_callbacks.find(p) != const_callbacks.end();
	}


	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	bool Visitor<Impl, AddPointer, PointerCaster, Base>::IsRegisted(BasePointer ptrBase) const noexcept {
		return IsRegisted(&(*ptrBase));
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename Base>
	bool Visitor<Impl, AddPointer, PointerCaster, Base>::IsRegisted(CBasePointer ptrCBase) const noexcept {
		return IsRegisted(&(*ptrCBase));
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
