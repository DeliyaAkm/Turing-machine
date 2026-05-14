#ifndef IGENERATOR_RULE_H
#define IGENERATOR_RULE_H

#include "sequence.h"

template <class T>
class IGeneratorRule 
{
public:
    virtual ~IGeneratorRule() = default;
    //Правило принимает абстрактную последовательность (кэш) для чтения прошлых данных
    virtual T Invoke(const Sequence<T>* previousData) = 0;
};

#endif // IGENERATOR_RULE_H