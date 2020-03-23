#pragma once

#include "VarPtr.h"

#include <memory>
#include <map>
#include <string>

namespace Ubpa {
	struct ReflectionBase {
		virtual std::map<std::string, std::shared_ptr<VarPtrBase>> VarPtrs(void* obj) const = 0;
		virtual std::map<std::string, std::shared_ptr<const VarPtrBase>> VarPtrs(const void* obj) const = 0;
	};
}
