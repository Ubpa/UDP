#pragma once

#include "VarPtr.h"
#include "../Visitor.h"

#include <type_traits>

namespace Ubpa {
	template<typename Func>
	class VarPtrVisitor;

	template<typename Ret, typename... Args>
	class VarPtrVisitor<Ret(Args...)> {
	public:
		// lambda(Ts&, Args...)
		template<typename... Funcs>
		void Register(Funcs&&... funcs);

		Ret Visit(std::shared_ptr<VarPtrBase> p, Args... args) const {
			return visitor.Visit(&(*p), std::forward<Args>(args)...);
		}

		bool IsRegistered(std::shared_ptr<VarPtrBase> p) const {
			return visitor.IsRegistered(&(*p));
		}

		template<typename T>
		bool IsRegistered() const {
			return visitor.template IsRegistered<VarPtr<T>>();
		}

	private:
		Visitor<Ret(Args...)> visitor;

		// lambda(Ts&, Args...)
		template<typename Func>
		void RegisterOne(Func&& func);
	};


	template<typename Impl, typename Ret, typename... Args>
	class VarPtrVisitor<Ret(Impl::*)(Args...)>
		: protected Visitor<Ret(VarPtrVisitor<Ret(Impl::*)(Args...)>::*)(Args...)>
	{
		using Base = Visitor<Ret(VarPtrVisitor<Ret(Impl::*)(Args...)>::*)(Args...)>;
	public:
		Ret Visit(std::shared_ptr<VarPtrBase> p, Args... args) const {
			return Base::Visit(&(*p), std::forward<Args>(args)...);
		}

		bool IsRegistered(std::shared_ptr<VarPtrBase> p) const {
			return Base::IsRegistered(&(*p));
		}

		template<typename T>
		bool IsRegistered() const {
			return Base::template IsRegistered<VarPtr<T>>();
		}

	protected:
		// ImplVisit(Ts&, Args...)
		template<typename... Ts>
		void Register() noexcept {
			Base::template Register<VarPtr<Ts>...>();
		}

	private:
		friend struct Base::Accessor;

		struct Accessor;

		template<typename T>
		inline Ret ImplVisit(VarPtr<T>* p, Args... args) const;
	};
}

#include "detail/VarPtrVisitor.inl"
