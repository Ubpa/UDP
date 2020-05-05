#pragma once

#include "ReflectionBase.h"
#include "../Basic/vtable.h"

namespace Ubpa {
	template<typename Obj>
	struct Reflection;

	struct ReflectionMngr {
		static inline ReflectionMngr& Instance();

		// Args must match exactly
		template<typename... Args>
		void* Create(std::string_view name, Args... args) const;

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
		xMap<size_t, ReflectionBase*> id2refl;
		xMap<std::string, std::function<void* (void*)>> constructors;

		ReflectionMngr() = default;
	};
}

#include "detail/ReflectionMngr.inl"
