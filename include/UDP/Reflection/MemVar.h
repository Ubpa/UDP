#pragma once

namespace Ubpa {
	template<typename Obj_T>
	class MemVar;

	template<typename Obj>
	class MemVarBase {
	public:
		MemVarBase(void* Obj::* var = nullptr) : var{ var } {}
		virtual ~MemVarBase() = default;

		template<typename U>
		const MemVar<U Obj::*> As() const noexcept {
			return reinterpret_cast<U Obj::*>(var);
		}

	protected:
		void* Obj::* var;
	};

	template<typename Obj, typename T>
	class MemVar<T Obj::*> : public MemVarBase<Obj> {
	public:
		MemVar(T Obj::* var = nullptr)
			: MemVarBase<Obj>(reinterpret_cast<void* Obj::*>(var)) {}

		T& Of(Obj& obj) const noexcept {
			return obj.*get();
		}
		T& Of(Obj* obj) const noexcept {
			return obj->*get();
		}
		const T& Of(const Obj& obj) const noexcept {
			return obj.*get();
		}
		const T& Of(const Obj* obj) const noexcept {
			return obj->*get();
		}

	private:
		T Obj::* get() const noexcept {
			return reinterpret_cast<T Obj::*>(MemVarBase<Obj>::var);
		}
		using MemVarBase<Obj>::As;
	};
}