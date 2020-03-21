#pragma once

#include "VarPtr.h"

#include <memory>

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

		virtual std::shared_ptr<VarPtrBase> PtrOf(Obj& obj) = 0;
		virtual std::shared_ptr<VarPtrBase> PtrOf(Obj* obj) = 0;
		virtual std::shared_ptr<VarPtrBase> PtrOf(const Obj& obj) = 0;
		virtual std::shared_ptr<VarPtrBase> PtrOf(const Obj* obj) = 0;

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

		virtual std::shared_ptr<VarPtrBase> PtrOf(Obj& obj) override {
			return std::make_shared<VarPtr<T>>(&Of(obj));
		}

		virtual std::shared_ptr<VarPtrBase> PtrOf(Obj* obj) override {
			return std::make_shared<VarPtr<T>>(&Of(obj));
		}

		virtual std::shared_ptr<VarPtrBase> PtrOf(const Obj& obj) override {
			return std::make_shared<VarPtr<const T>>(&Of(obj));
		}

		virtual std::shared_ptr<VarPtrBase> PtrOf(const Obj* obj) override {
			return std::make_shared<VarPtr<const T>>(&Of(obj));
		}

	private:
		T Obj::* get() const noexcept {
			return reinterpret_cast<T Obj::*>(MemVarBase<Obj>::var);
		}
		using MemVarBase<Obj>::As;

		VarPtr<T> pvar;
	};
}