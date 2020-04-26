#pragma once

#include "../Visitor.h"
#include "VarPtr.h"

#include "../Basic/xSTL/xMap.h"

namespace Ubpa {
	struct ReflectionBase;
	template<typename T>
	struct Reflection;
	class ReflTraitsIniter;

	// for ReflTraitsIniter
	class ReflTraitsVisitor : public Visitor<void(ReflTraitsVisitor::*)()> {
	protected:
		virtual void Receive(void* obj, std::string_view name, ReflectionBase& refl) = 0;

	private:
		friend class ReflTraitsIniter;
		friend struct Visitor<void(ReflTraitsVisitor::*)()>::Accessor;

		template<typename T>
		void ImplVisit(T* obj) {
			auto& refl = Reflection<T>::Instance();
			Receive(obj, refl.Name(), refl);
		}
	};

	// singleton
	// help for ReflTraitsVisitor's registion
	class ReflTraitsIniter {
	public:
		static ReflTraitsIniter& Instance() {
			static ReflTraitsIniter instance;
			return instance;
		}

		template<typename T>
		void Regist() {
			if constexpr (std::is_polymorphic_v<T>) {
				inits.push_back([](ReflTraitsVisitor& visitor) {
					visitor.Regist<T>();
				});
			}
		}

		void Init(ReflTraitsVisitor& visitor) const {
			for (const auto& init : inits)
				init(visitor);
		}

	private:
		std::vector<std::function<void(ReflTraitsVisitor&)>> inits;
		ReflTraitsIniter() = default;
	};
}
