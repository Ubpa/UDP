#pragma once

#include <UTemplate/FuncTraits.h>

#include <tuple>

namespace Ubpa::detail::Signal_ {
	template<typename SignalArgList, typename SlotArgList>
	struct CheckCompatibleArguments;
}

namespace Ubpa {
	template<typename... Args>
	template<typename Slot>
	size_t Signal<Args...>::Connect(Slot&& slot) {
		using SlotArgList = typename FuncTraits<Slot>::ArgList;
		detail::Signal_::SlotFit<SlotArgList>::run(*this, std::forward<Slot>(slot), TypeList<Args...>{});
		return id++;
	}

	template<typename... Args>
	void Signal<Args...>::Emit(Args... args) {
		for (auto p : slots)
			p.second(args...);
	}
}

namespace Ubpa::detail::Signal_ {
	// ref: qobjectdefs_impl.h

	template<typename A1, typename A2>
	struct AreArgumentsCompatible
		: std::is_same<const std::remove_reference<A1>&, const std::remove_reference<A2>&> {};
	template<typename A1, typename A2> struct AreArgumentsCompatible<A1, A2&>
		: std::false_type {};
	template<typename A> struct AreArgumentsCompatible<A&, A&>
		: std::true_type {};
	// void as a return value
	template<typename A> struct AreArgumentsCompatible<void, A>
		: std::true_type {};
	template<typename A> struct AreArgumentsCompatible<A, void>
		: std::true_type {};
	template<> struct AreArgumentsCompatible<void, void>
		: std::true_type {};

	// ========================

	template<typename SignalArgList, typename SlotArgList>
	struct CheckCompatibleArguments
		: std::false_type {};

	template<>
	struct CheckCompatibleArguments<TypeList<>, TypeList<>>
		: std::true_type {};

	template<typename SignalArgList>
	struct CheckCompatibleArguments<SignalArgList, TypeList<>>
		: std::true_type {};

	template<typename SlotArgHead, typename... SlotArgTail>
	struct CheckCompatibleArguments<TypeList<>, TypeList<SlotArgHead, SlotArgTail...>>
		: std::false_type {};

	template<typename SignalArgHead, typename SlotArgHead, typename... SignalArgTail, typename... SlotArgTail>
	struct CheckCompatibleArguments<TypeList<SignalArgHead, SignalArgTail...>, TypeList<SlotArgHead, SlotArgTail...>> {
		static constexpr bool value =
			AreArgumentsCompatible<std::decay_t<SignalArgHead>, std::decay_t<SlotArgHead>>::value
			&& CheckCompatibleArguments<TypeList<SignalArgTail...>, TypeList<SlotArgTail...>>::value;
	};

	// =========================

	template<typename... SlotArgs>
	struct SlotFit<TypeList<SlotArgs...>> {
		using SlotArgList = TypeList<SlotArgs...>;
		template<typename Sig, typename Slot, typename... SignalArgs>
		static void run(Sig& sig, Slot&& slot, TypeList<SignalArgs...>) {
			using SignalArgList = TypeList<SignalArgs...>;
			sig.slots[sig.id] = [slot = std::forward<Slot>(slot)](SignalArgs... signalArgs) {
				std::tuple<SignalArgs...> argTuple{ signalArgs... };
				if constexpr (detail::Signal_::CheckCompatibleArguments<SignalArgList, SlotArgList>::value)
					slot(std::get<Find_v<SlotArgList, SlotArgs>>(argTuple)...);
				else
					slot(std::get<SlotArgs>(argTuple)...);
			};
		}
	};
}
