#ifndef ITERATOR_H
#define ITERATOR_H

#include "exceptions.h"

//Базовый интерфейс итератора
template <class T>
class IEnumerator 
{
public:
    IEnumerator() = default;          
    virtual ~IEnumerator() {}
    virtual bool MoveNext() = 0;
    virtual const T& Current() const = 0;
    virtual void Reset() = 0;

    virtual void Release()
    { 
        delete this; 
    }

    //Запрет поверхностного копирования
    IEnumerator(const IEnumerator&) = delete;
    IEnumerator& operator=(const IEnumerator&) = delete;
};

//Интерфейс для коллекции
template <class T>
class IEnumerable 
{
public:
    virtual ~IEnumerable() {}
    virtual IEnumerator<T>* GetEnumerator() const = 0;
};

#endif // ITERATOR_H