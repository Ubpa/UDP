#pragma once

#include <cassert>

#ifndef NDEBUG
#include <iostream>
#endif // !NDEBUG

#include "../../Basic/Read.h"

namespace Ubpa {
	template<typename Obj>
	Reflection<Obj>& Reflection<Obj>::Instance() noexcept {
		static Reflection instance;
		return instance;
	}

	template<typename Obj>
	Reflection<Obj>& Reflection<Obj>::RegisterConstructor() {
		ReflectionMngr::Instance().RegisterConstructor(Name(), []() -> void* {
			return reinterpret_cast<void*>(new Obj);
		});
		return *this;
	}

	template<typename Obj>
	template<typename Func>
	Reflection<Obj>& Reflection<Obj>::RegisterConstructor(Func&& func) {
		ReflectionMngr::Instance().RegisterConstructor(Name(), std::forward<Func>(func));
		return *this;
	}

	template<typename Obj>
	template<typename T>
	Reflection<Obj>& Reflection<Obj>::Register(T Obj::* ptr, std::string_view name) noexcept {
		detail::Reflection_::Register<T Obj::*>::run(*this, ptr, name);
		return *this;
	}

	template<typename Obj>
	Reflection<Obj>& Reflection<Obj>::Register(std::string_view key, std::string_view value) noexcept {
		metamap.emplace(key, value);
		return *this;
	}

	template<typename Obj>
	Reflection<Obj>& Reflection<Obj>::Register(std::string_view field, std::string_view kind, std::string_view value) noexcept {
		metamap.try_emplace(std::string(field) + "::" + kind.data(), value);
		return *this;
	}

	template<typename Obj>
	const std::string Reflection<Obj>::Meta(std::string_view key) const noexcept {
		auto target = metamap.find(key);
		if (target == metamap.end())
			return "";
		else
			return target->second;
	}

	template<typename Obj>
	const std::string Reflection<Obj>::FieldMeta(std::string_view field, std::string_view kind) const noexcept {
		auto target = metamap.find(std::string(field) + "::" + kind.data());
		if (target == metamap.end())
			return "";
		else
			return target->second;
	}

	template<typename Obj>
	template<typename U>
	MemVar<U Obj::*> Reflection<Obj>::Var(std::string_view name) const noexcept {
		auto target = n2mv.find(name);
		if (target != n2mv.end())
			return target->second->As<U>();
		else {
#ifndef NDEBUG
			std::cerr << "WARNING::Reflection::Var:" << std::endl
				<< "\t" << name << " is unregistered" << std::endl;
#endif // !NDEBUG
			return static_cast<U Obj::*>(nullptr);
		}
	}

	template<typename Obj>
	xMap<std::string, MemVarBase<Obj>*> Reflection<Obj>::Vars() const noexcept {
		return n2mv;
	}

	template<typename Obj>
	xMap<std::string, std::shared_ptr<VarPtrBase>> Reflection<Obj>::VarPtrs(Obj& obj) const noexcept {
		xMap<std::string, std::shared_ptr<VarPtrBase>> rst;
		for (const auto& [n, mv] : n2mv)
			rst[n] = mv->PtrOf(obj);
		return rst;
	}

	template<typename Obj>
	xMap<std::string, std::shared_ptr<const VarPtrBase>> Reflection<Obj>::VarPtrs(const Obj& obj) const noexcept {
		xMap<std::string, std::shared_ptr<const VarPtrBase>> rst;
		for (const auto& [n, mv] : n2mv)
			rst[n] = mv->PtrOf(obj);
		return rst;
	}

	template<typename Obj>
	xMap<std::string, std::shared_ptr<VarPtrBase>> Reflection<Obj>::VarPtrs(void* obj) const {
		return VarPtrs(*reinterpret_cast<Obj*>(obj));
	}

	template<typename Obj>
	xMap<std::string, std::shared_ptr<const VarPtrBase>> Reflection<Obj>::VarPtrs(const void* obj) const {
		return VarPtrs(*reinterpret_cast<const Obj*>(obj));
	}

	template<typename Obj>
	template<typename Ret, typename RObj, typename... Args>
	Ret Reflection<Obj>::Call(std::string_view name, RObj&& obj, Args&&... args) {
		return detail::Reflection_::Call<Obj, RObj, Ret, Args...>::run(*this, name, std::forward<RObj>(obj), std::forward<Args>(args)...);
	}
}

namespace Ubpa::detail::Reflection_ {
	template<typename Obj, typename Ret, typename... Args>
	struct Register<Ret(Obj::*)(Args...)> {
		using Func = Ret(Args...);
		static void run(Reflection<Obj>& refl, Func Obj::* ptr, std::string_view name) {
#ifndef NDEBUG
			if (refl.n2mf.find(name) != refl.n2mf.end()) {
				std::cerr << "WARNING::Reflection::Register:" << std::endl
					<< "\t" << name << " is already registed" << std::endl;
			}
#endif // !NDEBUG
			refl.n2mf.emplace(name, new MemFunc<Func Obj::*>{ ptr });
		}
	};

	template<typename Obj, typename Ret, typename... Args>
	struct Register<Ret(Obj::*)(Args...) const> {
		using Func = Ret(Args...) const;
		static void run(Reflection<Obj>& refl, Func Obj::* ptr, std::string_view name) {
#ifndef NDEBUG
			if (refl.n2mcf.find(name) != refl.n2mcf.end()) {
				std::cerr << "WARNING::Reflection::Register:" << std::endl
					<< "\t" << name << " is already registed" << std::endl;
			}
#endif // !NDEBUG
			refl.n2mcf.emplace(name, new MemFunc<Func Obj::*>{ ptr });
		}
	};

	template<typename Obj, typename T>
	struct Register<T Obj::*> {
		static void run(Reflection<Obj>& refl, T Obj::* ptr, std::string_view name) {
#ifndef NDEBUG
			if (refl.n2mv.find(name) != refl.n2mv.end()) {
				std::cerr << "WARNING::Reflection::Register:" << std::endl
					<< "\t" << name << " is already registed" << std::endl;
			}
#endif // !NDEBUG
			if constexpr (std::is_enum_v<T>) {
				// enum is treated as int
				refl.n2mv.emplace(name, new MemVar<int Obj::*>{ reinterpret_cast<int Obj::*>(ptr) });
			}
			else
				refl.n2mv.emplace(name, new MemVar<T Obj::*>{ ptr });
		}
	};

	template<typename Obj, typename T>
	struct Register<Read<Obj, T> Obj::*> {
		static void run(Reflection<Obj>& refl, Read<Obj, T> Obj::* ptr, std::string_view name) {
#ifndef NDEBUG
			if (refl.n2mv.find(name) != refl.n2mv.end()) {
				std::cerr << "WARNING::Reflection::Register:" << std::endl
					<< "\t" << name << " is already registed" << std::endl;
			}
#endif // !NDEBUG
			refl.n2mv.emplace(name, new MemVar<T Obj::*>{ reinterpret_cast<T Obj::*>(ptr) });
			refl.Register(name, ReflectionBase::Meta::read_only, ReflectionBase::Meta::default_value);
		}
	};

	// default case
	template<typename Obj, typename PtrObj, typename Ret, typename... Args>
	struct Call {
		static Ret run(Reflection<Obj>& refl, std::string_view name, PtrObj&& obj, Args&&... args) {
			auto target_mf = refl.n2mf.find(name);
			if (target_mf != refl.n2mf.end())
				return target_mf->second->template Call<Ret>(*obj, std::forward<Args>(args)...);
			auto target_mfc = refl.n2mcf.find(name);
			if (target_mfc != refl.n2mcf.end())
				return target_mfc->second->template Call<Ret>(*obj, std::forward<Args>(args)...);
#ifndef NDEBUG
			std::cerr << "WARNING::Reflection::Call:" << std::endl
				<< "\t" << "not found " << name << std::endl;
#endif // !NDEBUG
			if constexpr (std::is_constructible_v<Ret>)
				return Ret{};
		}
	};

	template<typename Obj, typename Ret, typename... Args>
	struct Call<Obj, Obj, Ret, Args...> {
		static Ret run(Reflection<Obj>& refl, std::string_view name, Obj obj, Args&&... args) {
			auto target_mf = refl.n2mf.find(name);
			if (target_mf != refl.n2mf.end())
				return target_mf->second->template Call<Ret>(obj, std::forward<Args>(args)...);
			auto target_mfc = refl.n2mcf.find(name);
			if (target_mfc != refl.n2mcf.end())
				return target_mfc->second->template Call<Ret>(obj, std::forward<Args>(args)...);
#ifndef NDEBUG
			std::cerr << "WARNING::Reflection::Call:" << std::endl
				<< "\t" << "not found " << name << std::endl;
#endif // !NDEBUG
			if constexpr (std::is_constructible_v<Ret>)
				return Ret{};
		}
	};

	template<typename Obj, typename Ret, typename... Args>
	struct Call<Obj, Obj&&, Ret, Args...> {
		static Ret run(Reflection<Obj>& refl, std::string_view name, Obj&& obj, Args&&... args) {
			auto target_mf = refl.n2mf.find(name);
			if (target_mf != refl.n2mf.end())
				return target_mf->second->template Call<Ret>(obj, std::forward<Args>(args)...);
			auto target_mfc = refl.n2mcf.find(name);
			if (target_mfc != refl.n2mcf.end())
				return target_mfc->second->template Call<Ret>(obj, std::forward<Args>(args)...);
#ifndef NDEBUG
			std::cerr << "WARNING::Reflection::Call:" << std::endl
				<< "\t" << "not found " << name << std::endl;
#endif // !NDEBUG
			if constexpr (std::is_constructible_v<Ret>)
				return Ret{};
		}
	};

	template<typename Obj, typename Ret, typename... Args>
	struct Call<Obj, Obj&, Ret, Args...> {
		static Ret run(Reflection<Obj>& refl, std::string_view name, Obj& obj, Args&&... args) {
			auto target_mf = refl.n2mf.find(name);
			if (target_mf != refl.n2mf.end())
				return target_mf->second->template Call<Ret>(obj, std::forward<Args>(args)...);
			auto target_mfc = refl.n2mcf.find(name);
			if(target_mfc != refl.n2mcf.end())
				return target_mfc->second->template Call<Ret>(obj, std::forward<Args>(args)...);
#ifndef NDEBUG
			std::cerr << "WARNING::Reflection::Call:" << std::endl
				<< "\t" << "not found " << name << std::endl;
#endif // !NDEBUG
			if constexpr (std::is_constructible_v<Ret>)
				return Ret{};
		}
	};

	template<typename Obj, typename Ret, typename... Args>
	struct Call<Obj, const Obj&, Ret, Args...> {
		static Ret run(Reflection<Obj>& refl, std::string_view name, const Obj& obj, Args&&... args) {
			auto target_mfc = refl.n2mcf.find(name);
			if (target_mfc != refl.n2mcf.end())
				return target_mfc->second->template Call<Ret>(obj, std::forward<Args>(args)...);
#ifndef NDEBUG
			std::cerr << "WARNING::Reflection::Call:" << std::endl
				<< "\t" << "not found " << name << std::endl;
#endif // !NDEBUG
			if constexpr (std::is_constructible_v<Ret>)
				return Ret{};
		}
	};
}
