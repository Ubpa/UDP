#pragma once

#include "../Visitor/InfVisitor.h"
#include "VarPtr.h"

#include <map>

namespace Ubpa {
	template<typename T>
	struct Reflection;
	class ReflTraitsIniter;

	// for ReflTraitsIniter
	// get name and vars
	class ReflTraitsVisitor : public InfVisitor<ReflTraitsVisitor> {
		friend class ReflTraitsIniter;

	protected:
		// obj is used for special visit (not just name and vars)
		virtual void Receive(void* obj, const std::string& name, const std::map<std::string, std::shared_ptr<VarPtrBase>>& vars) {};
		// obj is used for special visit (not just name and vars)
		virtual void Receive(const void* obj, const std::string& name, const std::map<std::string, std::shared_ptr<const VarPtrBase>>& vars) {};

	private:
		friend struct InfVisitor<ReflTraitsVisitor>::Accessor;

		template<typename T>
		void ImplVisit(T* obj) {
			auto name = Reflection<T>::Instance().GetName();
			auto nv = Reflection<T>::Instance().VarPtrs(*obj);
			Receive(obj, name, nv);
		}

		template<typename T>
		void ImplVisit(const T* obj) {
			auto name = Reflection<T>::Instance().GetName();
			auto nv = Reflection<T>::Instance().VarPtrs(*obj);
			Receive(obj, name, nv);
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
				cinits.push_back([](ReflTraitsVisitor& visitor) {
					visitor.RegistC<T>();
				});
			}
		}

		void Init(ReflTraitsVisitor& visitor) const {
			InitNC(visitor);
			InitC(visitor);
		}

		void InitNC(ReflTraitsVisitor& visitor) const {
			for (const auto& init : inits)
				init(visitor);
		}

		void InitC(ReflTraitsVisitor& visitor) const {
			for (const auto& cinit : cinits)
				cinit(visitor);
		}

	private:
		std::vector<std::function<void(ReflTraitsVisitor&)>> inits;
		std::vector<std::function<void(ReflTraitsVisitor&)>> cinits;
		ReflTraitsIniter() = default;
	};
}
