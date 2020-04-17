#pragma once

#include "VarPtr.h"
#include "../Visitor/Visitor.h"

namespace Ubpa {
	template<typename Impl>
	class VarPtrVisitor : public SharedPtrVisitor<VarPtrVisitor<Impl>, VarPtrBase> {
	public:
		template<typename... Funcs>
		inline void Regist(Funcs&&... funcs) noexcept;

	protected:
		template<typename... Ts>
		inline void Regist() noexcept; // ImplVisit(Ts&)

		template<typename... Ts>
		inline void RegistC() noexcept; // ImplVisit(const Ts&)

	// ================================================================================

	private:
		friend struct SharedPtrVisitor<VarPtrVisitor<Impl>, VarPtrBase>::Accessor;

		template<typename T>
		inline void ImplVisit(std::shared_ptr<VarPtr<T>> p);

		template<typename T>
		inline void ImplVisit(std::shared_ptr<const VarPtr<T>> p);

	private:
		template<typename Func>
		inline void RegistOne(Func&& func) noexcept;
		template<typename T> struct Accessor;
	};
}

#include "detail/VarPtrVisitor.inl"
