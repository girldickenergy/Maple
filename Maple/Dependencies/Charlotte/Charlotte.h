#pragma once

#include "CharlotteFunction.h"

#include <map>
#include <functional>
#include <optional>

#include <Singleton.h>

class Charlotte : public Singleton<Charlotte>
{
	std::map<uintptr_t, CharlotteFunction> m_Map;

	bool checkIfHooked(uintptr_t address);

	/**
	 * \brief Calculates a hash based on a few instructions.
	 * 
	 * \param address Address pointing towards the function
	 * \return The calculated hash
	 */
	uint32_t calculateHash(uintptr_t address);

	bool callCheck(uintptr_t address);
public:
	explicit Charlotte(singletonLock);

	bool Add(uintptr_t address);

	/**
	 * \brief Attempts to call the function referenced, if it does not exist in the map, it will add it.
	 * 
	 * \param address Address to the function to call
	 * \return 
	 */
	template <typename T>
	T Call(uintptr_t address)
	{
		if (!callCheck(address))
			return { };

		typedef T(*functionType)();
		auto func = reinterpret_cast<functionType>(address);
		return func();
	}

	void Call(uintptr_t address)
	{
		if (!callCheck(address))
			return;

		typedef void(*functionType)();
		auto func = reinterpret_cast<functionType>(address);
		func();
	}

	template <typename T, typename Y>
	T Call(uintptr_t address, Y&& arg)
	{
		if (!callCheck(address))
			return { };

		typedef T(*functionType)(Y&&);
		auto func = reinterpret_cast<functionType>(address);
		return func(std::forward<Y>(arg));
	}

	template <typename Y>
	void Call(uintptr_t address, Y&& arg)
	{
		if (!callCheck(address))
			return;

		typedef void(*functionType)(Y&&);
		auto func = reinterpret_cast<functionType>(address);
		func(std::forward<Y>(arg));
	}

	template <typename T, typename Y, typename U>
	T Call(uintptr_t address, Y&& arg1, U&& arg2)
	{
		if (!callCheck(address))
			return { };

		typedef T(*functionType)(Y&&, U&&);
		auto func = reinterpret_cast<functionType>(address);
		return func(std::forward<Y>(arg1), std::forward<U>(arg2));
	}

	template <typename Y, typename U>
	void Call(uintptr_t address, Y&& arg1, U&& arg2)
	{
		if (!callCheck(address))
			return;

		typedef void(*functionType)(Y&&, U&&);
		auto func = reinterpret_cast<functionType>(address);
		func(std::forward<Y>(arg1), std::forward<U>(arg2));
	}

	template <typename T, typename Y, typename U, typename I>
	T Call(uintptr_t address, Y&& arg1, U&& arg2, I&& arg3)
	{
		if (!callCheck(address))
			return { };

		typedef T(*functionType)(Y&&, U&&, I&&);
		auto func = reinterpret_cast<functionType>(address);
		return func(std::forward<Y>(arg1), std::forward<U>(arg2), std::forward<I>(arg3));
	}

	template <typename Y, typename U, typename I>
	void Call(uintptr_t address, Y&& arg1, U&& arg2, I&& arg3)
	{
		if (!callCheck(address))
			return;

		typedef void(*functionType)(Y&&, U&&, I&&);
		auto func = reinterpret_cast<functionType>(address);
		func(std::forward<Y>(arg1), std::forward<U>(arg2), std::forward<I>(arg3));
	}

	template <typename T, typename Y, typename U, typename I, typename O>
	T Call(uintptr_t address, Y&& arg1, U&& arg2, I&& arg3, O&& arg4)
	{
		if (!callCheck(address))
			return { };

		typedef T(*functionType)(Y&&, U&&, I&&, O&&);
		auto func = reinterpret_cast<functionType>(address);
		return func(std::forward<Y>(arg1), std::forward<U>(arg2), std::forward<I>(arg3), std::forward<O>(arg4));
	}

	template <typename Y, typename U, typename I, typename O>
	void Call(uintptr_t address, Y&& arg1, U&& arg2, I&& arg3, O&& arg4)
	{
		if (!callCheck(address))
			return;

		typedef void(*functionType)(Y&&, U&&, I&&, O&&);
		auto func = reinterpret_cast<functionType>(address);
		func(std::forward<Y>(arg1), std::forward<U>(arg2), std::forward<I>(arg3), std::forward<O>(arg4));
	}

	template <typename T, typename Y, typename U, typename I, typename O, typename P>
	T Call(uintptr_t address, Y&& arg1, U&& arg2, I&& arg3, O&& arg4, P&& arg5)
	{
		if (!callCheck(address))
			return { };

		typedef T(*functionType)(Y&&, U&&, I&&, O&&, P&&);
		auto func = reinterpret_cast<functionType>(address);
		return func(std::forward<Y>(arg1), std::forward<U>(arg2), std::forward<I>(arg3), std::forward<O>(arg4), std::forward<P>(arg5));
	}

	template <typename T, typename Y, typename U, typename I, typename O, typename P, typename A>
	T Call(uintptr_t address, Y&& arg1, U&& arg2, I&& arg3, O&& arg4, P&& arg5, A&& arg6)
	{
		if (!callCheck(address))
			return { };

		typedef T(*functionType)(Y&&, U&&, I&&, O&&, P&&, A&&);
		auto func = reinterpret_cast<functionType>(address);
		return func(std::forward<Y>(arg1), std::forward<U>(arg2), std::forward<I>(arg3), std::forward<O>(arg4), std::forward<P>(arg5), std::forward<A>(arg6));
	}
};
