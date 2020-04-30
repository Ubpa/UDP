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
	struct Register;
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
		Reflection& RegisterConstructor();
		template<typename Func>
		Reflection& RegisterConstructor(Func&& func);

		// member variable pointer, member function pointer
		template<typename T>
		Reflection& Register(T Obj::* ptr, std::string_view name) noexcept;
		Reflection& Register(std::string_view key, std::string_view value) noexcept;
		Reflection& Register(std::string_view field, std::string_view kind, std::string_view value) noexcept;

		virtual const std::string Meta(std::string_view key) const noexcept override;
		virtual const xMap<std::string, std::string>& Metas() const noexcept override { return metamap; }
		virtual const std::string FieldMeta(std::string_view field, std::string_view kind) const noexcept override;

		template<typename U>
		MemVar<U Obj::*> Var(std::string_view name) const noexcept;

		xMap<std::string, MemVarBase<Obj>*> Vars() const noexcept;

		xMap<std::string, std::shared_ptr<VarPtrBase>> VarPtrs(Obj& obj) const noexcept;
		xMap<std::string, std::shared_ptr<const VarPtrBase>> VarPtrs(const Obj& obj) const noexcept;

		template<typename Ret = void, typename RObj, typename... Args>
		Ret Call(std::string_view name, RObj&& obj, Args&&... args);

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
			ReflTraitsIniter::Instance().Register<Obj>();
			ReflectionMngr::Instance().RegisterRefl<Obj>(this);
			if constexpr (std::is_constructible_v<Obj>)
				RegisterConstructor();
		}

		template<typename Mem>
		friend struct detail::Reflection_::Register;
		template<typename Obj, typename RObj, typename Ret, typename... Args>
		friend struct detail::Reflection_::Call;
	};
}

#include "detail/Reflection.inl"
