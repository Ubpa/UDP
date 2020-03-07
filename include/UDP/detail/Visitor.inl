#pragma once

#include <UTemplate/Typelist.h>

#ifndef NDEBUG
#include <iostream>
#endif

namespace Ubpa::detail::Visitor_ {

}

namespace Ubpa {
	template<typename Base, typename Impl, template<typename>class AddPointer, typename PointerCaster>
	void Visitor<Base, Impl, AddPointer,PointerCaster>::Visit(BasePointer& ptrBase) const noexcept {
		// 不是用 typeid(T)，因为可能是多态类
		auto target = visitOps.find(typeid(*ptrBase));
		if (target != visitOps.end())
			target->second(ptrBase);
#ifndef NDEBUG
		else {
			std::cout << "WARNING::" << typeid(Impl).name() << "::Visit:" << std::endl
				<< "\t" << "hasn't regist " << typeid(*ptrBase).name() << std::endl;
		}
#endif // !NDEBUG
	}

	template<typename Base, typename Impl, template<typename>class AddPointer, typename PointerCaster>
	void Visitor<Base, Impl, AddPointer, PointerCaster>::Visit(BasePointer&& ptrBase) const noexcept {
		auto target = visitOps.find(typeid(*ptrBase));
		if (target != visitOps.end())
			target->second(std::move(ptrBase));
#ifndef NDEBUG
		else {
			std::cout << "WARNING::" << typeid(Impl).name() << "::Visit:" << std::endl
				<< "\t" << "hasn't regist " << typeid(*ptrBase).name() << std::endl;
		}
#endif // !NDEBUG
	}

	template<typename Base, typename Impl, template<typename>class AddPointer, typename PointerCaster>
	template<typename Func>
	void Visitor<Base, Impl, AddPointer, PointerCaster>::RegistOne(Func&& func) noexcept {
		using DerivedPointer = Front_t<typename FuncTraits<Func>::ArgList>;
		using Derived = std::decay_t<decltype(*DerivedPointer{ nullptr })>;
		static_assert(std::is_same_v<DerivedPointer, AddPointer<Derived>>);
		static_assert(std::is_base_of_v<Base, Derived>);

#ifndef NDEBUG
		if (visitOps.find(typeid(Derived)) != visitOps.end()) {
			std::cout << "WARNING::" << typeid(Impl).name() << "::Visit:" << std::endl
				<< "\t" << "repeatedly regist " << typeid(Derived).name() << std::endl;
		}
#endif // !NDEBUG


		visitOps[typeid(Derived)] = [func = std::forward<Func>(func)](BasePointer ptrBase) {
			func(PointerCaster::template run<Derived, Base>(ptrBase));
		};
	}

	template<typename Base, typename Impl, template<typename>class AddPointer, typename PointerCaster>
	template<typename... Funcs>
	void Visitor<Base, Impl, AddPointer, PointerCaster>::Regist(Funcs&&... func) noexcept {
		static_assert(IsSet_v<TypeList<std::decay_t<decltype(*Front_t<typename FuncTraits<Funcs>::ArgList>{ nullptr })>...>>);
		(RegistOne<Funcs>(std::forward<Funcs>(func)), ...);
	}

	template<typename Base, typename Impl, template<typename>class AddPointer, typename PointerCaster>
	template<typename Derived>
	void Visitor<Base, Impl, AddPointer, PointerCaster>::RegistOne() noexcept {
		using DerivedPointer = AddPointer<Derived>;
		using Func = void(Impl::*)(DerivedPointer);

#ifndef NDEBUG
		if (visitOps.find(typeid(Derived)) != visitOps.end()) {
			std::cout << "WARNING::" << typeid(Impl).name() << "::Visit:" << std::endl
				<< "\t" << "repeatedly regist " << typeid(Derived).name() << std::endl;
		}
#endif // !NDEBUG

		visitOps[typeid(Derived)] = [impl = static_cast<Impl*>(this)](BasePointer ptrBase) {
			constexpr Func f = &Impl::ImplVisit;
			(impl->*f)(PointerCaster::template run<Derived, Base>(ptrBase));
		};
	}

	template<typename Base, typename Impl, template<typename>class AddPointer, typename PointerCaster>
	template<typename... Deriveds>
	inline void Visitor<Base, Impl, AddPointer, PointerCaster>::Regist() noexcept {
		static_assert(IsSet_v<TypeList<Deriveds...>>);
		(RegistOne<Deriveds>(), ...);
	}

	/*template<typename Base, typename Impl, template<typename>class AddPointer, typename PointerCaster>
	template<typename Derived, typename FuncObj>
	void Visitor<Base, Impl, AddPointer, PointerCaster>::RegistOverloadOne(FuncObj& funcObj) noexcept {
		visitOps[typeid(Derived)] = [&funcObj](BasePointer ptrBase) {
			funcObj(PointerCaster::template run<Derived, Base>(ptrBase));
		};
	}

	template<typename Base, typename Impl, template<typename>class AddPointer, typename PointerCaster>
	template<typename Derived, typename FuncObj>
	void Visitor<Base, Impl, AddPointer, PointerCaster>::RegistOverloadOne(FuncObj&& funcObj) noexcept {
		visitOps[typeid(Derived)] = [funcObj=std::move(funcObj)](BasePointer ptrBase) {
			funcObj(PointerCaster::template run<Derived, Base>(ptrBase));
		};
	}

	template<typename Base, typename Impl, template<typename>class AddPointer, typename PointerCaster>
	template<typename... Deriveds, typename FuncObj>
	void Visitor<Base, Impl, AddPointer, PointerCaster>::RegistOverload(FuncObj&& funcObj) noexcept {
		static_assert(IsSet_v<TypeList<Deriveds...>>);
		(RegistOverloadOne<Deriveds>(std::forward<FuncObj>(funcObj)), ...);
	}*/

	template<typename Base, typename Impl>
	class SharedPtrVisitor
		: public Visitor<Base, Impl, std::shared_ptr> {};
	template<typename Base>
	class SharedPtrVisitor<Base, void> final
		: public Visitor<void, Base, std::shared_ptr> {};

	template<typename Base, typename Impl>
	class RawPtrVisitor
		: public Visitor<Base, Impl> {};
	template<typename Base>
	class RawPtrVisitor<Base, void> final
		: public Visitor<Base, void> {};
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
