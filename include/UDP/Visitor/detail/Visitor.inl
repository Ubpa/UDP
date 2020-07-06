#include "../../Basic/vtable.h"

#include <UTemplate/Func.h>
#include <UTemplate/Concept.h>
#include <UTemplate/TypeID.h>

#include <unordered_map>
#include <functional>

namespace Ubpa {
	template<typename T>
	constexpr size_t Visitor_GetID() noexcept {
		static_assert(!std::is_const_v<T> && !std::is_pointer_v<T>,
			"GetID: <T> must be non-const and non-pointer");
		if constexpr (std::is_polymorphic_v<T>)
			return reinterpret_cast<size_t>(vtable_of<T>::get());
		else
			return Ubpa::TypeID<T>;
	}
	template<typename T>
	constexpr size_t Visitor_GetID(const T* ptr) noexcept {
		assert(ptr != nullptr);
		if constexpr (std::is_void_v<T> || std::is_polymorphic_v<T>)
			return reinterpret_cast<size_t>(vtable(ptr));
		else
			return Ubpa::TypeID<T>;
	}
}

namespace Ubpa::detail::Visitor_ {
	template<typename Impl, typename Func>
	Concept(HaveImplVisit, MemFuncOf<Func>::template run<Impl>(&Impl::ImplVisit));

	template<typename Impl>
	struct Accessor : Impl {
		template<typename DerivedPtr, typename Ret, typename... Args>
		static Ret run(Impl* impl, const void* obj, Args... args) {
			// impl->ImplVisit
			return (impl->*MemFuncOf<Ret(DerivedPtr, Args...)>::template run<Impl>(&Impl::ImplVisit))(reinterpret_cast<DerivedPtr>(const_cast<void*>(obj)), std::forward<Args>(args)...);
		}

		template<typename DerivedPtr, typename Ret, typename... Args>
		static Ret run(const Impl* impl, const void* obj, Args... args) {
			// impl->ImplVisit
			return (impl->*MemFuncOf<Ret(DerivedPtr, Args...)const>::template run<Impl>(&Impl::ImplVisit))(reinterpret_cast<DerivedPtr>(const_cast<void*>(obj)), std::forward<Args>(args)...);
		}

		template<typename T>
		static Impl* cast(T* ptr) noexcept {
			return static_cast<Impl*>(ptr);
		}

		template<typename T>
		static const Impl* cast(const T* ptr) noexcept {
			return static_cast<const Impl*>(ptr);
		}
	};
}
