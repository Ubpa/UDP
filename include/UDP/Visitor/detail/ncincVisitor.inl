#pragma once

namespace Ubpa {
	template<typename Impl, typename Ret, typename... Args>
	Ret Visitor<Ret(Impl::*)(void*, Args...)>::Visit(size_t ID, void* ptr, Args... args) {
		assert("ERROR::Visitor::Visit: unregistered" && IsRegistered(ID));
		auto target = impl_callbacks.find(ID);
		if (target != impl_callbacks.end())
			return target->second(detail::Visitor_::Accessor<Impl>::template cast(this), ptr, std::forward<Args>(args)...);
		else
			return this->callbacks.find(ID)->second(ptr, std::forward<Args>(args)...);
	}

	template<typename Impl, typename Ret, typename... Args>
	Ret Visitor<Ret(Impl::*)(void*, Args...)>::Visit(void* ptr, Args... args) {
		return Visit(reinterpret_cast<size_t>(vtable(ptr)), ptr, std::forward<Args>(args)...);
	}

	template<typename Impl, typename Ret, typename... Args>
	template<typename T>
	inline Ret Visitor<Ret(Impl::*)(void*, Args...)>::Visit(T* ptr, Args... args) {
		if constexpr (std::is_polymorphic_v<T>)
			return Visit(static_cast<void*>(ptr), std::forward<Args>(args)...);
		else
			return Visit(TypeID<T>, ptr, std::forward<Args>(args)...);
	}

	template<typename Impl, typename Ret, typename... Args>
	template<typename... Deriveds>
	void Visitor<Ret(Impl::*)(void*, Args...)>::Register() {
		(RegisterOne<Deriveds>(), ...);
	}

	template<typename Impl, typename Ret, typename... Args>
	template<typename Derived>
	void Visitor<Ret(Impl::*)(void*, Args...)>::RegisterOne() {
		impl_callbacks[detail::Visitor_::GetID<Derived>()] = [](Impl* impl, void* ptr, Args... args) {
			return detail::Visitor_::Accessor<Impl>::
				template run<Derived*, Ret>(impl, ptr, std::forward<Args>(args)...);
		};
	}

	template<typename Impl, typename Ret, typename... Args>
	bool Visitor<Ret(Impl::*)(void*, Args...)>::IsRegistered(size_t ID) const {
		return impl_callbacks.find(ID) != impl_callbacks.end()
			|| this->callbacks.find(ID) != this->callbacks.end();
	}

	template<typename Impl, typename Ret, typename... Args>
	template<typename T>
	bool Visitor<Ret(Impl::*)(void*, Args...)>::IsRegistered() const {
		return IsRegistered(Visitor<Ret(void*, Args...)>::template GetID<T>());
	}

	template<typename Impl, typename Ret, typename... Args>
	bool Visitor<Ret(Impl::*)(void*, Args...)>::IsRegistered(const void* ptr) const {
		return IsRegistered(reinterpret_cast<size_t>(vtable(ptr)));
	}
}
