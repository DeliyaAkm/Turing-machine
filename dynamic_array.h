#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include "exceptions.h"
#include "iterator.h"

//Всп-ная функция для выделения памяти
template <class T>
T* allocateMemory(int size) 
{
    if (size < 0) 
    {
        throw InvalidArgumentException();
    }

    if (size == 0)
    {
        return nullptr;
    }

    T* ptr = nullptr;
    try 
    {
        ptr = new T[size]();
    } catch (...) 
    {
        throw MemoryAllocationException();
    }
    
    return ptr;
}

//Всп-я функция для освобождения памяти
template <class T>
void deallocateMemory(T* ptr) 
{
    delete[] ptr;
}

template <class T>
class DynamicArray 
{
private:
    T* data;
    int size;

    void copyFrom(const T* source, int count)
    {
        for (int i = 0; i < count; ++i)
        {
            data[i] = source[i];
        }
    }
    
public:
    DynamicArray(const T* items, int count) : size(count) 
    {
        if (count < 0) 
        {
            throw InvalidArgumentException("Size cannot be negative");
        }
        if (count > 0 && items == nullptr) 
        {
            throw InvalidArgumentException("Null pointer provided");
        }
        data = allocateMemory<T>(count);
        copyFrom(items, count);
    }
    
    //Конструктор создания массива заданной длины
    explicit DynamicArray(int sz) : size(sz) 
    {
        if (sz < 0) 
        {
            throw InvalidArgumentException();
        }
        data = allocateMemory<T>(sz);
    }
    
    //Конструктор копирования
    DynamicArray(const DynamicArray<T>& other) : size(other.size) 
    {
        data = allocateMemory<T>(size);
        if (size > 0) copyFrom(other.data, size);
    }

    // Деструктор
    ~DynamicArray()
    {
        deallocateMemory(data);
    }
    
    // Оператор присваивания
    DynamicArray<T>& operator=(const DynamicArray<T>& other) 
    {
        if (this != &other) 
        {
            T* newData = allocateMemory<T>(other.size);
            if (other.size > 0)
            {
                for (int i = 0; i < other.size; ++i)
                    newData[i] = other.data[i];
            }
            deallocateMemory(data);
            data = newData;
            size = other.size;
        }
        return *this;
    }

    //Декомпозиция (неconst версия вызывает const)
    const T& Get(int index) const 
    {
        if (index < 0 || index >= size)
        {
            throw IndexOutOfRangeException();
        }
        return data[index];
    }
    
    T& Get(int index) 
    {
        return const_cast<T&>(static_cast<const DynamicArray<T>&>(*this).Get(index));
    }

    const T& operator[](int index) const 
    {
        return Get(index); 
    }
    
    T& operator[](int index) 
    {
        return const_cast<T&>(static_cast<const DynamicArray<T>&>(*this)[index]);
    }

    int GetSize() const 
    {
        return size;
    }
    
    void Set(int index, const T& value)
    {
        if (index < 0 || index >= size)
        {
            throw IndexOutOfRangeException();
        }
        data[index] = value;
    }

    void Resize(int newSize)
    {
        if (newSize < 0) 
        {
            throw InvalidArgumentException();
        }
        if (newSize == size) return;
        
        T* newData = allocateMemory<T>(newSize);
        
        int copyCount = (newSize < size) ? newSize : size;
        for (int i = 0; i < copyCount; ++i)
        {
            newData[i] = data[i];
        }
        
        deallocateMemory(data);
        data = newData;
        size = newSize;
    }

    IEnumerator<T>* GetEnumerator() const;
};

//IEnumerator
template <class T>
class DynamicArrayEnumerator : public IEnumerator<T> 
{
private:
    const DynamicArray<T>* array;  
    int currentIndex;               

public:
    explicit DynamicArrayEnumerator(const DynamicArray<T>* arr)
        : array(arr), currentIndex(-1) {}

    DynamicArrayEnumerator(const DynamicArrayEnumerator&) = delete;
    DynamicArrayEnumerator& operator=(const DynamicArrayEnumerator&) = delete;

    bool MoveNext() override
    {
        if (!array)
        {
            throw ObjectDisposedException();
        }
        if (currentIndex + 1 < array->GetSize())
        {
            ++currentIndex;
            return true;
        }
        return false;
    }

    const T& Current() const override
    {
        if (currentIndex < 0 || currentIndex >= array->GetSize())
        {
            throw IndexOutOfRangeException();
        }
        return array->Get(currentIndex);
    }

    void Reset() override { currentIndex = -1; }
};

template <class T>
IEnumerator<T>* DynamicArray<T>::GetEnumerator() const
{
    return new DynamicArrayEnumerator<T>(this);
}

#endif // DYNAMIC_ARRAY_H