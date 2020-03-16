#pragma once

#include "../vtable.h"

#include <utility>

namespace Ubpa {
	template<typename Func_Obj> class MemFunc;

	template<typename Obj>
	class MemFuncBase {
	public:
		using Func = void();
		MemFuncBase(Func Obj::* func = nullptr) : func(func) {}
		virtual ~MemFuncBase() = default;

		template<typename Ret, typename... Args>
		Ret Call(Obj & obj, Args &&... args) const noexcept {
			assert(vtable_is<MemFunc<Ret(Obj::*)(Args...)>>(this));
			return (obj.*reinterpret_cast<Ret(Obj::*)(Args...)>(func))(std::forward<Args>(args)...);
		}

		template<typename PtrObj, typename Ret, typename... Args>
		Ret Call(const PtrObj& ptrObj, Args &&... args) const noexcept {
			return Call<Ret>(*ptrObj, std::forward<Args>(args)...);
		}

	protected:
		Func Obj::* func;
	};

	template<typename Obj>
	class MemCFuncBase {
	public:
		using CFunc = void() const;
		MemCFuncBase(CFunc Obj::* cfunc = nullptr) : cfunc(cfunc) {}
		virtual ~MemCFuncBase() = default;

		template<typename Ret, typename... Args>
		Ret Call(const Obj& obj, Args&&... args) const noexcept {
			assert(vtable_is<MemFunc<Ret(Obj::*)(Args...) const>>(this));
			return (obj.*reinterpret_cast<Ret(Obj::*)(Args...) const>(cfunc))(std::forward<Args>(args)...);
		}

		template<typename PtrObj, typename Ret, typename... Args>
		Ret Call(const PtrObj& ptrObj, Args&&... args) const noexcept {
			return Call<Ret>(*ptrObj, std::forward<Args>(args)...);
		}

	protected:
		CFunc Obj::* cfunc;
	};

	template<typename Obj, typename Ret, typename... Args>
	class MemFunc<Ret(Obj::*)(Args...)> : public MemFuncBase<Obj> {
	public:
		MemFunc(Ret(Obj::*func)(Args...) = nullptr)
			: MemFuncBase<Obj>(reinterpret_cast<void(Obj::*)()>(func)) {}

		Ret Call(Obj& obj, Args&&... args) const noexcept {
			return MemFuncBase<Obj>::template Call<Ret, Args...>(obj, std::forward<Args>(args)...);
		}

	private:
		using MemFuncBase<Obj>::Call;
	};

	template<typename Obj, typename Ret, typename... Args>
	class MemFunc<Ret(Obj::*)(Args...) const> : public MemCFuncBase<Obj> {
	public:
		MemFunc(Ret(Obj::*cfunc)(Args...) const = nullptr)
			: MemCFuncBase<Obj>(reinterpret_cast<void(Obj::*)()const>(cfunc)) {}

		Ret Call(const Obj& obj, Args&&... args) const noexcept {
			return MemCFuncBase<Obj>::template Call<Ret, Args...>(obj, std::forward<Args>(args)...);
		}

	private:
		using MemCFuncBase<Obj>::Call;
	};
}
