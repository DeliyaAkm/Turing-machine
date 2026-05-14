#ifndef LAZY_SEQUENCE_H
#define LAZY_SEQUENCE_H

#include "sequence.h"
#include "array_sequence.h"
#include "generator.h"
#include "igenerator_rule.h"

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
                T nextVal = generator->GetNext(); 
                cache->Append(nextVal);                
            } else 
            {
                throw IndexOutOfRangeException("Cannot materialize: reached the end of finite lazy sequence");
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
        
        //Передаем генератору указатель на кэш 
        generator = new Generator<T>(cache, rule);
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
        if (len == 0) 
        {
            throw IndexOutOfRangeException("Sequence is empty");
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

    //Заглушки........................................
    Sequence<T>* Append(const T& item) override { throw NotImplementedException(); }
    Sequence<T>* Prepend(const T& item) override { throw NotImplementedException(); }
    Sequence<T>* InsertAt(const T& item, int index) override { throw NotImplementedException(); }
    Sequence<T>* RemoveAt(int index) override { throw NotImplementedException(); }
    Sequence<T>* Concat(Sequence<T>* other) override { throw NotImplementedException(); }
    Sequence<T>* Map(T (*f)(const T&)) const override { throw NotImplementedException(); }
    Sequence<T>* Where(bool (*predicate)(const T&)) const override { throw NotImplementedException(); }

    IEnumerator<T>* GetEnumerator() const override {
        return cache->GetEnumerator();
    }
};

#endif // LAZY_SEQUENCE_H