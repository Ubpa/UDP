#pragma once

#include "VarPtr.h"

#include <UContainer/xSTL/xMap.h>

#include <memory>
#include <string>

namespace Ubpa {
	struct ReflectionBase {
		struct MetaKey {
			static constexpr std::string_view empty_value = "";
			static constexpr std::string_view read_only = "read_only";
		};

		virtual std::string_view Name() const = 0;

		virtual xMap<std::string, std::shared_ptr<VarPtrBase>> VarPtrs(void* obj) const = 0;
		virtual xMap<std::string, std::shared_ptr<const VarPtrBase>> VarPtrs(const void* obj) const = 0;

		virtual const std::string& Meta(std::string_view key) const = 0;
		virtual bool HaveMeta(std::string_view key) const = 0;
		virtual const xMap<std::string, std::string>& Metas() const = 0;

		virtual const std::string& FieldMeta(std::string_view field, std::string_view key) const = 0;
		virtual bool HaveFieldMeta(std::string_view field, std::string_view key) const = 0;
		virtual const xMap<std::string, xMap<std::string, std::string>>& FieldMetas() const = 0;
	};
}
