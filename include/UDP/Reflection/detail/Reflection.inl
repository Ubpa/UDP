#pragma once

namespace Ubpa {
	template<typename Obj>
	Reflection<Obj>& Reflection<Obj>::Instance() noexcept {
		static Reflection instance;
		return instance;
	}

	template<typename Obj>
	template<typename T>
	Reflection<Obj>& Reflection<Obj>::Regist(T Obj::* ptr, const std::string& name) noexcept {
		detail::Reflection_::Regist<T Obj::*>::run(*this, ptr, name);
		return *this;
	}

	template<typename Obj>
	template<typename U>
	const MemVar<U Obj::*> Reflection<Obj>::Var(const std::string& name) {
		return (n2mv[name]).As<U>();
	}

	template<typename Obj>
	const std::map<std::string, MemVar<void* Obj::*>> Reflection<Obj>::Vars() const noexcept {
		return n2mv;
	}

	template<typename Obj>
	template<typename Ret, typename RObj, typename... Args>
	Ret Reflection<Obj>::Call(const std::string& name, RObj&& obj, Args&&... args) {
		return detail::Reflection_::Call<Obj, RObj, Ret, Args...>::run(*this, name, std::forward<RObj>(obj), std::forward<Args>(args)...);
	}
}

namespace Ubpa::detail::Reflection_ {
	template<typename Obj, typename Ret, typename... Args>
	struct Regist<Ret(Obj::*)(Args...)> {
		using Func = Ret(Args...);
		static void run(Reflection<Obj>& refl, Func Obj::* ptr, const std::string& name) {
			refl.n2mf[name] = MemFunc<Func Obj::*>{ ptr };
		}
	};

	template<typename Obj, typename Ret, typename... Args>
	struct Regist<Ret(Obj::*)(Args...) const> {
		using Func = Ret(Args...) const;
		static void run(Reflection<Obj>& refl, Func Obj::* ptr, const std::string& name) {
			refl.n2mfc[name] = MemFunc<Func Obj::*>{ ptr };
		}
	};

	template<typename Obj, typename T>
	struct Regist<T Obj::*> {
		static void run(Reflection<Obj>& refl, T Obj::* ptr, const std::string& name) {
			refl.n2mv[name] = MemVar<T Obj::*>{ ptr };
		}
	};

	// default case
	template<typename Obj, typename PtrObj, typename Ret, typename... Args>
	struct Call {
		static Ret run(Reflection<Obj>& refl, const std::string& name, PtrObj&& obj, Args&&... args) {
			auto target_mf = refl.n2mf.find(name);
			if (target_mf != refl.n2mf.end())
				return target_mf->second.template Call<Ret>(*obj, std::forward<Args>(args)...);
			auto target_mfc = refl.n2mfc.find(name);
			if (target_mfc != refl.n2mfc.end())
				return target_mfc->second.template Call<Ret>(*obj, std::forward<Args>(args)...);
			if constexpr (std::is_constructible_v<Ret>)
				return Ret{};
		}
	};

	template<typename Obj, typename Ret, typename... Args>
	struct Call<Obj, Obj, Ret, Args...> {
		static Ret run(Reflection<Obj>& refl, const std::string& name, Obj obj, Args&&... args) {
			auto target_mf = refl.n2mf.find(name);
			if (target_mf != refl.n2mf.end())
				return target_mf->second.template Call<Ret>(obj, std::forward<Args>(args)...);
			auto target_mfc = refl.n2mfc.find(name);
			if (target_mfc != refl.n2mfc.end())
				return target_mfc->second.template Call<Ret>(obj, std::forward<Args>(args)...);
			if constexpr (std::is_constructible_v<Ret>)
				return Ret{};
		}
	};

	template<typename Obj, typename Ret, typename... Args>
	struct Call<Obj, Obj&&, Ret, Args...> {
		static Ret run(Reflection<Obj>& refl, const std::string& name, Obj&& obj, Args&&... args) {
			auto target_mf = refl.n2mf.find(name);
			if (target_mf != refl.n2mf.end())
				return target_mf->second.template Call<Ret>(obj, std::forward<Args>(args)...);
			auto target_mfc = refl.n2mfc.find(name);
			if (target_mfc != refl.n2mfc.end())
				return target_mfc->second.template Call<Ret>(obj, std::forward<Args>(args)...);
			if constexpr(std::is_constructible_v<Ret>)
				return Ret{};
		}
	};

	template<typename Obj, typename Ret, typename... Args>
	struct Call<Obj, Obj&, Ret, Args...> {
		static Ret run(Reflection<Obj>& refl, const std::string& name, Obj& obj, Args&&... args) {
			auto target_mf = refl.n2mf.find(name);
			if (target_mf != refl.n2mf.end())
				return target_mf->second.template Call<Ret>(obj, std::forward<Args>(args)...);
			auto target_mfc = refl.n2mfc.find(name);
			if(target_mfc != refl.n2mfc.end())
				return target_mfc->second.template Call<Ret>(obj, std::forward<Args>(args)...);
			if constexpr (std::is_constructible_v<Ret>)
				return Ret{};
		}
	};

	template<typename Obj, typename Ret, typename... Args>
	struct Call<Obj, const Obj&, Ret, Args...> {
		static Ret run(Reflection<Obj>& refl, const std::string& name, const Obj& obj, Args&&... args) {
			auto target_mfc = refl.n2mfc.find(name);
			if (target_mfc != refl.n2mfc.end())
				return target_mfc->second.template Call<Ret>(obj, std::forward<Args>(args)...);
			if constexpr (std::is_constructible_v<Ret>)
				return Ret{};
		}
	};
}
