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

    template <typename T, typename ... Args>
    T Call(uintptr_t address, Args&& ... args)
    {
        if (!callCheck(address))
            return { };

        typedef T(*functionType)(Args...);
        auto func = reinterpret_cast<functionType>(address);

        return func(std::forward<Args>(args)...);
    }

    template <typename ... Args>
    void Call(uintptr_t address, Args&& ... args)
    {
        if (!callCheck(address))
            return;

        typedef void(*functionType)(Args...);
        auto func = reinterpret_cast<functionType>(address);

        func(std::forward<Args>(args)...);
    }
};
