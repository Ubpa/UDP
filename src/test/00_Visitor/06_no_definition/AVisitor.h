#pragma once

#include <UDP/Visitor/Visitor.h>

namespace Ubpa{
	struct A;
	struct B;
	struct C;

	class AVisitor : public RawPtrVisitor<AVisitor, A> {
	public:
		AVisitor();
	protected:
		void ImplVisit(A*);
		void ImplVisit(B*);
		void ImplVisit(C*);
	};
}