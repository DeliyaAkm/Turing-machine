#ifndef GENERATOR_H
#define GENERATOR_H

#include "exceptions.h"
#include "igenerator_rule.h"

template <class T>
class Generator 
{
private:
    IGeneratorRule<T>* rule; 
    int limit; 
    int generatedCount;

public:
    //Конструктор 
    Generator(IGeneratorRule<T>* generationRule, int maxElements = -1)
        : rule(generationRule), limit(maxElements), generatedCount(0) 
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

    //Владелец передает контекст самостоятельно
    T GetNext(const Sequence<T>& context) 
    {
        if (!HasNext()) 
        {
            throw IndexOutOfRangeException("Generator reached its limit");
        }
        
        T nextVal = rule->Invoke(context);
        generatedCount++;
        
        return nextVal;
    }
};

#endif // GENERATOR_H