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
	template<typename... Deriveds>
	void MultiVisitor<Impl, AddPointer, PointerCaster, Bases...>::Regist() noexcept {
		using BaseList = TypeList<Bases...>;
		static_assert(((Length_v<Filter_t<BaseList, detail::MultiVisitor_::IsBaseOf<Bases>::template Ttype>> == 1) &&...),
			"there is a base which is base of another type in Bases");
		(RegistOne<Deriveds>(),...);
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename... Bases>
	template<typename DerivedPtr>
	void MultiVisitor<Impl, AddPointer, PointerCaster, Bases...>::Visit(DerivedPtr&& ptrDerived) const noexcept {
		using Derived = detail::Visitor_::RemovePtr<DerivedPtr>;
		using BaseOfDerived = detail::MultiVisitor_::FilterBase<TypeList<Bases...>, Derived>;
		VisitorOf<BaseOfDerived>::Visit(PointerCaster::template run<Derived, BaseOfDerived>(std::forward<DerivedPtr>(ptrDerived)));
	}

	template<typename Impl, template<typename>class AddPointer, typename PointerCaster, typename... Bases>
	template<typename Func>
	void MultiVisitor<Impl, AddPointer, PointerCaster, Bases...>::RegistOne(Func&& func) noexcept {
		using DerivedPointer = Front_t<typename FuncTraits<Func>::ArgList>;
		using Derived = detail::Visitor_::RemovePtr<DerivedPointer>;
		using BaseOfDerived = detail::MultiVisitor_::FilterBase<TypeList<Bases...>, Derived>;
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
