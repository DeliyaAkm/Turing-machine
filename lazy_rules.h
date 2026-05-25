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
    
    T Invoke(const Sequence<T>& context) override 
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
    
    T Invoke(const Sequence<T>& context) override 
    {
        return mapper(original->Get(context.GetLength()));
    }

    ~MapRule() override 
    {
        delete const_cast<Sequence<T>*>(original);
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
    
    T Invoke(const Sequence<T>& context) override 
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

    ~WhereRule() override 
    {
        delete const_cast<Sequence<T>*>(original);
    }
};

//Concat (Конкатенация)
template <class T>
class ConcatRule : public IGeneratorRule<T> 
{
private:
    const Sequence<T>* first;
    const Sequence<T>* second;
public:
    ConcatRule(const Sequence<T>* f, const Sequence<T>* s) : first(f), second(s) {}

    T Invoke(const Sequence<T>& context) override 
    {
        int currIndex = context.GetLength();
        try 
        {
            return first->Get(currIndex);
        } catch (const IndexOutOfRangeException&) 
        {
            int firstLen = first->GetLength();
            return second->Get(currIndex - firstLen);
        }
    }

    ~ConcatRule() override 
    {
        delete const_cast<Sequence<T>*>(first);
        delete const_cast<Sequence<T>*>(second);
    }
};

enum class ModifyType 
{
    Insert, 
    Remove 
};

//InsertAt/Prepend и RemoveAt
template <class T>
class ModifyRule : public IGeneratorRule<T> 
{
private:
    const Sequence<T>* original;
    T item; //Сохраняем элемент (испо-я для Insert)
    int targetIndex;
    ModifyType type;
    
public:
    //Конструктор для Insert / Prepend
    ModifyRule(const Sequence<T>* orig, const T& it, int target) 
        : original(orig), item(it), targetIndex(target), type(ModifyType::Insert) 
    {
        if (target < 0) 
        {
            throw IndexOutOfRangeException("Index cannot be negative");
        }
    }

    // Конструктор для Remove
    ModifyRule(const Sequence<T>* orig, int target) 
        : original(orig), item(T()), targetIndex(target), type(ModifyType::Remove) 
    {
        if (target < 0) 
        {
            throw IndexOutOfRangeException("Index cannot be negative");
        }
    }
    
    T Invoke(const Sequence<T>& context) override 
    {
        int currIndex = context.GetLength();

        if (type == ModifyType::Insert) 
        {
            try 
            {
                if (currIndex < targetIndex) 
                {
                    return original->Get(currIndex);
                } if (currIndex == targetIndex) 
                {
                    return item; 
                }
                //Если индекс больше целевого - сдвигаем запрос на 1 назад
                return original->Get(currIndex - 1);
            } 
            catch (const IndexOutOfRangeException&) 
            {
                int origLen = original->GetLength();
                if (currIndex == origLen) 
                {
                    return item;
                } 
                //Если запрашивают индексы дальше, то говорим, что это конец
                throw; 
            }
        } else //Remove
        {
            if (currIndex < targetIndex) 
            {
                return original->Get(currIndex);
            }
            return original->Get(currIndex + 1);
        }
    }

    ~ModifyRule() override 
    {
        delete const_cast<Sequence<T>*>(original);
    }
};

template <class T>
class AppendRule : public IGeneratorRule<T> 
{
private:
    const Sequence<T>* original;
    T item;
public:
    AppendRule(const Sequence<T>* orig, const T& it) : original(orig), item(it) {}
    
    T Invoke(const Sequence<T>& context) override 
    {
        int currIndex = context.GetLength();

        try 
        {
            return original->Get(currIndex);
        }
        catch (const IndexOutOfRangeException&) 
        {
            int origLen = original->GetLength();
            if (currIndex == origLen) 
            {
                return item;
            }
            throw; 
        }
    }

    ~AppendRule() override 
    {
        delete const_cast<Sequence<T>*>(original);
    }
};

#endif // LAZY_RULES_H