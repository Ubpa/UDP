#pragma once

#include <map>
#include <functional>

namespace Ubpa {
	template<typename... Args>
	struct Signal {
		template<typename Slot>
		size_t Connect(Slot&& slot);

		void Emit(Args... args) const;

		void Disconnect(size_t id) { slots.erase(id); }

	private:
		size_t id{ 0 };
		std::map<size_t, std::function<void(Args...)>> slots;
	};
}

#include "detail/Signal.inl"
