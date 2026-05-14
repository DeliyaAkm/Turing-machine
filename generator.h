#ifndef GENERATOR_H
#define GENERATOR_H

#include "exceptions.h"
#include "sequence.h"
#include "igenerator_rule.h"

template <class T>
class Generator 
{
private:
    //Генератор знает ТОЛЬКО об абстрактной последовательности, откуда брать историю
    const Sequence<T>* historyCache; 
    
    IGeneratorRule<T>* rule; 
    int limit; 
    int generatedCount;

public:
    //Конструктор принимает кэш и забирает владение правилом
    Generator(const Sequence<T>* cache, IGeneratorRule<T>* generationRule, int maxElements = -1)
        : historyCache(cache), rule(generationRule), limit(maxElements), generatedCount(0) 
        {
        
        if (!rule) 
        {
            throw InvalidArgumentException("Generator rule cannot be null");
        }
    }

    ~Generator() 
    {
        delete rule; 
    }

    bool HasNext() const 
    {
        return limit == -1 || generatedCount < limit;
    }

    T GetNext() 
    {
        if (!HasNext()) 
        {
            throw IndexOutOfRangeException("Generator reached its limit");
        }
        
        //Передаем кэш в правило
        T nextVal = rule->Invoke(historyCache);
        generatedCount++;
        
        return nextVal;
    }
};

#endif // GENERATOR_H