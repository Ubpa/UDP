#pragma once

#include "VarPtr.h"
#include "../Visitor.h"

namespace Ubpa {
	template<typename Impl>
	class VarPtrVisitor : protected Visitor<void(VarPtrVisitor<Impl>::*)()> {
		using Base = Visitor<void(VarPtrVisitor<Impl>::*)()>;
	public:
		void Visit(std::shared_ptr<VarPtrBase> p) const {
			Base::Visit(&(*p));
		}

		bool IsRegistered(std::shared_ptr<VarPtrBase> p) const {
			return Base::IsRegistered(&(*p));
		}

		template<typename T>
		bool IsRegistered() const {
			return Base::template IsRegistered<VarPtr<T>>();
		}

	protected:
		// ImplVisit(Ts&)
		template<typename... Ts>
		void Register() noexcept {
			Base::template Register<VarPtr<Ts>...>();
		}

	// ================================================================================

	private:
		friend struct Visitor<void(VarPtrVisitor<Impl>::*)()>::Accessor;

		struct Accessor;

		template<typename T>
		inline void ImplVisit(VarPtr<T>* p) const;
	};
}

#include "detail/VarPtrVisitor.inl"
