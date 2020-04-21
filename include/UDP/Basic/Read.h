#pragma once

#include <utility>

namespace Ubpa {
	// when Reflection regist, it will be T Friend::*
	// so you should treat it as T but read only
	template<typename Friend, typename T>
	struct Read {
		template<typename... Args>
		Read(Args&&... args) : val{ std::forward<Args>(args)... } {}

		Read(const Read& read) noexcept : val{ read.val } {}
		Read(Read&& read) noexcept : val{ std::move(read.val) } {}

		const T& get() const noexcept { return val; }
		operator T() const noexcept { return val; }

		T* operator->() noexcept { return &val; }
		const T* operator->() const noexcept { return &val; }

		template<typename F, typename U>
		auto operator+(const Read<F, U>& y) const noexcept { return val + y.val; }
		template<typename F, typename U>
		auto operator-(const Read<F, U>& y) const noexcept { return val - y.val; }
		template<typename F, typename U>
		auto operator*(const Read<F, U>& y) const noexcept { return val * y.val; }
		template<typename F, typename U>
		auto operator/(const Read<F, U>& y) const noexcept { return val / y.val; }
		template<typename F, typename U>
		auto operator%(const Read<F, U>& y) const noexcept { return val % y.val; }

		template<typename U>
		auto operator+(const U& y) const noexcept { return val + y; }
		template<typename U>
		auto operator-(const U& y) const noexcept { return val - y; }
		template<typename U>
		auto operator*(const U& y) const noexcept { return val * y; }
		template<typename U>
		auto operator/(const U& y) const noexcept { return val / y; }
		template<typename U>
		auto operator%(const U& y) const noexcept { return val % y; }

		const auto& operator[](size_t n) const noexcept { return val[n]; }

		bool operator==(const T& y) const noexcept { return val == y; }
		bool operator!=(const T& y) const noexcept { return val != y; }
		bool operator>(const T& y) const noexcept { return val > y; }
		bool operator>=(const T& y) const noexcept { return val >= y; }
		bool operator<(const T& y) const noexcept { return val < y; }
		bool operator<=(const T& y) const noexcept { return val <= y; }

		auto operator*() const noexcept { return *val; }

		auto begin() const noexcept { return val.begin(); }
		auto end() const noexcept { return val.end(); }

	private:
		T val;
		
		Read& operator=(const Read& read) noexcept { val = read.val; return *this; }
		Read& operator=(Read&& read) noexcept { val = std::move(read.val); return *this; }

		friend Friend;
		template<typename F, typename U>
		friend struct Read;
	};

	template<typename Friend, typename T>
	struct Read<Friend, T*> {
		template<typename... Args>
		Read(Args&&... args) : val{ std::forward<Args>(args)... } {}

		T* get() const noexcept { return val; }

		operator T*() const noexcept { return val; }

		T* operator->() const noexcept { return val; }

		Read(const Read& read) noexcept : val{ read.val } {}
		Read(Read&& read) noexcept : val{ std::move(read.val) } {}

	private:
		friend Friend;
		
		Read& operator=(const Read& read) noexcept { val = read.val; return *this; }
		Read& operator=(Read&& read) noexcept { val = std::move(read.val); return *this; }
		
		T* val;
	};
}