#pragma once

#include "ReflectionBase.h"
#include "../Basic/vtable.h"
#include <functional>

namespace Ubpa {
	template<typename Obj>
	struct Reflection;

	struct ReflectionMngr {
		static inline ReflectionMngr& Instance();

		// Args must match exactly, e.g. Create<std::string&&>()
		template<typename... Args>
		void* Create(std::string_view name, Args... args) const;

		inline bool IsRegistered(size_t ID) const;
		inline bool IsRegistered(const void* obj) const;
		inline ReflectionBase* GetReflction(size_t ID) const;
		inline ReflectionBase* GetReflction(const void* obj) const;

	private:
		template<typename Obj>
		friend struct Reflection;

		template<typename Obj>
		void RegisterRefl(ReflectionBase* refl);

		template<typename Func>
		void RegisterConstructor(std::string_view name, Func&& func);

	private:
		std::map<size_t, ReflectionBase*> id2refl;
		xMap<std::string, std::function<void*(void*)>> constructors;

		ReflectionMngr() = default;
	};
}

#include "detail/ReflectionMngr.inl"
