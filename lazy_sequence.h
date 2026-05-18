#ifndef LAZY_SEQUENCE_H
#define LAZY_SEQUENCE_H

#include "sequence.h"
#include "array_sequence.h"
#include "generator.h"
#include "igenerator_rule.h"
#include "lazy_rules.h"

template <class T>
class LazySequence : public Sequence<T> 
{
private:
    mutable MutableArraySequence<T>* cache;
    mutable Generator<T>* generator;

    void MaterializeUpTo(int index) const 
    {
        while (cache->GetLength() <= index) 
        {
            if (generator && generator->HasNext()) 
            {
                //Передаем кэш в качестве контекста
                T nextVal = generator->GetNext(cache); 
                cache->Append(nextVal);                
            } else 
            {
                throw IndexOutOfRangeException("Reached the end of finite lazy sequence");
            }
        }
    }

public:
    LazySequence(IGeneratorRule<T>* rule, int initialCount = 0, const T* initialItems = nullptr) 
    {
        cache = new MutableArraySequence<T>();
        if (initialItems != nullptr && initialCount > 0) 
        {
            for (int i = 0; i < initialCount; ++i) 
            {
                cache->Append(initialItems[i]);
            }
        }
        generator = rule ? new Generator<T>(rule) : nullptr;
    }

    ~LazySequence() override 
    {
        delete cache;
        delete generator;
    }

    const T& Get(int index) const override 
    {
        if (index < 0) 
        {
            throw IndexOutOfRangeException("Index cannot be negative");
        }
        MaterializeUpTo(index);
        return cache->Get(index);
    }

    const T& GetFirst() const override 
    { 
        return Get(0); 
    }

    const T& GetLast() const override 
    {
        int len = cache->GetLength();
        if (len == 0 && (!generator || !generator->HasNext())) 
        {
            throw IndexOutOfRangeException();
        }
        if (len == 0) 
        {
            return Get(0);
        }
        return cache->Get(len - 1); 
    }

    int GetLength() const override 
    { 
        return cache->GetLength(); 
    }

    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override 
    {
        MaterializeUpTo(endIndex);
        return cache->GetSubsequence(startIndex, endIndex);
    }

    Sequence<T>* Map(T (*f)(const T&)) const override 
    {
        return new LazySequence<T>(new MapRule<T>(this, f));
    }

    Sequence<T>* Where(bool (*predicate)(const T&)) const override 
    {
        return new LazySequence<T>(new WhereRule<T>(this, predicate));
    }

    Sequence<T>* Concat(Sequence<T>* other) override 
    {
        if (!other) 
        {
            throw InvalidArgumentException("Cannot concat with null");
        }
        return new LazySequence<T>(new ConcatRule<T>(this, other));
    }

    // Операции модификации не применимы к чисто ленивым спискам (константам),
    // Но могут быть реализованы через правила, если необходимо:
    Sequence<T>* InsertAt(const T& item, int index) override { throw InvalidOperationException("Not implemented here"); }
    Sequence<T>* RemoveAt(int index) override { throw InvalidOperationException("Not implemented here"); }
    Sequence<T>* Append(const T& item) override { throw InvalidOperationException("Not implemented here"); }
    Sequence<T>* Prepend(const T& item) override { throw InvalidOperationException("Not implemented here"); }

    IEnumerator<T>* GetEnumerator() const override 
    {
        return cache->GetEnumerator();
    }
};

#endif // LAZY_SEQUENCE_H