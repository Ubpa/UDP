#pragma once

#include "../../Basic/xSTL/xStr.h"

namespace Ubpa::detail::Reflection_ {
	template<typename ArgList> struct Pack;
}

namespace Ubpa {
	inline ReflectionMngr& ReflectionMngr::Instance() {
		static ReflectionMngr instance;
		return instance;
	}

	template<typename... Args>
	void* ReflectionMngr::Create(std::string_view name, Args... args) const {
		auto target = constructors.find(name);
		if (target == constructors.end())
			return nullptr;

		if constexpr (sizeof...(Args) == 0) {
			return target->second(nullptr);
		}
		else {
			std::tuple<Args...> argTuple{ std::forward<Args>(args)... };
			return target->second(&argTuple);
		}
	}

	ReflectionBase* ReflectionMngr::GetReflction(const void* obj) const {
		auto target = vt2refl.find(vtable(obj));
		if (target == vt2refl.end())
			return nullptr;

		return target->second;
	}

	template<typename Obj>
	void ReflectionMngr::RegistRefl(ReflectionBase* refl) {
		if constexpr (std::is_polymorphic_v<Obj>)
			vt2refl[vtable_of<Obj>::get()] = refl;
	}

	template<typename Func>
	void ReflectionMngr::RegistConstructor(std::string_view name, Func&& func) {
		constructors[str(name)] = detail::Reflection_::Pack<FuncTraits_ArgList<Func>>::template run(func);
	}
}

namespace Ubpa::detail::Reflection_ {
	template<typename... Args>
	struct Pack<TypeList<Args...>> {
	private:
		template<typename Ret, typename Func, typename ArgTuple, size_t... Ns>
		static Ret unpack(const Func& func, const ArgTuple& argTuple, std::index_sequence<Ns...>) noexcept {
			return func(std::get<Ns>(argTuple)...);
		}

	public:
		template<typename Func>
		static auto run(Func&& func) noexcept {
			return[func = std::forward<Func>(func)](void* ptr) ->void* {
				const auto& argTuple = *reinterpret_cast<std::tuple<Args...>*>(ptr);
				auto obj = unpack<FuncTraits_Ret<Func>>(func,
					argTuple, std::make_index_sequence<sizeof...(Args)>{});
				return reinterpret_cast<void*>(obj);
			};
		}
	};

	template<>
	struct Pack<TypeList<>> {
		template<typename Func>
		static auto run(Func&& func) {
			return[func = std::forward<Func>(func)](void*) ->void* {
				return func();
			};
		}
	};
}