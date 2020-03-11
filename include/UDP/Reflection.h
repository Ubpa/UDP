#pragma once

#include <map>
#include <string>

namespace Ubpa::detail::Reflection_ {
	template<typename Obj>
	struct Reflection;
}

namespace Ubpa {
	template<typename Obj, typename T>
	struct VarPtr {
		VarPtr(T Obj::* ptr = nullptr) : ptr(ptr) {}

		T& Of(Obj& obj) const noexcept {
			return obj.*ptr;
		}

		template<typename U>
		const VarPtr<Obj, U> As() const noexcept {
			return reinterpret_cast<U Obj::*>(ptr);
		}

		T Obj::* ptr;
	};

	// singleton : detail::Reflection_::Reflection
	template<typename Obj>
	struct Reflection {
		template<typename T>
		static void Regist(T Obj::* ptr, const std::string& name) noexcept;

		template<typename U = void*>
		static const VarPtr<Obj, U> Var(const std::string& name) noexcept;

		static const std::map<std::string, VarPtr<Obj, void*>> Vars() noexcept;

	private:
		Reflection() = delete;
	};
}

#include "detail/Reflection.inl"
