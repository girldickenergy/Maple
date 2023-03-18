#pragma once

#include <memory>
#include <mutex>

template<typename T>
class Singleton {
public:
    static T& Get()
    {
        std::unique_lock lock(initMutex);
        static const std::unique_ptr<T> instance{ new T{singletonLock{ }} };
        lock.unlock();

        return *instance;
    }

    Singleton(const Singleton&) = delete;
    Singleton& operator= (const Singleton) = delete;

protected:
    struct singletonLock {};
    Singleton() {}

private:
    static inline std::mutex initMutex;
};