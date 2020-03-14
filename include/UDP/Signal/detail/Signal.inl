#pragma once

#include <UTemplate/FuncTraits.h>

#include <tuple>

namespace Ubpa::detail::Signal_ {
	template<typename SlotArgList>
	struct SlotFit;
}

namespace Ubpa {
	template<typename... Args>
	template<typename Slot>
	size_t Signal<Args...>::Connect(Slot&& slot) {
		using SlotArgList = typename FuncTraits<Slot>::ArgList;
		slots[id] = detail::Signal_::SlotFit<SlotArgList>::run(std::forward<Slot>(slot), TypeList<Args...>{});
		return id++;
	}

	template<typename... Args>
	void Signal<Args...>::Emit(Args... args) const {
		for (auto p : slots)
			p.second(args...);
	}
}

namespace Ubpa::detail::Signal_ {
	// ref: qobjectdefs_impl.h

	template <typename T> struct RmvLValueRef : IType<T> {};
	template <typename T> struct RmvLValueRef<T&> : IType<T> {};
	template <typename T> struct RmvConstRef : IType<T> {};
	template <typename T> struct RmvConstRef<const T&> : IType<T> {};

	template<typename A1, typename A2>
	struct AreArgumentsCompatible
		: std::is_same<const typename RmvLValueRef<A1>::type&, const typename RmvLValueRef<A2>::type&> {};
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
			AreArgumentsCompatible<typename RmvConstRef<SignalArgHead>::type,
				typename RmvConstRef<SlotArgHead>::type>::value
			&& CheckCompatibleArguments<TypeList<SignalArgTail...>, TypeList<SlotArgTail...>>::value;
	};

	// =========================

	template<typename... SlotArgs>
	struct SlotFit<TypeList<SlotArgs...>> {
		using SlotArgList = TypeList<SlotArgs...>;
		template<typename Slot, typename... SignalArgs>
		static auto run(Slot&& slot, TypeList<SignalArgs...>) {
			using SignalArgList = TypeList<SignalArgs...>;
			return [slot = std::forward<Slot>(slot)](SignalArgs... signalArgs) {
				std::tuple<SignalArgs...> argTuple{ signalArgs... };
				static_assert(detail::Signal_::CheckCompatibleArguments<SignalArgList, SlotArgList>::value,
					"Signal and slot arguments are not compatible.");
				slot(std::get<Find_v<SlotArgList, SlotArgs>>(argTuple)...);
			};
		}
	};
}
