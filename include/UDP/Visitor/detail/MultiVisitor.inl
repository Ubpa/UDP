#pragma once

namespace Ubpa::detail::MultiVisitor_ {
	template<typename Base>
	struct IsDerivedOf {
		template<typename Derived>
		using Ttype = std::is_base_of<Base, Derived>;
	};
	template<typename Derived>
	struct IsBaseOf {
		template<typename Base>
		using Ttype = std::is_base_of<Base, Derived>;
	};

	template<typename List, typename Derived>
	using FilterBase = Front_t<Filter_t<List, IsBaseOf<Derived>::template Ttype>>;
}

namespace Ubpa {
	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename... Bases>
	template<typename DerivedPtr>
	void MultiVisitor<Impl, AddPointer, PointerCaster, Bases...>::RegistVFPtr(DerivedPtr&& ptrDerived) noexcept {
		using Derived = detail::Visitor_::RemovePtr<DerivedPtr>;
		using BaseOfDerived = detail::MultiVisitor_::FilterBase<TypeList<Bases...>, Derived>;
		VisitorOf<BaseOfDerived>::template RegistVFPtr<DerivedPtr>(std::forward<DerivedPtr>(ptrDerived));
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename... Bases>
	template<typename Derived>
	void MultiVisitor<Impl, AddPointer, PointerCaster, Bases...>::RegistOne() noexcept {
		using BaseOfDerived = detail::MultiVisitor_::FilterBase<TypeList<Bases...>, Derived>;
		VisitorOf<BaseOfDerived>::template RegistOne<Derived>(static_cast<Impl*>(this));
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename... Bases>
	template<typename Derived>
	void MultiVisitor<Impl, AddPointer, PointerCaster, Bases...>::RegistOneC() noexcept {
		using BaseOfDerived = detail::MultiVisitor_::FilterBase<TypeList<Bases...>, std::decay_t<Derived>>;
		VisitorOf<BaseOfDerived>::template RegistOneC<Derived>(static_cast<Impl*>(this));
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename... Bases>
	template<typename... Deriveds>
	void MultiVisitor<Impl, AddPointer, PointerCaster, Bases...>::Regist() noexcept {
		using BaseList = TypeList<Bases...>;
		static_assert(((Length_v<Filter_t<BaseList, detail::MultiVisitor_::IsBaseOf<Bases>::template Ttype>> == 1) &&...),
			"there is a base which is base of another type in Bases");
		(RegistOne<Deriveds>(),...);
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename... Bases>
	template<typename... Deriveds>
	void MultiVisitor<Impl, AddPointer, PointerCaster, Bases...>::RegistC() noexcept {
		using BaseList = TypeList<Bases...>;
		static_assert(((Length_v<Filter_t<BaseList, detail::MultiVisitor_::IsBaseOf<Bases>::template Ttype>> == 1) &&...),
			"there is a base which is base of another type in Bases");
		(RegistOneC<Deriveds>(), ...);
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename... Bases>
	template<typename DerivedPtr>
	void MultiVisitor<Impl, AddPointer, PointerCaster, Bases...>::Visit(DerivedPtr&& ptrDerived) const noexcept {
		using Derived = detail::Visitor_::RemovePtr<DerivedPtr>;
		using BaseOfDerived = detail::MultiVisitor_::FilterBase<TypeList<Bases...>, std::decay_t<Derived>>;
		VisitorOf<BaseOfDerived>::Visit(std::forward<DerivedPtr>(ptrDerived));
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename... Bases>
	void MultiVisitor<Impl, AddPointer, PointerCaster, Bases...>::Visit(void* ptr) const noexcept {
#ifndef NDEBUG
		bool success = 
#endif // !NDEBUG
		(VisitOne<Bases>(ptr) || ...);
#ifndef NDEBUG
		if (!success) {
			std::cout << "WARNING::" << typeid(Impl).name() << "::Visit:" << std::endl
				<< "\t" << "hasn't regist" << std::endl;
		}
#endif // !NDEBUG
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename... Bases>
	void MultiVisitor<Impl, AddPointer, PointerCaster, Bases...>::Visit(const void* ptr) const noexcept {
#ifndef NDEBUG
		bool success =
#endif // !NDEBUG
			(VisitOne<Bases>(ptr) || ...);
#ifndef NDEBUG
		if (!success) {
			std::cout << "WARNING::" << typeid(Impl).name() << "::Visit:" << std::endl
				<< "\t" << "hasn't regist" << std::endl;
		}
#endif // !NDEBUG
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename... Bases>
	template<typename Base>
	bool MultiVisitor<Impl, AddPointer, PointerCaster, Bases...>::VisitOne(void* ptr) const {
		const void* vt = vtable(ptr);
		auto target = VisitorOf<Base>::offsets.find(vt);
		if (target != VisitorOf<Base>::offsets.end()) {
			size_t offset = target->second;
			Base* ptrBase = reinterpret_cast<Base*>(reinterpret_cast<size_t>(ptr) + offset);
			VisitorOf<Base>::Visit(ptrBase);
			return true;
		}
		return false;
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename... Bases>
	template<typename Base>
	bool MultiVisitor<Impl, AddPointer, PointerCaster, Bases...>::VisitOne(const void* ptr) const {
		const void* vt = vtable(ptr);
		auto target = VisitorOf<Base>::offsets.find(vt);
		if (target != VisitorOf<Base>::offsets.end()) {
			size_t offset = target->second;
			const Base* ptrCBase = reinterpret_cast<const Base*>(reinterpret_cast<size_t>(ptr) + offset);
			VisitorOf<Base>::Visit(ptrCBase);
			return true;
		}
		return false;
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename... Bases>
	template<typename Func>
	void MultiVisitor<Impl, AddPointer, PointerCaster, Bases...>::RegistOne(Func&& func) noexcept {
		using DerivedPointer = Front_t<typename FuncTraits<Func>::ArgList>;
		using Derived = detail::Visitor_::RemovePtr<DerivedPointer>;
		using BaseOfDerived = detail::MultiVisitor_::FilterBase<TypeList<Bases...>, std::decay_t<Derived>>;
		VisitorOf<BaseOfDerived>::RegistOne(std::forward<Func>(func));
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename... Bases>
	template<typename... Funcs>
	void MultiVisitor<Impl, AddPointer, PointerCaster, Bases...>::Regist(Funcs&&... func) noexcept {
		using BaseList = TypeList<Bases...>;
		static_assert(((Length_v<Filter_t<BaseList, detail::MultiVisitor_::IsBaseOf<Bases>::template Ttype>> == 1) &&...),
			"there is a base which is base of another type in Bases");
		(RegistOne(func), ...);
	}

	// ====================================

	template<typename Impl, typename... Bases>
	class RawPtrMultiVisitor
		: public MultiVisitor<Impl, std::add_pointer_t, detail::Visitor_::PointerCaster<std::add_pointer_t>, Bases...> {};

	template<typename... Bases>
	class BasicRawPtrMultiVisitor
		: public RawPtrMultiVisitor<detail::Visitor_::VoidImpl, Bases...> {};

	template<typename Impl, typename... Bases>
	class SharedPtrMultiVisitor
		: public MultiVisitor<Impl, std::shared_ptr, detail::Visitor_::PointerCaster<std::shared_ptr>, Bases...> {};

	template<typename... Bases>
	class BasicSharedPtrMultiVisitor
		: public SharedPtrMultiVisitor<detail::Visitor_::VoidImpl, Bases...> {};
}
