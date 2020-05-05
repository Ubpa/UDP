#pragma once

#include "detail/Visitor.inl"

#include "cVisitor.h"
#include "ncVisitor.h"
#include "cicVisitor.h"
#include "cincVisitor.h"
#include "ncicVisitor.h"
#include "ncincVisitor.h"

namespace Ubpa {
	// [Func]
	// - Ret([const] void*, Args...)
	// - Ret(Impl::*)([const] void*, Args...)[const]
	// [ID] vtable, TypeID, customed ID
	template<typename Func>
	class Visitor;
}
