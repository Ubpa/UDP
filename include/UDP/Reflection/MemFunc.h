#pragma once

#include <utility>

namespace Ubpa {
	template<typename Func_Obj>
	class MemFunc;

	template<typename Obj>
	class MemFunc<void(Obj::*)(void*)> {
	public:
		using Func = void(void*);
		MemFunc(Func Obj::* ptr = nullptr) : ptr(ptr) {}
		virtual ~MemFunc() = default;

		template<typename Ret, typename... Args>
		Ret Call(Obj& obj, Args&&... args) const noexcept {
			return (obj.*reinterpret_cast<Ret(Obj::*)(Args...)>(ptr))(std::forward<Args>(args)...);
		}
		template<typename PtrObj, typename Ret, typename... Args>
		Ret Call(PtrObj&& ptrObj, Args&&... args) const noexcept {
			return Call<Ret>(*ptrObj, std::forward<Args>(args)...);
		}
	protected:
		Func Obj::* ptr;
	};

	template<typename Obj>
	class MemFunc<void(Obj::*)(void*) const> {
	public:
		using Func = void(void*) const;
		MemFunc(Func Obj::* ptr = nullptr) : ptr(ptr) {}
		virtual ~MemFunc() = default;

		template<typename Ret, typename... Args>
		Ret Call(const Obj& obj, Args&&... args) const noexcept {
			return (obj.*reinterpret_cast<Ret(Obj::*)(Args...) const>(ptr))(std::forward<Args>(args)...);
		}

		template<typename PtrObj, typename Ret, typename... Args>
		Ret Call(PtrObj&& ptrObj, Args&&... args) const noexcept {
			return Call<Ret>(*ptrObj, std::forward<Args>(args)...);
		}
	protected:
		Func Obj::* ptr;
	};

	template<typename Obj, typename Ret, typename... Args>
	class MemFunc<Ret(Obj::*)(Args...)> : public MemFunc<void(Obj::*)(void*)> {
	public:
		using Base = MemFunc<void(Obj::*)(void*)>;
		template<typename RetU, typename... ArgsU>
		MemFunc(RetU(Obj::* ptr)(ArgsU...)) : Base(reinterpret_cast<void(Obj::*)(void*)>(ptr)) {}

		Ret Call(Obj& obj, Args&&... args) const noexcept {
			return Base::template Call<Ret, Args...>(obj, std::forward<Args>(args)...);
		}
	private:
		using Base::Call;
	};

	template<typename Obj, typename Ret, typename... Args>
	class MemFunc<Ret(Obj::*)(Args...) const> : public MemFunc<void(Obj::*)(void*) const> {
	public:
		using Base = MemFunc<void(Obj::*)(void*) const>;
		template<typename RetU, typename... ArgsU>
		MemFunc(RetU(Obj::* ptr)(ArgsU...) const) : Base(reinterpret_cast<void(Obj::*)(void*) const>(ptr)) {}

		Ret Call(const Obj& obj, Args&&... args) const noexcept {
			return Base::template Call<Ret, Args...>(obj, std::forward<Args>(args)...);
		}
	private:
		using Base::Call;
	};
}
