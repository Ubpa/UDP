#pragma once

#include "VarPtr.h"

#include "../Basic/xSTL/xMap.h"

#include <memory>
#include <string>

namespace Ubpa {
	struct ReflectionBase {
		virtual xMap<std::string, std::shared_ptr<VarPtrBase>> VarPtrs(void* obj) const = 0;
		virtual xMap<std::string, std::shared_ptr<const VarPtrBase>> VarPtrs(const void* obj) const = 0;
		virtual const std::string Meta(const std::string& key) const = 0;
		virtual const xMap<std::string, std::string> Metas() const = 0;

		struct Meta
		{
			static constexpr char value_true[] = "true";
			static constexpr char value_false[] = "false";
			static constexpr char is_read_only[] = "is_read_only";
		};

		const std::string FieldMeta(std::string_view field, std::string_view kind) const {
			return Meta(std::string(field) + "::" + kind.data());
		}
	};
}
