#pragma once

#ifndef NDEBUG
#include <iostream>
#endif

namespace Ubpa {
	template<typename Impl, typename Base, template<typename>class AddPointer, typename PointerCaster>
	void Visitor<Impl,Base,AddPointer,PointerCaster>::Visit(BasePointer& ptr_base) const noexcept {
		// 不是用 typeid(T)，因为可能是多态类
		auto target = visitOps.find(typeid(*ptr_base));
		if (target != visitOps.end())
			target->second(ptr_base);
#ifndef NDEBUG
		else {
			std::cout << "WARNING::" << typeid(Impl).name() << "::Visit:" << std::endl
				<< "\t" << "hasn't regist " << typeid(*ptr_base).name() << std::endl;
		}
#endif // !NDEBUG
	}

	template<typename Impl, typename Base, template<typename>class AddPointer, typename PointerCaster>
	void Visitor<Impl, Base, AddPointer, PointerCaster>::Visit(BasePointer&& ptr_base) const noexcept {
		auto target = visitOps.find(typeid(*ptr_base));
		if (target != visitOps.end())
			target->second(std::move(ptr_base));
#ifndef NDEBUG
		else {
			std::cout << "WARNING::" << typeid(Impl).name() << "::Visit:" << std::endl
				<< "\t" << "hasn't regist " << typeid(*ptr_base).name() << std::endl;
		}
#endif // !NDEBUG
	}

	template<typename Impl, typename Base, template<typename>class AddPointer, typename PointerCaster>
	template<typename Func>
	void Visitor<Impl, Base, AddPointer, PointerCaster>::Regist(Func&& func) noexcept {
		using DerivedPointer = Front_t<typename FuncTraits<Func>::ArgList>;
		using Derived = std::decay_t<decltype(*DerivedPointer{ nullptr }) > ;
		static_assert(std::is_same_v<DerivedPointer, AddPointer<Derived>>);
		static_assert(std::is_base_of_v<Base, Derived>);

#ifndef NDEBUG
		if (visitOps.find(typeid(Derived)) != visitOps.end()) {
			std::cout << "WARNING::" << typeid(Impl).name() << "::Visit:" << std::endl
				<< "\t" << "repeatedly regist " << typeid(Derived).name() << std::endl;
		}
#endif // !NDEBUG


		visitOps[typeid(Derived)] = [func = std::forward<Func>(func)](BasePointer ptr_base) {
			func(PointerCaster::template run<Derived, Base>(ptr_base));
		};
	}

	template<typename Impl, typename Base, template<typename>class AddPointer, typename PointerCaster>
	template<typename Derived>
	void Visitor<Impl, Base, AddPointer, PointerCaster>::Regist() noexcept {
		using DerivedPointer = AddPointer<Derived>;
		using Func = void(Impl::*)(DerivedPointer);

#ifndef NDEBUG
		if (visitOps.find(typeid(Derived)) != visitOps.end()) {
			std::cout << "WARNING::" << typeid(Impl).name() << "::Visit:" << std::endl
				<< "\t" << "repeatedly regist " << typeid(Derived).name() << std::endl;
		}
#endif // !NDEBUG

		visitOps[typeid(Derived)] = [impl = static_cast<Impl*>(this)](BasePointer ptr_base) {
			constexpr Func f = &Impl::ImplVisit;
			(impl->*f)(PointerCaster::template run<Derived, Base>(ptr_base));
		};
	}


	template<typename Base>
	class SharedPtrVisitor final
		: public Visitor<SharedPtrVisitor<Base>, Base, std::shared_ptr> {};

	template<typename Base>
	class RawPtrVisitor final
		: public Visitor<RawPtrVisitor<Base>, Base> {};
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
