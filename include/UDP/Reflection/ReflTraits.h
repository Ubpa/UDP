#pragma once

#include "../Visitor/InfVisitor.h"
#include "VarPtr.h"

#include <map>

namespace Ubpa {
	template<typename T>
	struct Reflection;
	class ReflTraitsIniter;

	class ReflTraitsVisitor : public InfVisitor<ReflTraitsVisitor> {
		friend class ReflTraitsIniter;
	protected:
		virtual void Receive(const std::string& name, const std::map<std::string, std::shared_ptr<VarPtrBase>>& vars) = 0;

		template<typename T>
		void ImplVisit(T* obj) {
			auto name = Reflection<T>::Instance().GetName();
			auto nv = Reflection<T>::Instance().VarPtrs(*obj);
			Receive(name, nv);
		}
	};

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