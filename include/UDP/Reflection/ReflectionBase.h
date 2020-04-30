#pragma once

#include "VarPtr.h"

#include "../Basic/xSTL/xMap.h"

#include <memory>
#include <string>

namespace Ubpa {
	struct ReflectionBase {
		virtual xMap<std::string, std::shared_ptr<VarPtrBase>> VarPtrs(void* obj) const = 0;
		virtual xMap<std::string, std::shared_ptr<const VarPtrBase>> VarPtrs(const void* obj) const = 0;
		virtual const std::string Meta(std::string_view key) const = 0;
		virtual const xMap<std::string, std::string>& Metas() const = 0;
		virtual const std::string FieldMeta(std::string_view field, std::string_view kind) const = 0;

		struct Meta
		{
			static constexpr std::string_view default_value = "null";
			static constexpr std::string_view read_only = "read_only";
		};
	};
}
