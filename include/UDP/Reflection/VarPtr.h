#pragma once

namespace Ubpa {
	struct VarPtrBase {
		VarPtrBase(void* pvar = nullptr) : pvar{ pvar } {};
		virtual ~VarPtrBase() = default;
		void* pvar;
	};

	template<typename T>
	struct VarPtr : VarPtrBase {
		VarPtr(T* pvar = nullptr) : VarPtrBase{ reinterpret_cast<void*>(pvar) } {}
		T& get() noexcept { return *reinterpret_cast<T*>(this->pvar); }
		const T& get() const noexcept { return const_cast<VarPtr*>(this)->get(); }
		/*T* operator->() noexcept { return get(); }
		const T* operator->() const noexcept { return get(); }
		T& operator*() noexcept { return *get(); }
		const T& operator*() const noexcept { return *get(); }*/
	};

	template<typename T>
	struct VarPtr<const T> : VarPtrBase {
		VarPtr(const T* pvar = nullptr) : VarPtrBase{ reinterpret_cast<void*>(const_cast<T*>(pvar)) } {}
		const T& get() const noexcept { return *reinterpret_cast<VarPtr*>(this->pvar); }
		/*T* operator->() noexcept { return get(); }
		const T* operator->() const noexcept { return get(); }
		T& operator*() noexcept { return *get(); }
		const T& operator*() const noexcept { return *get(); }*/
	};
}
