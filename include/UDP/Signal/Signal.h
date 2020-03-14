#pragma once

#include <UTemplate/Typelist.h>

#include <map>
#include <functional>
namespace Ubpa::detail::Signal_ {
	template<typename SlotArgList>
	struct SlotFit;
}

namespace Ubpa {
	template<typename... Args>
	struct Signal {
		using ArgList = TypeList<Args...>;

		template<typename Func>
		size_t Connect(Func&& func);

		void Emit(Args... args);

	private:
		size_t id{ 0 };
		std::map<size_t, std::function<void(Args...)>> slots;

		template<typename SlotArgList>
		friend struct detail::Signal_::SlotFit;

		/*template<typename SlotArgList>
		template<typename Sig, typename Func, typename... Args>
		friend void detail::Signal_::SlotFit<SlotArgList>::template run<Sig, Func, Args...>(Sig, Func, Args...);*/
	};
}

#include "detail/Signal.inl"
