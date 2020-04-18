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

		inline ReflectionBase* GetReflction(const void* obj) const;

	private:
		template<typename Obj>
		friend struct Reflection;

		template<typename Obj>
		void RegistRefl(ReflectionBase* refl);

		template<typename Func>
		void RegistConstructor(std::string_view name, Func&& func);

	private:
		xMap<const void*, ReflectionBase*> vt2refl;
		xMap<std::string, std::function<void* (void*)>> constructors;

		ReflectionMngr() = default;
	};
}

#include "detail/ReflectionMngr.inl"
