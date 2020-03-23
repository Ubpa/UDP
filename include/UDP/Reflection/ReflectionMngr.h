#pragma once

#include "ReflectionBase.h"
#include "../Basic/vtable.h"

namespace Ubpa {
	template<typename Obj>
	struct Reflection;

	struct ReflectionMngr {
		static ReflectionMngr& Instance() {
			static ReflectionMngr instance;
			return instance;
		}

		void* Creat(const std::string& name) const {
			auto target = constructors.find(name);
			if (target == constructors.end())
				return nullptr;
			return target->second();
		}

		ReflectionBase* GetReflction(const void* obj) const {
			auto target = vt2refl.find(vtable(obj));
			if (target == vt2refl.end())
				return nullptr;

			return target->second;
		}

	private:
		template<typename Obj>
		friend struct Reflection;

		template<typename Obj>
		void Regist(ReflectionBase* refl) {
			if constexpr (std::is_polymorphic_v<Obj>)
				vt2refl[vtable_of<Obj>::get()] = refl;
		}

		template<typename Func>
		void RegistConstructor(const std::string& name, Func&& func) {
			constructors[name] = std::forward<Func>(func);
		}

	private:
		std::map<const void*, ReflectionBase*> vt2refl;
		std::map<std::string, std::function<void* (void)>> constructors;

		ReflectionMngr() = default;
	};
}