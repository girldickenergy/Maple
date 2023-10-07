#pragma once

#include <any>
#include <map>
#include <memory>
#include <typeindex>

class DependencyContainer
{
    std::map<std::type_index, std::any> m_Dependencies;

public:
    template <typename T> void Cache(std::shared_ptr<T> object)
    {
        m_Dependencies[std::type_index(typeid(T))] = object;
    }

    template <typename T> std::shared_ptr<T> Resolve()
    {
        return std::any_cast<std::shared_ptr<T>>(m_Dependencies[std::type_index(typeid(T))]);
    }
};