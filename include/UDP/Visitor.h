#pragma once

#include <unordered_map>
#include <functional>

namespace Ubpa {
	template<typename Func>
	class Visitor;

	// [single inheritance] [polymorphic] infinite visitor
	template<typename Ret, typename... Args>
	class Visitor<Ret(Args...)> {
	public:
		inline Ret Visit(void* ptr, Args... args) const;
		// const_cast<void*>(ptr)
		inline Ret Visit(const void* ptr, Args... args) const;

		// check polymorphic, reinterpret_cast<void*>
		template<typename T>
		inline Ret Visit(T* ptr, Args... args) const;
		// const_cast<T*>(ptr)
		template<typename T>
		inline Ret Visit(const T* ptr, Args... args) const;

		// auto detect type
		// support const and non-const
		// [const] T* (T is polymophic) with Args...
		template<typename... Funcs>
		inline void Register(Funcs&&... funcs);
		
		template<typename T>
		inline bool IsRegistered() const;

		// ptr is pointed to a [single inheritance] [polymorphic] obj
		// T is get by ptr's vtable
		inline bool IsRegistered(const void* ptr) const;

	private:
		template<typename Func>
		inline void RegisterOne(Func&& func);

	protected:
		std::unordered_map<const void*, std::function<Ret(void*, Args...)>> callbacks; // vtable -> func
	};

	template<typename Impl, typename Ret, typename... Args>
	class Visitor<Ret(Impl::*)(Args...)> : private Visitor<Ret(Args...)> {
	public:
		using Visitor<Ret(Args...)>::Register;
		
		inline Ret Visit(void* ptr, Args... args) const;
		// const_cast<void*>(ptr)
		inline Ret Visit(const void* ptr, Args... args) const;

		// check polymorphic, reinterpret_cast<void*>
		template<typename T>
		inline Ret Visit(T* ptr, Args... args) const;
		// const_cast<T*>(ptr)
		template<typename T>
		inline Ret Visit(const T* ptr, Args... args) const;

		template<typename T>
		inline bool IsRegistered() const;

		// ptr is pointed to a [single inheritance] [polymorphic] obj
		// T is get by ptr's vtable
		inline bool IsRegistered(const void* ptr) const;

	protected:
		// ImplVisit
		template<typename... Deriveds>
		void Register();

	private:
		// ImplVisit
		template<typename Derived>
		void RegisterOne();

	protected:
		struct Accessor;

	private:
		std::unordered_map<const void*, std::function<Ret(Impl*, void*, Args...)>> impl_callbacks;
	};
}

#include "detail/Visitor.inl"
