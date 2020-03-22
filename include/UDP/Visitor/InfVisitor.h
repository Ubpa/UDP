#pragma once

#include <unordered_map>
#include <functional>

namespace Ubpa {
	// [single inheritance] [polymorphic] class visitor
	template<typename Impl>
	class InfVisitor {
	public:
		template<typename T>
		void Visit(T* ptr);
		void Visit(void* ptr);

		template<typename T>
		void Visit(const T* ptr);
		void Visit(const void* ptr);

		template<typename... Funcs>
		void Regist(Funcs&&... funcs);

		template<typename T>
		bool IsRegisted();
		bool IsRegisted(void* ptr);
		bool IsRegisted(const void* ptr);

	protected:
		template<typename... Deriveds>
		void Regist();
		template<typename... Deriveds>
		void RegistC();

	private:
		template<typename Derived>
		void RegistOne();
		template<typename Derived>
		void RegistOneC();
		template<typename Func>
		void RegistOne(Func&& func);

	private:
		std::unordered_map<const void*, std::function<void(void *)>> callbacks;
		std::unordered_map<const void*, std::function<void(const void*)>> const_callbacks;
		struct Accessor;
	};
}

#include "detail/InfVisitor.inl"