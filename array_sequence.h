#ifndef ARRAY_SEQUENCE_H
#define ARRAY_SEQUENCE_H

#include "sequence.h"
#include "dynamic_array.h"

template <class T> class ArraySequenceEnumerator; 

template <class T>
class ArraySequenceBase : public Sequence<T> 
{
private:
    template <class U>
    friend class ArraySequenceBuilder;

protected:
    DynamicArray<T>* items;

    virtual Sequence<T>* instance(const T* arr, int count) const = 0;

    // Вс-ый метод cоздать tempArray
    Sequence<T>* buildFromArray(T* arr, int count) const
    {
        Sequence<T>* result = nullptr;
        try 
        {
            result = instance(arr, count);
        } catch (...) 
        {
        delete[] arr;
        throw;
    }
    delete[] arr;
    return result;
    }
    
public:
    using Sequence<T>::GetFirst;  
    using Sequence<T>::GetLast;

    //Конструкторы
    ArraySequenceBase(const T* items, int count) : items(new DynamicArray<T>(items, count)) {}
    ArraySequenceBase() : items(new DynamicArray<T>(0)) {}
    ArraySequenceBase(const ArraySequenceBase<T>& other) : items(new DynamicArray<T>(*other.items)) {}

    ArraySequenceBase<T>& operator=(const ArraySequenceBase<T>& other) 
    {
    if (this != &other) 
    {
        delete items;
        items = new DynamicArray<T>(*other.items);
    }
    return *this;
    }

    //Деструктор
    ~ArraySequenceBase() override 
    {
        delete items;
    }

    //Декомпозиция 
    const T& GetFirst() const override 
    {   
        if (items->GetSize() == 0)
        {
            throw IndexOutOfRangeException("ArraySequence::GetFirst — sequence is empty");
        }
            return items->Get(0);
    }

    const T& GetLast() const override 
    {
        if (items->GetSize() == 0)
        {
            throw IndexOutOfRangeException("ArraySequence::GetLast — sequence is empty");
        }
            return items->Get(items->GetSize() - 1);
    }

    const T& Get(int index) const override
    {
        return items->Get(index);
    }

    int GetLength() const override 
    {
        return items->GetSize();
    }

    IEnumerator<T>* GetEnumerator() const override
    {
        return new ArraySequenceEnumerator<T>(this);
    }

    const T& operator[](int index) const
    {
        return items->Get(index);
    }

    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override 
    {
        if (startIndex < 0 || endIndex >= GetLength() || startIndex > endIndex) 
        {
            throw IndexOutOfRangeException();
        }
        
        int subLength = endIndex - startIndex + 1;
        T* tempArray = new T[subLength];
        for (int i = startIndex; i <= endIndex; ++i)
        {
            tempArray[i - startIndex] = this->Get(i);
        }

        return buildFromArray(tempArray, subLength);
    }

    Sequence<T>* Append(const T& item) override 
    {
        return InsertAt(item, GetLength());
    }

    Sequence<T>* Prepend(const T& item) override 
    {
        return InsertAt(item, 0);
    }

    Sequence<T>* InsertAt(const T& item, int index) override
    {
        if (index < 0 || index > GetLength())
        {
            throw IndexOutOfRangeException();
        }
        const int oldLen = GetLength();
        T* tempArray = new T[oldLen + 1];

        for (int i = 0; i < index; ++i)
        {
            tempArray[i] = this->Get(i);
        }

        tempArray[index] = item;

        for (int i = index; i < oldLen; ++i)
        {    
            tempArray[i + 1] = this->Get(i);
        }
        return buildFromArray(tempArray, oldLen + 1);
    }

    Sequence<T>* RemoveAt(int index) override 
    {
        if (index < 0 || index >= GetLength()) 
        {
            throw IndexOutOfRangeException();
        }
        
        int oldLen = GetLength();
        T* tempArray = new T[oldLen - 1];
        for (int i = 0, j = 0; i < oldLen; ++i)
        {
            if (i != index) 
            {
                tempArray[j++] = this->Get(i);
            }
        }
        return buildFromArray(tempArray, oldLen - 1);
    }

    Sequence<T>* Concat(const Sequence<T>* other) override 
    {
        if (!other)
        {
            throw InvalidArgumentException();
        }

        const int len = GetLength();       
        const int otherLen = other->GetLength();

        T* tempArray = new T[len + otherLen];
        for (int i = 0; i < len; ++i)
        {
            tempArray[i] = this->Get(i);
        }
        for (int i = 0; i < otherLen; ++i)
        {
            tempArray[len + i] = other->Get(i);
        }
        return buildFromArray(tempArray, len + otherLen);
    }

    Sequence<T>* Map(T (*f)(const T&)) const override 
    {
        int len = GetLength();
        if (len == 0) 
        {
            return instance(nullptr, 0);
        }

        T* tempArray = new T[len];

        for (int i = 0; i < len; ++i)
        {
            tempArray[i] = f(this->Get(i));
        }
        return buildFromArray(tempArray, len);
    }

    Sequence<T>* Where(bool (*predicate)(const T&)) const override 
    {
        const int len = GetLength();
        int count = 0;

        for (int i = 0; i < len; ++i)
        {
            if (!predicate || predicate(this->Get(i))) 
            {
                ++count;
            }
        }

        if (count == 0) 
        {
            return instance(nullptr, 0);
        }

        T* tempArray = new T[count];

        int j = 0;
        for (int i = 0; i < len; ++i)
        {
            const T& item = this->Get(i);
            if (!predicate || predicate(item))
            {
                tempArray[j++] = item;
            }     
        }
        return buildFromArray(tempArray, count);
        }
};

//Mutable версия 
template <class T>
class MutableArraySequence : public ArraySequenceBase<T> 
{
protected:
    using ArraySequenceBase<T>::items;

    Sequence<T>* instance(const T* arr, int count) const override 
    {
        return new MutableArraySequence<T>(arr, count);
    }

public:
    using ArraySequenceBase<T>::GetLength;
    MutableArraySequence(const T* items, int count) : ArraySequenceBase<T>(items, count) {}
    MutableArraySequence() : ArraySequenceBase<T>() {}
    MutableArraySequence(const MutableArraySequence<T>& other) : ArraySequenceBase<T>(other) {}

    //Переопределяем методы для модификации текущего объекта
    Sequence<T>* InsertAt(const T& item, int index) override 
    {
        if (index < 0 || index > GetLength())
        {
            throw IndexOutOfRangeException();
        }

        const int oldLen = GetLength();
        items->Resize(oldLen + 1);

        for (int i = oldLen; i > index; --i)
        {
            items->Set(i, items->Get(i - 1));
        }
        items->Set(index, item);
        return this;
    }

    Sequence<T>* RemoveAt(int index) override 
    {
        if (index < 0 || index >= GetLength()) 
        {
            throw IndexOutOfRangeException();
        }
        
        for (int i = index; i < GetLength() - 1; ++i) 
        {
            items->Set(i, items->Get(i + 1));
        }
        items->Resize(GetLength() - 1);
        return this;
    }

    Sequence<T>* Concat(const Sequence<T>* other) override 
    {
        int otherLen = other->GetLength();
        int currentLen = GetLength();
        items->Resize(currentLen + otherLen);

        for (int i = 0; i < otherLen; ++i) 
        {
            items->Set(currentLen + i, other->Get(i));
        }
        return this;
    }

    T& operator[](int index) 
    {
        return const_cast<T&>(items->Get(index));
    }
};

//Immutable версия
template <class T>
class ImmutableArraySequence : public ArraySequenceBase<T> 
{
protected:
    
    Sequence<T>* instance(const T* arr, int count) const override 
    {
        return new ImmutableArraySequence<T>(arr, count);
    }

public:
    ImmutableArraySequence(const T* items, int count) : ArraySequenceBase<T>(items, count) {}
    ImmutableArraySequence() : ArraySequenceBase<T>() {}
    ImmutableArraySequence(const ImmutableArraySequence<T>& other) : ArraySequenceBase<T>(other) {}
};

template <class T>
class ArraySequenceEnumerator : public IEnumerator<T>
{
private:
    const ArraySequenceBase<T>* seq;
    int index;

public:
    ArraySequenceEnumerator(const ArraySequenceBase<T>* s)
        : seq(s), index(-1) {}

    bool MoveNext() override
    {
        ++index;
        return index < seq->GetLength();
    }

    const T& Current() const override
    {
        if (index < 0 || index >= seq->GetLength())
        {
            throw IndexOutOfRangeException();
        }
        return seq->Get(index);
    }

    void Reset() override { index = -1; }
};

#endif // ARRAY_SEQUENCE_H