#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

class BaseLabException : public std::exception
{
private:
    std::string message;
    
public:
    explicit BaseLabException(const std::string& msg = "Exception") : message(msg) {}
    virtual ~BaseLabException() {}
    virtual const char* what() const noexcept override 
    { 
        return message.c_str(); 
    }
};

class IndexOutOfRangeException : public BaseLabException 
{
public:
    explicit IndexOutOfRangeException(const std::string& msg = "Index out of range") 
        : BaseLabException(msg) {}
};

class EmptyStructureException : public BaseLabException 
{
public:
    explicit EmptyStructureException(const std::string& msg = "Operation is not allowed on an empty structure") 
        : BaseLabException(msg) {}
};

class InvalidArgumentException : public BaseLabException 
{
public:
    explicit InvalidArgumentException(const std::string& msg = "Invalid argument provided") 
        : BaseLabException(msg) {}
};

class MemoryAllocationException : public BaseLabException 
{
public:
    explicit MemoryAllocationException(const std::string& msg = "Failed to allocate memory") 
        : BaseLabException(msg) {}
};

class InvalidOperationException : public BaseLabException
{
public:
    explicit InvalidOperationException(const std::string& msg = "Invalid operation")
        : BaseLabException(msg) {}
};

class ObjectDisposedException : public BaseLabException
{
public:
    explicit ObjectDisposedException(const std::string& msg = "Object has been disposed")
        : BaseLabException(msg) {}
};

//Расширенное исключение с информацией о файле, строке и методе
class DetailedIndexOutOfRangeException : public IndexOutOfRangeException
{
private:
    static std::string intToStr(int n)
    {
        if (n == 0) 
        {
            return "0";
        }

        bool negative = (n < 0);

        if (negative) 
        {
            n = -n;
        }

        std::string result;

        while (n > 0)
        {
            result = (char)('0' + n % 10) + result;
            n /= 10;
        }

        if (negative) 
        {
            result = "-" + result;
        }
        
        return result;
    }

public:
    DetailedIndexOutOfRangeException(int index, int minVal, int maxVal,
        const std::string& method, const char* file, int line)
        : IndexOutOfRangeException("Index " + intToStr(index) +" out of range [" + 
        intToStr(minVal) + ", " + intToStr(maxVal) + "]" +" in method '" + method + "'" +
            " at " + std::string(file) + ":" + intToStr(line))
    {}
};
//Макрос для  исключения с указанием файла и строки через __FILE__ и __LINE__
#define THROW_DETAILED_INDEX_ERROR(index, min, max, method) \
    throw DetailedIndexOutOfRangeException(index, min, max, method, __FILE__, __LINE__)
    
#endif // EXCEPTIONS_H