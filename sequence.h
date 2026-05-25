#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "exceptions.h"
#include "option.h"
#include "iterator.h"

template <class T>
class Sequence : public IEnumerable<T>
{
public:
    virtual ~Sequence() {}

    //Декомпозиция
    virtual const T& GetFirst() const = 0;
    virtual const T& GetLast()  const = 0;
    virtual const T& Get(int index) const = 0;
    virtual Sequence<T>* GetSubsequence(int startIndex, int endIndex) const = 0;
    virtual int GetLength() const = 0;

    //Операции
    virtual Sequence<T>* Append(const T& item) = 0;
    virtual Sequence<T>* Prepend(const T& item) = 0;
    virtual Sequence<T>* InsertAt(const T& item, int index) = 0;
    virtual Sequence<T>* RemoveAt(int index) = 0;
    virtual Sequence<T>* Concat(const Sequence<T>* other) = 0;

    //Базовый комплект операций map-reduce
    virtual Sequence<T>* Map(T (*f)(const T&)) const = 0;
    virtual Sequence<T>* Where(bool (*predicate)(const T&)) const = 0;
 
    virtual IEnumerator<T>* GetEnumerator() const override = 0;
    
    Option<T> FindElement(bool (*predicate)(const T&), bool fromFirst) const
    {
        int len = GetLength();
        int start = fromFirst ? 0 : len - 1;
        int end   = fromFirst ? len : -1;
        int step  = fromFirst ? 1 : -1;
        for (int i = start; i != end; i += step)
        {
            const T& item = Get(i);
            if (!predicate || predicate(item))
            {
                return Option<T>(item);
            }
        }
        return Option<T>();
    }

    virtual T Reduce(T (*func)(T, const T&), T initial) const
    {
        T result = initial;
        int len = GetLength();
        for (int i = 0; i < len; ++i)
        {
            result = func(result, Get(i));
        }
        return result;
    }

    virtual Option<T> GetFirst(bool (*predicate)(const T&)) const
    {
        return FindElement(predicate, true);
    }

    virtual Option<T> GetLast(bool (*predicate)(const T&)) const
    {
        return FindElement(predicate, false);
    }
    
};

#endif // SEQUENCE_H