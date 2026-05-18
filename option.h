#ifndef OPTION_H
#define OPTION_H

#include "exceptions.h"

template <class T>
class Option 
{
private:
    bool hasValue;
    T value;
    
public:
    //Конструктор для None
    Option() : hasValue(false), value() {}
    
    //Конструктор для Some
    Option(const T& val) : hasValue(true), value(val) {}
    
    //Конструктор копирования
    Option(const Option<T>& other) : hasValue(other.hasValue), value()
    {
        if (hasValue) 
        {
            value = other.value;
        }
    }
    
    Option<T>& operator=(const Option<T>& other)
    {
        if (this != &other)
        {
            hasValue = other.hasValue;
            if (hasValue)
            {
                value = other.value;
            }
        }
        return *this;
    }

    //Деструктор
    ~Option() noexcept {}
    
    //Проверка наличия значения
    bool HasValue() const noexcept
    {
        return hasValue;
    }
    
    T GetValue() const 
    {
        if (!hasValue) 
        {
            throw InvalidOperationException();
        }
        return value;
    }
    
    //Получение значения с значением по умолчанию
    T GetValueOrDefault(const T& defaultValue) const 
    {
        return hasValue ? value : defaultValue;
    }
    
    explicit operator bool() const noexcept
    {
        return hasValue;
    }

    //Доступ к значению
    const T& operator*() const 
    {
        if (!hasValue) 
        {
            throw InvalidOperationException();
        }
        return value;
    }

    T& operator*() 
    {
        if (!hasValue) 
        {
            throw InvalidOperationException();
        }
        return value;
    }

    const T* operator->() const 
    { 
        if (!hasValue) 
        {
            throw InvalidOperationException();
        }
        return &value;
    }

    T* operator->()
    {
        if (!hasValue)
        {
            throw InvalidOperationException();
        }
        return &value;
    }

    friend bool operator==(const Option<T>& lhs, const Option<T>& rhs) 
    {
        if (!lhs.hasValue && !rhs.hasValue) 
        {
            return true;
        }
        
        if (lhs.hasValue != rhs.hasValue) 
        {
            return false;
        }
        return lhs.value == rhs.value;
    }
};

#endif // OPTION_H