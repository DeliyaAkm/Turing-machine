#ifndef IGENERATOR_RULE_H
#define IGENERATOR_RULE_H

template <class T> class Sequence; 

template <class T>
class IGeneratorRule 
{
public:
    virtual ~IGeneratorRule() = default;
    
    //Правило получает историю только в момент вызова
    virtual T Invoke(const Sequence<T>* context) = 0;
};

#endif // IGENERATOR_RULE_H