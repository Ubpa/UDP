#pragma once

#include "MemVar.h"
#include "MemFunc.h"

#include <map>
#include <string>

namespace Ubpa::detail::Reflection_ {
	template<typename Mem>
	struct Regist;
	template<typename Obj, typename RObj, typename Ret, typename... Args>
	struct Call;
}

namespace Ubpa {
	template<typename Obj>
	struct Reflection final {
		inline static Reflection& Instance() noexcept;

		Reflection& SetName(const std::string& name) noexcept { this->name = name; return *this; }
		const std::string& GetName() noexcept { return name; }

		template<typename T>
		inline Reflection& Regist(T Obj::* ptr, const std::string& name) noexcept;

		template<typename U>
		inline const MemVar<U Obj::*> Var(const std::string& name) const noexcept;

		inline const std::map<std::string, MemVarBase<Obj>*> Vars() const noexcept;

		template<typename Ret = void, typename RObj, typename... Args>
		inline Ret Call(const std::string& name, RObj&& obj, Args&&... args);

		inline std::map<std::string, MemFuncBase<Obj>*> Funcs() const noexcept { return n2mf; }

		inline std::map<std::string, MemCFuncBase<Obj>*> CFuncs() const noexcept { return n2mcf; }

	private:
		std::map<std::string, MemVarBase<Obj>*> n2mv;
		std::map<std::string, MemFuncBase<Obj>*> n2mf;
		std::map<std::string, MemCFuncBase<Obj>*> n2mcf;
		std::string name;
		Reflection() = default;

		template<typename Mem>
		friend struct detail::Reflection_::Regist;
		template<typename Obj, typename RObj, typename Ret, typename... Args>
		friend struct detail::Reflection_::Call;
	};
}

#include "detail/Reflection.inl"
