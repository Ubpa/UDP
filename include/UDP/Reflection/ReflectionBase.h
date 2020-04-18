#pragma once

#include "VarPtr.h"

#include "../Basic/xSTL/xMap.h"

#include <memory>
#include <string>

namespace Ubpa {
	struct ReflectionBase {
		virtual xMap<std::string, std::shared_ptr<VarPtrBase>> VarPtrs(void* obj) const = 0;
		virtual xMap<std::string, std::shared_ptr<const VarPtrBase>> VarPtrs(const void* obj) const = 0;
	};
}
