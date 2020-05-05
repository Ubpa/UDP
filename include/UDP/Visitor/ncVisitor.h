#pragma once

#include "Visitor.h"

namespace Ubpa {
	template<typename Func>
	class Visitor;

	template<typename Ret, typename... Args>
	class Visitor<Ret(void*, Args...)> {
	public:
		inline Ret Visit(void* ptr, Args... args) const;
		inline Ret Visit(size_t ID, void* ptr, Args... args) const;

		template<typename T>
		inline Ret Visit(T* ptr, Args... args) const;

		template<typename... Funcs>
		inline void Register(Funcs&&... funcs);

		template<typename Func>
		inline void Register(size_t ID, Func&& func);

		inline bool IsRegistered(size_t ID) const;
		template<typename T>
		inline bool IsRegistered() const;

		inline bool IsRegistered(const void* ptr) const;

	private:
		template<typename Func>
		inline void RegisterOne(Func&& func);

	protected:
		std::unordered_map<size_t, std::function<Ret(void*, Args...)>> callbacks; // ID -> func
	};
}

#include "detail/ncVisitor.inl"
