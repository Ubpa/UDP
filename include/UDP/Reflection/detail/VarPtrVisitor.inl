#pragma once

namespace Ubpa {
	template<typename Ret, typename... Args>
	template<typename Func>
	void VarPtrVisitor<Ret(Args...)>::RegisterOne(Func&& func) {
		using ArgList = FuncTraits_ArgList<Func>;
		using VarRef = Front_t<ArgList>;
		static_assert(std::is_reference_v<VarRef>,
			"ERROR::VarPtrVisitor::RegisterOne: <Func>'s first argument must be [const] <T>& ");
		static_assert(std::is_same_v<ArgList, TypeList<VarRef, Args...>>,
			"ERROR::VarPtrVisitor::RegisterOne: <Func>'s argument list must be ([const] <T>&, Args...) ");
		visitor.Register([func = std::forward<Func>(func)](VarPtr<std::decay_t<VarRef>>* ptr, Args... args) -> Ret {
			return func(**ptr, std::forward<Args>(args...));
		});
	}

	template<typename Ret, typename... Args>
	template<typename... Funcs>
	void VarPtrVisitor<Ret(Args...)>::Register(Funcs&&... funcs) {
		(RegisterOne(std::forward<Funcs>(funcs)), ...);
	}

	// ================================================================================

	template<typename Impl, typename Ret, typename... Args>
	struct VarPtrVisitor<Ret(Impl::*)(Args...)>::Accessor : Impl {
		template<typename T>
		static Ret run(Impl* impl, T& val, Args... args) {
			if constexpr (Require<detail::Visitor_::HaveImplVisit, Impl, Ret(T&, Args...)>) {
				return (impl->*MemFuncOf<Ret(T&, Args...)>::template run<Impl>(&Impl::ImplVisit))(val, std::forward<Args>(args)...);
			}
			else if constexpr (Require<detail::Visitor_::HaveImplVisit, Impl, Ret(const T&, Args...)>) {
				return (impl->*MemFuncOf<Ret(const T&, Args...)>::template run<Impl>(&Impl::ImplVisit))(val, std::forward<Args>(args)...);
			}
			else if constexpr (Require<detail::Visitor_::HaveImplVisit, Impl, Ret(T&, Args...) const>) {
				return (impl->*MemFuncOf<Ret(T&, Args...) const>::template run<Impl>(&Impl::ImplVisit))(val, std::forward<Args>(args)...);
			}
			else if constexpr (Require<detail::Visitor_::HaveImplVisit, Impl, Ret(const T&, Args...) const>) {
				return (impl->*MemFuncOf<Ret(const T&, Args...) const>::template run<Impl>(&Impl::ImplVisit))(val, std::forward<Args>(args)...);
			}
			else
				static_assert(false, "VarPtrVisitor::Accessor: <Impl> has no correct ImplVisit");
		}

		static const Impl* cast(const VarPtrVisitor<Ret(Impl::*)(Args...)>* ptr) noexcept {
			return static_cast<const Impl*>(ptr);
		}
	};

	template<typename Impl, typename Ret, typename... Args>
	template<typename T>
	Ret VarPtrVisitor<Ret(Impl::*)(Args...)>::ImplVisit(VarPtr<T>* p, Args... args) const {
		return Accessor::template run(const_cast<Impl*>(Accessor::template cast(this)), **p, std::forward<Args>(args)...);
	}
}
