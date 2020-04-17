#pragma once

#include "VarPtr.h"

#include <memory>

namespace Ubpa {
	template<typename Obj_T>
	class MemVar;

	template<typename Obj>
	class MemVarBase {
	public:
		MemVarBase(void* Obj::* var = nullptr) : var{ var } {}
		virtual ~MemVarBase() = default;

		template<typename U>
		const MemVar<U Obj::*> As() const noexcept {
			return reinterpret_cast<U Obj::*>(var);
		}

		virtual std::shared_ptr<VarPtrBase> PtrOf(Obj& obj) = 0;
		virtual std::shared_ptr<VarPtrBase> PtrOf(Obj* obj) = 0;
		virtual std::shared_ptr<const VarPtrBase> PtrOf(const Obj& obj) = 0;
		virtual std::shared_ptr<const VarPtrBase> PtrOf(const Obj* obj) = 0;

	protected:
		void* Obj::* var;
	};

	template<typename Obj, typename T>
	class MemVar<T Obj::*> : public MemVarBase<Obj> {
	private:
		template<typename U> struct RemoveConst { using type = U; };
		template<typename U> struct RemoveConst<const U> { using type = U; };
		template<typename U> struct RemoveConst<const U*> { using type = U*; };
		template<typename U> struct RemoveConst<const U* const> { using type = U*; };
		template<typename U> using RemoveConst_t = typename RemoveConst<U>::type;
		template<typename U> struct IsConst : std::false_type {};
		template<typename U> struct IsConst<const U> : std::true_type {};
		template<typename U> struct IsConst<const U*> : std::true_type {};
		template<typename U> struct IsConst<const U* const> : std::true_type {};
		template<typename U> static constexpr bool IsConst_v = IsConst<U>::value;
	public:
		MemVar(T Obj::* var = nullptr)
			: MemVarBase<Obj>{ reinterpret_cast<void* Obj::*>(const_cast<RemoveConst_t<T> Obj::*>(var)) } {}

		T& Of(Obj& obj) const noexcept {
			return obj.*get();
		}
		T& Of(Obj* obj) const noexcept {
			return obj->*get();
		}
		const T& Of(const Obj& obj) const noexcept {
			return obj.*get();
		}
		const T& Of(const Obj* obj) const noexcept {
			return obj->*get();
		}

		virtual std::shared_ptr<VarPtrBase> PtrOf(Obj& obj) override {
			return std::make_shared<VarPtr<T>>(&Of(obj));
		}

		virtual std::shared_ptr<VarPtrBase> PtrOf(Obj* obj) override {
			return std::make_shared<VarPtr<T>>(&Of(obj));
		}

		virtual std::shared_ptr<const VarPtrBase> PtrOf(const Obj& obj) override {
			return std::make_shared<const VarPtr<T>>(const_cast<T*>(&Of(obj)));
		}

		virtual std::shared_ptr<const VarPtrBase> PtrOf(const Obj* obj) override {
			return std::make_shared<const VarPtr<T>>(const_cast<T*>(&Of(obj)));
		}

	private:
		T Obj::* get() const noexcept {
			if constexpr (IsConst_v<T>)
				return reinterpret_cast<T Obj::*>(const_cast<const void* Obj::*>(MemVarBase<Obj>::var));
			else
				return reinterpret_cast<T Obj::*>(MemVarBase<Obj>::var);
		}
		using MemVarBase<Obj>::As;
	};
}
