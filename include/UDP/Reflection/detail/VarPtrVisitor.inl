#pragma once

namespace Ubpa {
	template<typename Impl>
	template<typename... Ts>
	inline void VarPtrVisitor<Impl>::Regist() noexcept {
		SharedPtrVisitor<VarPtrVisitor<Impl>, VarPtrBase>::template Regist<VarPtr<Ts>...>();
	}

	template<typename Impl>
	template<typename... Ts>
	inline void VarPtrVisitor<Impl>::RegistC() noexcept {
		SharedPtrVisitor<VarPtrVisitor<Impl>, VarPtrBase>::template RegistC<VarPtr<Ts>...>();
	}

	/*template<typename Impl>
	template<typename T>
	bool VarPtrVisitor<Impl>::IsRegisted() const {
		Base::IsRegisted<VarPtr<T>>();
	}*/

	template<typename Impl>
	template<typename T>
	struct VarPtrVisitor<Impl>::Accessor : Impl {
		inline static void ImplVisitOf(Impl* const impl, T& val) noexcept {
			constexpr void(Impl:: * f)(T&) = &Impl::ImplVisit;
			(impl->*f)(val);
		}

		inline static void ImplVisitOfC(Impl* const impl, const T& val) noexcept {
			constexpr void(Impl:: * f)(const T&) = &Impl::ImplVisit;
			(impl->*f)(val);
		}
	};

	template<typename Impl>
	template<typename... Funcs>
	void VarPtrVisitor<Impl>::Regist(Funcs&&... funcs) noexcept {
		(RegistOne<Funcs>(std::forward<Funcs>(funcs)), ...);
	}

	template<typename Impl>
	template<typename Func>
	void VarPtrVisitor<Impl>::RegistOne(Func&& func) noexcept {
		using Val = std::remove_reference_t<Front_t<FuncTraits_ArgList<Func>>>;
		if constexpr (std::is_const_v<Val>) {
			using PconstValPtr = std::shared_ptr<const VarPtr<std::remove_const_t<Val>>>;
			SharedPtrVisitor<VarPtrVisitor<Impl>, VarPtrBase>::Regist(
				[func = std::forward<Func>(func)](PconstValPtr p) {func(**p); }
			);
		}
		else {
			using PconstValPtr = std::shared_ptr<VarPtr<Val>>;
			SharedPtrVisitor<VarPtrVisitor<Impl>, VarPtrBase>::Regist(
				[func = std::forward<Func>(func)](PconstValPtr p) {func(**p); }
			);
		}
	}

	template<typename Impl>
	template<typename T>
	void VarPtrVisitor<Impl>::ImplVisit(std::shared_ptr<VarPtr<T>> p) {
		Accessor<T>::ImplVisitOf(static_cast<Impl*>(this), **p);
	}

	template<typename Impl>
	template<typename T>
	void VarPtrVisitor<Impl>::ImplVisit(std::shared_ptr<const VarPtr<T>> p) {
		Accessor<T>::ImplVisitOfC(static_cast<Impl*>(this), **p);
	}
}
