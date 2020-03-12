#pragma  once

namespace Ubpa {
	template<typename Obj_T>
	class MemVar;

	template<typename Obj>
	class MemVar<void * Obj::*> {
	public:
		MemVar(void* Obj::* ptr = nullptr) : ptr(ptr) {}
		virtual ~MemVar() = default;

		template<typename U>
		const MemVar<U Obj::*> As() const noexcept {
			return reinterpret_cast<U Obj::*>(ptr);
		}

	protected:
		void* Obj::* ptr;
	};

	template<typename Obj, typename T>
	class MemVar<T Obj::*> final : public MemVar<void* Obj::*> {
	public:
		using Base = MemVar<void* Obj::*>;

		MemVar(T Obj::* ptr = nullptr) : Base(reinterpret_cast<void* Obj::*>(ptr)) {}

		T& Of(Obj& obj) const noexcept {
			return obj.*reinterpret_cast<T Obj::*>(Base::ptr);
		}
		const T& Of(const Obj& obj) const noexcept {
			return obj.*reinterpret_cast<T Obj::*>(Base::ptr);
		}

	private:
		using Base::As;
	};
}