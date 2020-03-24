#pragma once

namespace Ubpa {
	struct VarPtrBase { virtual ~VarPtrBase() = default; };

	template<typename T>
	struct VarPtr : VarPtrBase {
		VarPtr(T* pvar = nullptr) : pvar{ pvar } {}

		T* get() noexcept { return pvar; }
		T* const get() const noexcept { return const_cast<VarPtr*>(this)->get(); }
		T& var() const noexcept { return *get(); }
		T* operator->() noexcept { return get(); }
		T* const operator->() const noexcept { return get(); }
		T& operator*() const noexcept { return *get(); }

		T* pvar;
	};
}
