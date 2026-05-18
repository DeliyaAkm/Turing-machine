#ifndef LAZY_RULES_H
#define LAZY_RULES_H

#include "igenerator_rule.h"
#include "exceptions.h"

//Генерация константы
template <class T>
class ConstantRule : public IGeneratorRule<T> 
{
private:
    T value;
public:
    ConstantRule(const T& val) : value(val) {}
    
    T Invoke(const Sequence<T>* context) override 
    {
        return value;
    }
};

//Map (Трансформация)
template <class T>
class MapRule : public IGeneratorRule<T> 
{
private:
    const Sequence<T>* original;
    T (*mapper)(const T&);
public:
    MapRule(const Sequence<T>* orig, T (*f)(const T&)) : original(orig), mapper(f) {}
    
    T Invoke(const Sequence<T>* context) override 
    {
        return mapper(original->Get(context->GetLength()));
    }
};

//Where (Фильтрация)
template <class T>
class WhereRule : public IGeneratorRule<T> 
{
private:
    const Sequence<T>* original;
    bool (*predicate)(const T&);
    int currentIndex; 
public:
    WhereRule(const Sequence<T>* orig, bool (*pred)(const T&)) 
        : original(orig), predicate(pred), currentIndex(0) {}
    
    T Invoke(const Sequence<T>* context) override 
    {
        while (true) 
        {
            T item = original->Get(currentIndex++);
            if (predicate(item)) 
            {
                return item;
            }
        }
    }
};

//Concat (Конкатенация)
template <class T>
class ConcatRule : public IGeneratorRule<T> 
{
private:
    const Sequence<T>* first;
    Sequence<T>* second;
public:
    ConcatRule(const Sequence<T>* f, Sequence<T>* s) : first(f), second(s) {}

    T Invoke(const Sequence<T>* context) override 
    {
        int currIndex = context->GetLength();
        try 
        {
            return first->Get(currIndex);
        } catch (const IndexOutOfRangeException&) 
        {
            int firstLen = first->GetLength();
            return second->Get(currIndex - firstLen);
        }
    }
};

#endif // LAZY_RULES_H