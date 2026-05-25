#include "tests.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

#include "turing_tape.h"
#include "turing_machine.h"
#include "lazy_sequence.h"
#include "lazy_rules.h"


//Фреймворки тестировния
static int totalPassed = 0;
static int totalFailed = 0;

void beginSuite(const std::string& name) 
{
    std::cout << "\n┌─────────────────────────────────────────────\n";
    std::cout << "│  " << name << "\n";
    std::cout << "└─────────────────────────────────────────────\n";
}

void check(const std::string& name, bool condition) 
{
    if (condition) 
    {
        std::cout << "  [ПРОЙДЕН]  " << name << "\n";
        ++totalPassed;
    } else 
    {
        std::cout << "  [ПРОВАЛЕН] " << name << "\n";
        ++totalFailed;
    }
}

template <typename ExceptionType, typename Func>
void checkThrows(const std::string& name, Func func) 
{
    try 
    {
        func();
        std::cout << "  [ПРОВАЛЕН] " << name << " (исключение не брошено)\n";
        ++totalFailed;
    } catch (const ExceptionType&) 
    {
        std::cout << "  [ПРОЙДЕН]  " << name << "\n";
        ++totalPassed;
    } catch (...) 
    {
        std::cout << "  [ПРОВАЛЕН] " << name << " (неверный тип исключения)\n";
        ++totalFailed;
    }
}

void printSummary() 
{
    std::cout << "\n═══════════════════════════════════════════════\n";
    std::cout << "  ИТОГО: пройдено " << totalPassed
              << ", провалено " << totalFailed
              << ", всего " << (totalPassed + totalFailed) << "\n";
    std::cout << "═══════════════════════════════════════════════\n";
}

//Утилиты для визуализации
void PrintLazyTrace(Sequence<int>* seq, const std::string& label) 
{
    std::cout << "    > " << label << ": [ ";
    for (int i = 0; i < 15; ++i) 
    {
        try 
        {
            std::cout << seq->Get(i) << " ";
        } 
        catch (const IndexOutOfRangeException&)
        {
            break;
        }
    }
    std::cout << "]\n";
}

void PrintTestTape(const TuringTape& tape, int minIdx, int maxIdx, int headPos, int state) 
{
    std::cout << "    > [q" << state << "] Лента: ...";
    for (int i = minIdx; i <= maxIdx; ++i) 
    {
        if (i == headPos) 
        {
            std::cout << "[" << tape.ReadSymbol(i) << "]";
        }else 
        {
            std::cout << " " << tape.ReadSymbol(i) << " ";
        }
    }
    std::cout << "...\n";
}

//Функции-помощники
int TestSquare(const int& x) 
{ 
    return x * x; 
}
bool TestIsEven(const int& x) 
{ 
    return x % 2 == 0; 
}
int TestMinusOne(const int& x) 
{ 
    return x - 1; 
}

//Тесты ленивой последовательности
void testLazySequenceOperations() 
{
    beginSuite("LazySequence: Демонстрация всех операций");
    
    int data[] = {10, 20, 30};
    Sequence<int>* baseSeq = new LazySequence<int>(nullptr, 3, data);
    PrintLazyTrace(baseSeq, "Исходная последовательность");

    //Append 
    Sequence<int>* appended = baseSeq->Append(40);
    PrintLazyTrace(appended, "После Append(40)");
    check("Append: элемент добавлен в конец", appended->Get(3) == 40);

    //Prepend
    Sequence<int>* prepended = appended->Prepend(0);
    PrintLazyTrace(prepended, "После Prepend(0)");
    check("Prepend: элемент сместил остальные", prepended->Get(0) == 0 && prepended->Get(1) == 10);

    //InsertAt
    Sequence<int>* inserted = prepended->InsertAt(25, 3);
    PrintLazyTrace(inserted, "После InsertAt(25, инд. 3)");
    check("InsertAt: вставка корректна", inserted->Get(3) == 25 && inserted->Get(4) == 30);

    //RemoveAt
    Sequence<int>* removed = inserted->RemoveAt(1); 
    PrintLazyTrace(removed, "После RemoveAt(инд. 1)");
    check("RemoveAt: элемент удален", removed->Get(1) == 20 && removed->Get(2) == 25);

    //Map и Where
    Sequence<int>* mapped = removed->Map(TestSquare);
    PrintLazyTrace(mapped, "После Map (x^2)");
    
    Sequence<int>* filtered = mapped->Where(TestIsEven);
    PrintLazyTrace(filtered, "После Where (четные)");
    check("Map и Where работают в цепочке", filtered->Get(0) == 0 && filtered->Get(1) == 400);

    //Concat
    int data2[] = {99, 100};
    Sequence<int>* seq2 = new LazySequence<int>(nullptr, 2, data2);
    Sequence<int>* concated = filtered->Concat(seq2);
    PrintLazyTrace(concated, "После Concat( [99, 100] )");
    check("Concat: слияние ленивых списков", concated->Get(4) == 99);
 
    delete concated;
}

void testLazySequenceExceptions() 
{
    beginSuite("LazySequence: исключения и границы памяти");
    
    Sequence<int>* emptySeq = new LazySequence<int>(nullptr, 0, nullptr);
    checkThrows<IndexOutOfRangeException>("Get(0) на пустой последовательности", [&]
    { 
        emptySeq->Get(0); 
    });

    int data[] = {10, 20};
    Sequence<int>* baseSeq = new LazySequence<int>(nullptr, 2, data);
    checkThrows<IndexOutOfRangeException>("Защита от отрицательного индекса Get(-5)", [&]
    { 
        baseSeq->Get(-5); 
    });
    checkThrows<IndexOutOfRangeException>("Выход за пределы справа Get(10)", [&]
    { 
        baseSeq->Get(10); 
    });

    delete emptySeq; 
    delete baseSeq;
}

//Тесты Машины Тьюринга
void testTuringMachineAlgorithm() 
{
    beginSuite("TuringMachine: алгоритм (AKMALOVA -> DELIYA)");
    
    TuringTape tape('_');
    const char* startWord = "AKMALOVA";
    for(int i = 0; startWord[i] != '\0'; i++) 
    {
        tape.WriteSymbol(i, startWord[i]);
    }

    TuringMachine tm(&tape, 0, 100);
    tm.AddRule(0, 'A', 1, 'D', Direction::Right);
    tm.AddRule(1, 'K', 2, 'E', Direction::Right);
    tm.AddRule(2, 'M', 3, 'L', Direction::Right);
    tm.AddRule(3, 'A', 4, 'I', Direction::Right);
    tm.AddRule(4, 'L', 5, 'Y', Direction::Right);
    tm.AddRule(5, 'O', 6, 'A', Direction::Right);
    tm.AddRule(6, 'V', 7, '_', Direction::Right); 
    tm.AddRule(7, 'A', 100, '_', Direction::Stay); 

    while(tm.Step()) {}
    //Проверка результата
    bool success = (tape.ReadSymbol(0) == 'D' && 
                    tape.ReadSymbol(1) == 'E' && 
                    tape.ReadSymbol(2) == 'L' && 
                    tape.ReadSymbol(3) == 'I' && 
                    tape.ReadSymbol(4) == 'Y' && 
                    tape.ReadSymbol(5) == 'A' && 
                    tape.ReadSymbol(6) == '_' && 
                    tape.ReadSymbol(7) == '_');
                    
    check("Алгоритм завершен корректно", success);
}

void testTuringMachineEdgeCases() 
{
    beginSuite("TuringMachine: граничные случаи и защиты");

    //Защита от бесконечного цикла 
    TuringTape tape('_');
    TuringMachine tm(&tape, 0, 100);
    tm.AddRule(0, '_', 1, '_', Direction::Right);
    tm.AddRule(1, '_', 0, '_', Direction::Left);

    int steps = 0;
    const int MAX_STEPS = 1000;
    while(tm.Step() && steps < MAX_STEPS) 
    {
        steps++;
    }
    check("Прерывание бесконечного цикла", steps == MAX_STEPS && tm.GetCurrentState() != 100);

    //Пустая лента
    TuringTape emptyTape('_');
    TuringMachine emptyTm(&emptyTape, 0, 10);
    bool didStep = emptyTm.Step();
    check("Корректная остановка на абсолютно пустой ленте", !didStep);
}

void testTuringMachineFuzzing() 
{
    beginSuite("TuringMachine: cтресс-тест мусором");
    
    std::srand(std::time(nullptr)); 
    bool crashed = false;
    
    try 
    {
        TuringTape tape('_');
        TuringMachine tm(&tape, 0, 999);

        //Генерация 100 случайных правил
        for (int i = 0; i < 100; i++) 
        {
            int fromState = std::rand() % 10;
            char readSym = (std::rand() % 2 == 0) ? '_' : char('A' + (std::rand() % 26));
            int toState = std::rand() % 10;
            char writeSym = char('a' + (std::rand() % 26));
            Direction dir = (std::rand() % 2 == 0) ? Direction::Left : Direction::Right;
            tm.AddRule(fromState, readSym, toState, writeSym, dir);
        }

        int steps = 0;
        while(tm.Step() && steps < 500) 
        {
            steps++;
        }
    }catch (...) 
    {
        crashed = true;
    }

    check("Программа выдерживает хаотичные правила без падения памяти", !crashed);
}

//Главный запуск
void RunAllTests() 
{
    std::cout << "\n═══════════════════════════════════════════════\n";
    std::cout << "   АВТОМАТИЧЕСКИЕ ТЕСТЫ: TURING & LAZY SEQ\n";
    std::cout << "═══════════════════════════════════════════════\n";
    
    totalPassed = 0;
    totalFailed = 0;

    //Ленивые последовательности
    testLazySequenceOperations();
    testLazySequenceExceptions();

    //Машина Тьюринга
    testTuringMachineAlgorithm();
    testTuringMachineEdgeCases();
    testTuringMachineFuzzing();

    printSummary();
}