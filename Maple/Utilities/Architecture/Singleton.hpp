#pragma once
#include <memory>

template<typename T>
class Singleton {
public:
    static T& Get()
    {
        static const std::unique_ptr<T> instance{ new T{singletonLock{ }} };
        return *instance;
    }

    Singleton(const Singleton&) = delete;
    Singleton& operator= (const Singleton) = delete;

protected:
    struct singletonLock {};
    Singleton() {}
};