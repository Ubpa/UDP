#pragma once

namespace Ubpa {
	template<typename T>
	struct CustomID {
		// return custom ID of T
		// if T is polymorphic, then return it's virtual table
		// * it will create a object of T, and get the virtual table from the object
		// * if T is not default constructable, you should call vtable_of<T>::regist(ptr) firstly
		// * or call CustomID(ptr)
		// else return TypeID<T>
		static constexpr size_t Of() noexcept;

		// return custom ID of T
		// if T is polymorphic, then return it's virtual table getting from ptr
		// else return TypeID<T>
		static constexpr size_t Of(const T* ptr) noexcept;
	};
}

#include "detail/CustomID.inl"
