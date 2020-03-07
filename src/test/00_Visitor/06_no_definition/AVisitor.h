#pragma once

#include <UDP/Visitor.h>

namespace Ubpa{
	struct A;
	struct B;
	struct C;

	class AVisitor final : public RawPtrMultiVisitor<AVisitor, A, B, C> {
	public:
		AVisitor();
	protected:
		void ImplVisit(A*);
		void ImplVisit(B*);
		void ImplVisit(C*);
	};
}