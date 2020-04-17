#pragma once

#include <unordered_map>
#include <functional>

namespace Ubpa {
	// [single inheritance] [polymorphic] infinite class visitor
	// ------
	// you can direct use it (Impl=void) and call Regist(lambda)
	// or inherit from it, define member functions
	// - name: 'ImplVisit'
	// - argument
	//   - T*: use Regist<T>
	//   - const T*: use RegistC<T>
	template<typename Impl = void> // Impl is non-final class
	class InfVisitor {
	public:
		// check polymorphic and call Visit(void*)
		template<typename T>
		void Visit(T* ptr);

		// ImplVisit(T*)
		// fallback: ImplVisit(const T*)
		void Visit(void* ptr);

		// check polymorphic and call Visit(const void*)
		template<typename T>
		void Visit(const T* ptr);

		// ImplVisit(const T*)
		void Visit(const void* ptr);

		// auto detect type
		// support const and non-const
		// argument't type must be [const] T* and T is polymophic
		template<typename... Funcs>
		void Regist(Funcs&&... funcs);

		// is registed ImplVisit(T*) or ImplVisit(const T*)
		template<typename T>
		bool IsRegisted() const;

		// ptr is pointed to a [single inheritance] [polymorphic] obj
		// check wether ImplVisit(T*) or ImplVisit(const T*) is registed
		// T is get by ptr's vtable
		bool IsRegisted(void* ptr) const;

		// ptr is pointed to a [single inheritance] [polymorphic] obj
		// check wether ImplVisit(const T*) is registed
		// T is get by ptr's vtable
		bool IsRegisted(const void* ptr) const;

	protected:
		// regist subclass's ImplVisit(const Deriveds*) ...
		// ImplVisit should be **publuc/protected(recommend)**
		// if use want to use private, you should use 'friend struct InfVisitor<Impl>::Accessor'
		template<typename... Deriveds>
		void Regist();

		// regist subclass's ImplVisit(const Deriveds*) ...
		// ImplVisit must be **publuc/protected(recommend)**
		// if use want to use private, you should use 'friend struct InfVisitor<Impl>::Accessor'
		template<typename... Deriveds>
		void RegistC();

	private:
		template<typename Derived>
		void RegistOne();
		template<typename Derived>
		void RegistOneC();
		template<typename Func>
		void RegistOne(Func&& func);

	protected:
		// used to access protected function 'ImplVisit'
		// if use want to use private, you should use 'friend struct InfVisitor<Impl>::Accessor'
		struct Accessor;

	private:
		std::unordered_map<const void*, std::function<void(void *)>> callbacks;
		std::unordered_map<const void*, std::function<void(const void*)>> const_callbacks;
	};
}

#include "detail/InfVisitor.inl"
