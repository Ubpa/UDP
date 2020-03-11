#pragma once

#include <map>

namespace Ubpa::detail::Reflection_ {
	template<typename Obj>
	struct Reflection;
}

namespace Ubpa {
	template<typename Obj>
	template<typename T>
	void Reflection<Obj>::Regist(T Obj::* ptr, const std::string& name) noexcept {
		detail::Reflection_::Reflection<Obj>::Instance().Regist(ptr, name);
	}

	template<typename Obj>
	template<typename U>
	const VarPtr<Obj, U> Reflection<Obj>::Var(const std::string& name) noexcept {
		return detail::Reflection_::Reflection<Obj>::Instance().Var<U>(name);
	}

	template<typename Obj>
	const std::map<std::string, VarPtr<Obj, void*>> Reflection<Obj>::Vars() noexcept {
		return detail::Reflection_::Reflection<Obj>::Instance().Vars();
	}
}

namespace Ubpa::detail::Reflection_ {
	template<typename Obj>
	struct Reflection final {
		static Reflection& Instance() noexcept {
			static Reflection instance;
			return instance;
		}

		template<typename T>
		void Regist(T Obj::* ptr, const std::string& name) {
			n2p[name] = VarPtr<Obj, void*>{ reinterpret_cast<void* Obj::*>(ptr) };
		}

		template<typename U>
		const VarPtr<Obj, U> Var(const std::string& name) {
			return (n2p[name]).As<U>();
		}

		const std::map<std::string, VarPtr<Obj, void*>> Vars() const noexcept {
			return n2p;
		}

	private:
		std::map<std::string, VarPtr<Obj, void*>> n2p;
		Reflection() = default;
	};
}