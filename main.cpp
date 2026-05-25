#include <iostream>
#include <cstdio>
#include <windows.h>

#include "turing_tape.h"
#include "turing_machine.h"
#include "lazy_sequence.h"
#include "lazy_rules.h"
#include "tests.h"

//Утилиты 
int GetMin(int a, int b) 
{ 
    return (a < b) ? a : b;
}
int GetMax(int a, int b) 
{ 
    return (a > b) ? a : b; 
}

int StringLen(const char* str) 
{
    int len = 0;
    while (str[len] != '\0') len++;
    return len;
}

void ClearInputBuffer() 
{
    std::cin.clear();
    while (std::cin.get() != '\n');
}

//Функция для строгого ввода чисел 
int ReadInt(const char* prompt) 
{
    int val;
    while (true) 
    {
        std::cout << prompt;
        if (std::cin >> val) 
        {
            //Что пользователь ввел сразу после числа
            char nextChar = std::cin.get();
            if (nextChar == '\n') 
            {
                return val;
            } else if (nextChar == ' ' || nextChar == '\t') //Если поставил пробелы, но само число корректное
            {
                while (std::cin.get() != '\n'); //Вычищаем остаток
                return val;
            } else //Слипшийся мусор
            {
                std::cout << "Ошибка ввода. Состояние должно быть целым числом без букв\n";
                while (std::cin.get() != '\n'); //Вычищаем мусор из буфера
                continue; 
            }
        }
        //Если ввели вообще не цифру
        std::cout << "Ошибка ввода. Ожидается число\n";
        std::cin.clear(); 
        while (std::cin.get() != '\n');
    }
}

//Универсальная функция для безопасного ввода символа
char ReadChar(const char* prompt) 
{
    char val;
    while (true) 
    {
        std::cout << prompt;
        //Пытаемся считать один символ
        if (std::cin >> val) 
        {
            char nextChar = std::cin.get();
            if (nextChar == '\n') 
            {
                return val;
            } else if (nextChar == ' ' || nextChar == '\t') 
            {
                bool hasGarbage = false;
                char c;
                while ((c = std::cin.get()) != '\n') 
                {
                    if (c != ' ' && c != '\t') 
                    {
                        hasGarbage = true;
                    }
                }
                if (!hasGarbage) 
                {
                    return val; 
                }
            } else 
            {
                //Ввели целое слово
                while (std::cin.get() != '\n');
            }
        }else 
        {
            //Ошибка потока
            std::cin.clear();
            while (std::cin.get() != '\n');
        }
        std::cout << "Ошибка ввода. Ожидается ровно один символ (без лишних букв)\n";
    }
}

//Функции отрисовки
void PrintMachineState(const TuringMachine& tm, const TuringTape& tape, int minIdx, int maxIdx) 
{
    int head = tm.GetHeadPosition();
    int printMin = GetMin(minIdx, head) - 2;
    int printMax = GetMax(maxIdx, head) + 2;

    std::cout << "Лента: ... | ";
    for (int i = printMin; i <= printMax; ++i) 
    {
        std::cout << tape.ReadSymbol(i) << " | ";
    }
    std::cout << "...\n             "; 
    for (int i = printMin; i <= printMax; ++i) 
    {
        if (i == head) 
        {
            std::cout << "^   "; 
        }else 
        {
            std::cout << "    ";
        }
    }
    std::cout << "\nСостояние: q" << tm.GetCurrentState() << "\n";
    std::cout << "--------------------------------------------------\n";
}

//Для перемещения курсора вверх
void MoveCursorUp(int lines) 
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    COORD pos = csbi.dwCursorPosition;
    pos.Y -= lines;
    if (pos.Y < 0) 
    {
        pos.Y = 0;
    }
    SetConsoleCursorPosition(hConsole, pos);
}

Direction ParseDirection(char d) 
{
    if (d == 'L' || d == 'l') 
    {
        return Direction::Left;
    }
    if (d == 'R' || d == 'r') 
    {
        return Direction::Right;
    }
    return Direction::Stay; 
}

//Функции для тестирования ленивой последовательности
bool IsEven(const int& x) 
{ 
    return x % 2 == 0; 
}
int MultiplyByTen(const int& x) 
{ 
    return x * 10; 
}

void AnimateTuringMachine(TuringMachine& tm, TuringTape& tape, int currentMinIdx, int currentMaxIdx, int limitSteps = 1000) 
{
    std::cout << "Начальное состояние:\n";
    PrintMachineState(tm, tape, currentMinIdx, currentMaxIdx);

    int stepCount = 0;
    while (tm.Step()) 
    {
        stepCount++;
        //Расширяем границы отрисовки, если каретка ушла за края
        currentMinIdx = GetMin(currentMinIdx, tm.GetHeadPosition());
        currentMaxIdx = GetMax(currentMaxIdx, tm.GetHeadPosition());
        
        Sleep(200); 
        MoveCursorUp(4); 
        PrintMachineState(tm, tape, currentMinIdx, currentMaxIdx);
        
        //Защита от зависания программы при кривых правилах
        if (stepCount > limitSteps) 
        {
            std::cout << "\nПревышен лимит в " << limitSteps << " шагов (возможно, бесконечный цикл)\n";
            break; 
        }
    }
    std::cout << "Вычисление завершено. Всего шагов: " << stepCount << "\n\n";
}

//Функция для правил инверсии
void SetupInversionRules(TuringMachine& tm) 
{
    tm.AddRule(0, '0', 0, '1', Direction::Right);
    tm.AddRule(0, '1', 0, '0', Direction::Right);
    tm.AddRule(0, '_', 1, '_', Direction::Stay);
}

// Функция для правил AKMALOVA -> DELIYA
void SetupAkmalovaRules(TuringMachine& tm) 
{
    tm.AddRule(0, 'A', 1, 'D', Direction::Right);
    tm.AddRule(1, 'K', 2, 'E', Direction::Right);
    tm.AddRule(2, 'M', 3, 'L', Direction::Right);
    tm.AddRule(3, 'A', 4, 'I', Direction::Right);
    tm.AddRule(4, 'L', 5, 'Y', Direction::Right);
    tm.AddRule(5, 'O', 6, 'A', Direction::Right);
    tm.AddRule(6, 'V', 7, '_', Direction::Right); 
    tm.AddRule(7, 'A', 100, '_', Direction::Stay); 
}

void RunTuringDemoBase(const char* title, const char* input, int termState, void (*setupRules)(TuringMachine&)) 
{
    std::cout << "\n=== ДЕМОНСТРАЦИЯ: " << title << " ===\n\n";
    
    TuringTape tape('_');
    int len = StringLen(input);
    for (int i = 0; i < len; ++i) 
    {
        tape.WriteSymbol(i, input[i]);
    }

    TuringMachine tm(&tape, 0, termState);
    
    //Вызываем функцию, чтобы она заполнила машину правилами
    setupRules(tm);

    int currentMinIdx = 0;
    int currentMaxIdx = len > 0 ? len - 1 : 0;

    AnimateTuringMachine(tm, tape, currentMinIdx, currentMaxIdx);
}

void RunTuringManual() 
{
    std::cout << "\n=== РУЧНОЙ ВВОД МАШИНЫ ТЬЮРИНГА ===\n";
    
    char blankSymbol = ReadChar("1. Введите пустой символ (обычно _): ");
    
    char startWord[256];
    std::cout << "2. Введите начальное слово (без пробелов, _ для пустой ленты): ";
    std::cin >> startWord;
    while (std::cin.get() != '\n'); 

    int startState = ReadInt("3. Введите номер начального состояния: ");
    int termState = ReadInt("4. Введите номер терминального состояния: ");

    TuringTape tape(blankSymbol);
    
    if (startWord[0] != blankSymbol || startWord[1] != '\0') 
    {
        int len = StringLen(startWord);
        for (int i = 0; i < len; ++i) 
        {
            tape.WriteSymbol(i, startWord[i]);
        }
    }

    TuringMachine tm(&tape, startState, termState);

    std::cout << "\n=== ВВОД ПРАВИЛ ПЕРЕХОДОВ ===\n";
    std::cout << "Формат: [Текущ Сост] [Символ] [Нов Сост] [Нов Символ] [Сдвиг L/R/S]\n";
    std::cout << "Введите -1 в качестве состояния для завершения ввода\n";

    while (true) 
    {
        int currState = ReadInt("Текущее состояние (-1 для выхода): ");
        if (currState == -1) 
        {
            break;
        }

        char readSym, writeSym, dirChar;
        int nextState;
        
        std::cout << "Остальные параметры (Символ Нов.Сост Нов.Символ Сдвиг): ";
        if (std::cin >> readSym >> nextState >> writeSym >> dirChar) 
        {
            tm.AddRule(currState, readSym, nextState, writeSym, ParseDirection(dirChar));
        } else 
        {
            std::cout << "Ошибка формата. Попробуйте снова\n";
            std::cin.clear();
        }
        while (std::cin.get() != '\n'); 
    }

    int currentMinIdx = 0;
    int currentMaxIdx = StringLen(startWord);

    std::cout << "\nЗапуск вычислений...\n";
    std::cout << "Начальное состояние:\n";
    PrintMachineState(tm, tape, currentMinIdx, currentMaxIdx); 

    int stepCount = 0;
    while (tm.Step()) 
    {
        stepCount++;
        currentMinIdx = GetMin(currentMinIdx, tm.GetHeadPosition());
        currentMaxIdx = GetMax(currentMaxIdx, tm.GetHeadPosition());
        
        Sleep(300); 
        MoveCursorUp(4);
        PrintMachineState(tm, tape, currentMinIdx, currentMaxIdx);
        
        //Защита от бесконечного цикла в ручном режиме
        if (stepCount > 1000) 
        {
            std::cout << "\nПревышен лимит в 1000 шагов (возможно, бесконечный цикл)\n";
            break; 
        }
    }
    std::cout << "Вычисление завершено. Всего шагов: " << stepCount << "\n\n";
}

void RunTuringFromFile() 
{
    std::cout << "\n=== ЧТЕНИЕ МАШИНЫ ТЬЮРИНГА ИЗ ФАЙЛА ===\n";
    
    char filename[512]; 
    std::cout << "Введите полный или относительный путь к файлу:\n> ";
    std::cin >> filename;
    ClearInputBuffer();

    FILE* file = fopen(filename, "r");
    if (file == nullptr) 
    {
        std::cout << "Ошибка: Не удалось открыть файл\n";
        std::cout << "Программа пыталась найти: [" << filename << "]\n"; 
        return;
    }

    char blankSymbol;
    char startWord[256];
    int startState, termState;

    if (fscanf(file, " %c %255s %d %d", &blankSymbol, startWord, &startState, &termState) != 4)
    {
        std::cout << "Ошибка: Неверный формат заголовка в файле\n";
        fclose(file);
        return;
    }

    TuringTape tape(blankSymbol);
    if (startWord[0] != blankSymbol || startWord[1] != '\0') 
    {
        int len = StringLen(startWord);
        for (int i = 0; i < len; ++i) 
        {
            tape.WriteSymbol(i, startWord[i]);
        }
    }

    TuringMachine tm(&tape, startState, termState);

    int currState, nextState;
    char readSym, writeSym, dirChar;
    int rulesCount = 0;

    while (fscanf(file, "%d %c %d %c %c", &currState, &readSym, &nextState, &writeSym, &dirChar) == 5) 
    {
        tm.AddRule(currState, readSym, nextState, writeSym, ParseDirection(dirChar));
        rulesCount++;
    }
    
    fclose(file);
    std::cout << "Успешно загружено " << rulesCount << " правил\n\n";
    
    int currentMinIdx = 0;
    int currentMaxIdx = StringLen(startWord);

    AnimateTuringMachine(tm, tape, currentMinIdx, currentMaxIdx, 5000);
}

//Ленивые последовательности
void RunLazyDemo() 
{
    std::cout << "\n=== ДЕМОНСТРАЦИЯ: Ленивые вычисления ===\n";
    
    std::cout << "1. Создаем конечную последовательность: [1, 2, 3, 4, 5]\n";
    int initArr[] = {1, 2, 3, 4, 5};
    Sequence<int>* seq1 = new LazySequence<int>(nullptr, 5, initArr);

    std::cout << "2. Append(6) -> в конец\n";
    Sequence<int>* seq2 = seq1->Append(6);

    std::cout << "3. Prepend(0) -> в начало\n";
    Sequence<int>* seq3 = seq2->Prepend(0);

    std::cout << "4. RemoveAt(2) -> удаляем число 2 (на индексе 2)\n";
    Sequence<int>* seq4 = seq3->RemoveAt(2);

    std::cout << "5. Where(IsEven) -> фильтруем только четные\n";
    Sequence<int>* seq5 = seq4->Where(IsEven);

    std::cout << "6. Map(MultiplyByTen) -> умножаем всё на 10\n";
    Sequence<int>* seq6 = seq5->Map(MultiplyByTen);

    std::cout << "\nВычисляем первые 4 элемента итоговой последовательности:\n";
    for(int i = 0; i < 4; i++) 
    {
        try 
        {
            std::cout << "Элемент " << i << " = " << seq6->Get(i) << "\n";
        } 
        catch (const IndexOutOfRangeException&) 
        {
            std::cout << "Конец последовательности\n";
            break;
        }
    }
    delete seq6; 
}

void RunLazyManual() 
{
    std::cout << "\n=== РУЧНОЙ ВВОД: Ленивая последовательность ===\n";
    std::cout << "Введите начальные числа. Для окончания введите любой нечисловой символ (например 'q'):\n";
    
    int tempArr[100];
    int count = 0;
    while (count < 100 && std::cin >> tempArr[count]) 
    {
        count++;
    }
    ClearInputBuffer(); //Очищаем буфер от нечислового символа завершения

    //Создаем базовую последовательность
    Sequence<int>* currentSeq = new LazySequence<int>(nullptr, count, tempArr);
    
    while (true) 
    {
        std::cout << "\nТекущие операции не изменяют данные, а создают новые правила\n";
        std::cout << "1. Добавить в конец (Append)\n";
        std::cout << "2. Добавить в начало (Prepend)\n";
        std::cout << "3. Удалить по индексу (RemoveAt)\n";
        std::cout << "4. Вставить по индексу (InsertAt)\n";
        std::cout << "5. Умножить все элементы на 10 (Map: x * 10)\n";
        std::cout << "6. Оставить только четные числа (Where: только четные)\n";
        std::cout << "7. Вычислить и показать первые N элементов\n";
        std::cout << "0. Выход\n";
        
        int choice = ReadInt("Выбор: ");
        
        if (choice == 0) 
        {
            break;
        } else if (choice == 1) 
        {
            int item = ReadInt("Введите число: ");
            currentSeq = currentSeq->Append(item);
        } else if (choice == 2) 
        {
            int item = ReadInt("Введите число: ");
            currentSeq = currentSeq->Prepend(item);
        } else if (choice == 3) 
        {
            int index = ReadInt("Введите индекс для удаления: ");
            currentSeq = currentSeq->RemoveAt(index);
        } else if (choice == 4) 
        {
            int item = ReadInt("Введите число для вставки: ");
            int index = ReadInt("Введите индекс для вставки: ");
            currentSeq = currentSeq->InsertAt(item, index);
        } else if (choice == 5) 
        {
            currentSeq = currentSeq->Map(MultiplyByTen);
            std::cout << "Добавлено правило трансформации: умножение на 10\n";
        } else if (choice == 6)
        {
            currentSeq = currentSeq->Where(IsEven);
            std::cout << "Добавлено правило фильтрации: только четные числа\n";
        } else if (choice == 7) 
        {
            int n = ReadInt("Сколько элементов вычислить? ");
            std::cout << "Результат: ";
            for(int i = 0; i < n; i++) 
            {
                try 
                {
                    std::cout << currentSeq->Get(i) << " ";
                } catch (const IndexOutOfRangeException&) 
                {
                    std::cout << "(конец) ";
                    break;
                }
            }
            std::cout << "\n";
        }
    }
    delete currentSeq; 
}

//Главное меню
void TuringMenu() 
{
    while (true) 
    {
        std::cout << "\n--- МАШИНА ТЬЮРИНГА ---\n";
        std::cout << "1. Демонстрация (Инверсия бинарной строки)\n";
        std::cout << "2. Демонстрация (Замена AKMALOVA -> DELIYA)\n";
        std::cout << "3. Ручной ввод правил и ленты\n";
        std::cout << "4. Загрузить конфигурацию из файла\n";
        std::cout << "0. Назад\n";
        
        int choice = ReadInt("Выберите действие: ");
        if (choice == 0) 
        {
            break;
        }if (choice == 1) 
        {
            RunTuringDemoBase("Инверсия бинарной строки", "1011001", 1, SetupInversionRules);
        }else if (choice == 2) 
        {
            RunTuringDemoBase("Замена AKMALOVA -> DELIYA", "AKMALOVA", 100, SetupAkmalovaRules);
        }else if (choice == 3) 
        {
            RunTuringManual();
        }else if (choice == 4) 
        {
            RunTuringFromFile();
        }
    }
}

void LazyMenu() 
{
    while (true) 
    {
        std::cout << "\n--- ЛЕНИВЫЕ ПОСЛЕДОВАТЕЛЬНОСТИ ---\n";
        std::cout << "1. Демонстрация цепочки правил (Map, Where, Insert)\n";
        std::cout << "2. Ручной ввод ленивой последовательности\n";
        std::cout << "0. Назад\n";
        
        int choice = ReadInt("Выберите действие: ");
        if (choice == 0) 
        {
            break;
        }else if (choice == 1) 
        {
            RunLazyDemo();
        }else if (choice == 2) 
        {
            RunLazyManual();
        }
    }
}

int main() 
{
    while (true) 
    {
        std::cout << "\n=================================\n";
        std::cout << "          ГЛАВНОЕ МЕНЮ           \n";
        std::cout << "=================================\n";
        std::cout << "1. Машина Тьюринга\n";
        std::cout << "2. Ленивые последовательности\n";
        std::cout << "3. Запустить тесты (Автоматизированные)\n";
        std::cout << "0. Выход\n";
        
        int choice = ReadInt("Выберите действие: ");
        
        if (choice == 0)
        {
            std::cout << "Завершение работы\n";
            break;
        } else if (choice == 1) 
        {
            TuringMenu();
        } else if (choice == 2) 
        {
            LazyMenu();
        } else if (choice == 3) 
        {
            RunAllTests();
        } else 
        {
            std::cout << "Неверный выбор\n";
        }
    }
    return 0;
}