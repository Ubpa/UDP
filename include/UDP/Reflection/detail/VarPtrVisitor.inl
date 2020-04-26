#pragma once

namespace Ubpa {
	template<typename Impl>
	struct VarPtrVisitor<Impl>::Accessor : Impl {
		template<typename T>
		static void run(Impl* impl, T& val) {
			if constexpr (Require<detail::Visitor_::HaveImplVisit, Impl, void(T&)>) {
				(impl->*MemFuncOf<void(T&)>::template run<Impl>(&Impl::ImplVisit))(val);
			}
			else if constexpr (Require<detail::Visitor_::HaveImplVisit, Impl, void(const T&)>) {
				(impl->*MemFuncOf<void(const T&)>::template run<Impl>(&Impl::ImplVisit))(val);
			}
			else if constexpr (Require<detail::Visitor_::HaveImplVisit, Impl, void(T&) const>) {
				(impl->*MemFuncOf<void(T&) const>::template run<Impl>(&Impl::ImplVisit))(val);
			}
			else if constexpr (Require<detail::Visitor_::HaveImplVisit, Impl, void(const T&) const>) {
				(impl->*MemFuncOf<void(const T&) const>::template run<Impl>(&Impl::ImplVisit))(val);
			}
			else {
				static_assert(false, "VarPtrVisitor::Accessor: <Impl> has no correct ImplVisit");
			}
		}
	};

	template<typename Impl>
	template<typename T>
	void VarPtrVisitor<Impl>::ImplVisit(VarPtr<T>* p) const {
		Accessor::template run(const_cast<Impl*>(static_cast<const Impl*>(this)), **p);
	}
}
