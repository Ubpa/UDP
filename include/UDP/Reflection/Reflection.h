#pragma once

#include "MemVar.h"
#include "MemFunc.h"
#include "ReflTraits.h"
#include "ReflectionMngr.h"

#include <_deps/nameof.hpp>

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
	struct Reflection final : ReflectionBase {
		static Reflection& Instance() noexcept;

		static void Init() noexcept { Instance(); }

		constexpr std::string_view Name() noexcept { return nameof::nameof_type<Obj>(); }

		// call after SetName()
		Reflection& RegistConstructor();
		template<typename Func>
		Reflection& RegistConstructor(Func&& func);

		// member variable pointer, member function pointer
		template<typename T>
		Reflection& Regist(T Obj::* ptr, const std::string& name) noexcept;
		Reflection& Regist(const std::string& key, const std::string& value) noexcept;
		Reflection& Regist(const std::string& field, const std::string& kind, const std::string& value) noexcept;

		virtual const std::string Meta(const std::string& key) const noexcept override;
		virtual const xMap<std::string, std::string> Metas() const noexcept override { return metamap; }

		template<typename U>
		MemVar<U Obj::*> Var(const std::string& name) const noexcept;

		xMap<std::string, MemVarBase<Obj>*> Vars() const noexcept;

		xMap<std::string, std::shared_ptr<VarPtrBase>> VarPtrs(Obj& obj) const noexcept;
		xMap<std::string, std::shared_ptr<const VarPtrBase>> VarPtrs(const Obj& obj) const noexcept;

		template<typename Ret = void, typename RObj, typename... Args>
		Ret Call(const std::string& name, RObj&& obj, Args&&... args);

		xMap<std::string, MemFuncBase<Obj>*> Funcs() const noexcept { return n2mf; }

		xMap<std::string, MemCFuncBase<Obj>*> CFuncs() const noexcept { return n2mcf; }

	private:
		virtual xMap<std::string, std::shared_ptr<VarPtrBase>> VarPtrs(void* obj) const override;
		virtual xMap<std::string, std::shared_ptr<const VarPtrBase>> VarPtrs(const void* obj) const override;

	private:
		xMap<std::string, MemVarBase<Obj>*> n2mv;
		xMap<std::string, MemFuncBase<Obj>*> n2mf;
		xMap<std::string, MemCFuncBase<Obj>*> n2mcf;
		xMap<std::string, std::string> metamap;

		Reflection() {
			ReflTraitsIniter::Instance().Regist<Obj>();
			ReflectionMngr::Instance().RegistRefl<Obj>(this);
			if constexpr (std::is_constructible_v<Obj>)
				RegistConstructor();
		}

		template<typename Mem>
		friend struct detail::Reflection_::Regist;
		template<typename Obj, typename RObj, typename Ret, typename... Args>
		friend struct detail::Reflection_::Call;
	};
}

#include "detail/Reflection.inl"
