#include <iostream>
#include <string>
#include <sstream>
#include <cctype>
#include <algorithm> //Для min и max
#include <cstdlib>   //Для exit

#include "turing_tape.h"
#include "turing_machine.h"
#include "lazy_sequence.h"
#include "lazy_rules.h"

//Безопасное чтение строки 
std::string SafeGetLine() 
{
    std::string line;
    if (!std::getline(std::cin, line)) 
    {
        std::cout << "\nПоток ввода закрыт. Завершение работы\n";
        std::exit(0);
    }
    return line;
}

//Функция для красивой отрисовки ленты
void PrintMachineState(const TuringMachine& tm, const TuringTape& tape, int minIdx, int maxIdx) 
{
    int head = tm.GetHeadPosition();
    //Расширяем область видимости на 2 символа влево и вправо от крайних точек или головки
    int printMin = std::min(minIdx, head) - 2;
    int printMax = std::max(maxIdx, head) + 2;

    std::cout << "Лента: ... | ";
    for (int i = printMin; i <= printMax; ++i) 
    {
        std::cout << tape.ReadSymbol(i) << " | ";
    }
    std::cout << "...\n";

    std::cout << "             "; 
    for (int i = printMin; i <= printMax; ++i) 
    {
        if (i == head) 
        {
            std::cout << "^   "; 
        } else 
        {
            std::cout << "    ";
        }
    }
    std::cout << "\nСостояние: q" << tm.GetCurrentState() << "\n";
    std::cout << "--------------------------------------------------\n";
}

//Вспомогательная функция для парсинга направления
Direction ParseDirection(char d) 
{
    if (d == 'L') 
    {
        return Direction::Left;
    }
    if (d == 'R') 
    {
        return Direction::Right;
    }
    return Direction::Stay; 
}

//Демо-режим 
void RunDemo() 
{
    std::cout << "\n=== ДЕМОНСТРАЦИЯ: Инверсия бинарной строки ===\n\n";
    
    Sequence<char>* posSeq = new LazySequence<char>(new ConstantRule<char>('_'));
    Sequence<char>* negSeq = new LazySequence<char>(new ConstantRule<char>('_'));
    
    TuringTape tape(posSeq, negSeq, '_');
    
    const std::string input = "1011001";
    for (size_t i = 0; i < input.length(); ++i) 
    {
        tape.WriteSymbol(i, input[i]);
    }

    TuringMachine tm(&tape, 0, 1);

    tm.AddRule(0, '0', 0, '1', Direction::Right);
    tm.AddRule(0, '1', 0, '0', Direction::Right);
    tm.AddRule(0, '_', 1, '_', Direction::Stay);

    int currentMinIdx = 0;
    int currentMaxIdx = input.length() > 0 ? input.length() - 1 : 0;

    std::cout << "Начальное состояние:\n";
    PrintMachineState(tm, tape, currentMinIdx, currentMaxIdx);

    int stepCount = 0;
    while (tm.Step()) 
    {
        stepCount++;
        currentMinIdx = std::min(currentMinIdx, tm.GetHeadPosition());
        currentMaxIdx = std::max(currentMaxIdx, tm.GetHeadPosition());
        
        std::cout << "Шаг " << stepCount << ":\n";
        PrintMachineState(tm, tape, currentMinIdx, currentMaxIdx);
    }
    std::cout << "Машина остановилась. Всего шагов: " << stepCount << "\n\n";
}

// Ручной режим
void RunManualMode() 
{
    std::cout << "\n=== РУЧНОЙ ВВОД МАШИНЫ ТЬЮРИНГА ===\n";
    
    char blankSymbol;
    while (true) 
    {
        std::cout << "1. Введите пустой символ (ровно 1 символ, обычно _): ";
        std::string line = SafeGetLine();
        std::istringstream iss(line);
        std::string token, garbage;
        
        if (iss >> token && token.length() == 1 && !(iss >> garbage)) 
        {
            blankSymbol = token[0];
            break;
        }
        std::cout << "Ошибка: введите ровно один символ (без пробелов и доп. текста)\n";
    }

    std::string startWord;
    while (true) 
    {
        std::cout << "2. Введите начальное слово (без пробелов, или нажмите Enter для пустой ленты): ";
        std::string line = SafeGetLine();
        std::istringstream iss(line);
        std::string garbage;
        
        if (!(iss >> startWord)) 
        {
            startWord = ""; //Пол-ь просто нажал Enter, лента пуста
            break;
        }
        
        if (iss >> garbage) 
        {
            std::cout << "Ошибка: слово не должно содержать пробелов\n";
            continue;
        }
        break; 
    }

    int startState;
    while (true) 
    {
        std::cout << "3. Введите номер начального состояния (например, 0): ";
        std::string line = SafeGetLine();
        std::istringstream iss(line);
        std::string garbage;
        
        if (iss >> startState && !(iss >> garbage)) 
        {
            break;
        }
        std::cout << "Ошибка: ожидается ровно одно целое число\n";
    }

    int termState;
    while (true) 
    {
        std::cout << "4. Введите номер терминального состояния (например, -1): ";
        std::string line = SafeGetLine();
        std::istringstream iss(line);
        std::string garbage;
        
        if (iss >> termState && !(iss >> garbage)) 
        {
            break;
        }
        std::cout << "Ошибка: ожидается ровно одно целое число\n";
    }

    //Настраиваем ленивую ленту
    Sequence<char>* posSeq = new LazySequence<char>(new ConstantRule<char>(blankSymbol));
    Sequence<char>* negSeq = new LazySequence<char>(new ConstantRule<char>(blankSymbol));
    TuringTape tape(posSeq, negSeq, blankSymbol);

    //Записываем слово на ленту
    for (size_t i = 0; i < startWord.length(); ++i) 
    {
        tape.WriteSymbol(i, startWord[i]);
    }

    TuringMachine tm(&tape, startState, termState);

    std::cout << "\n=== ВВОД ПРАВИЛ ПЕРЕХОДОВ ===\n";
    std::cout << "Формат: [Текущ Сост] [Символ] [Нов Сост] [Нов Символ] [Сдвиг L/R/S]\n";
    std::cout << "Пример: 0 a 1 b R\n";
    std::cout << "Введите -1 для завершения\n\n";

    while (true) 
    {
        std::cout << "Правило: ";
        std::string line = SafeGetLine();
        
        if (line.empty()) 
        {
            continue; 
        }

        std::istringstream iss(line);
        
        int currState;
        if (!(iss >> currState)) 
        {
            std::cout << "Ошибка: строка должна начинаться с числа\n";
            continue;
        }

        if (currState == -1) 
        {
            break; 
        }

        char readSym, writeSym, dirChar;
        int nextState;
        std::string extraGarbage; 
        
        if (!(iss >> readSym >> nextState >> writeSym >> dirChar)) 
        {
            std::cout << "Ошибка: вы ввели не все параметры или ошиблись типом\n";
            continue;
        }

        if (iss >> extraGarbage) 
        {
            std::cout << "Ошибка: обнаружены лишние параметры ('" << extraGarbage << "')\n";
            continue;
        }

        char upperDir = std::toupper(dirChar); 
        std::string validDirs = "LRS";
        
        if (validDirs.find(upperDir) == std::string::npos) 
        {
            std::cout << "Ошибка: неизвестное направление сдвига ('" << dirChar 
                      << "'). Используйте только L, R или S\n";
            continue;
        }
        
        tm.AddRule(currState, readSym, nextState, writeSym, ParseDirection(dirChar));
    }

    int currentMinIdx = 0;
    int currentMaxIdx = startWord.length() > 0 ? startWord.length() - 1 : 0;

    std::cout << "\nЗапуск вычислений...\n";
    std::cout << "Начальное состояние:\n";
    PrintMachineState(tm, tape, currentMinIdx, currentMaxIdx);

    int stepCount = 0;
    while (tm.Step()) 
    {
        stepCount++;
        currentMinIdx = std::min(currentMinIdx, tm.GetHeadPosition());
        currentMaxIdx = std::max(currentMaxIdx, tm.GetHeadPosition());
        
        std::cout << "Шаг " << stepCount << ":\n";
        PrintMachineState(tm, tape, currentMinIdx, currentMaxIdx);
        
        if (stepCount > 1000) 
        {
            std::cout << "Превышен лимит в 1000 шагов. Возможно, машина зациклилась\n";
            break;
        }
    }
    std::cout << "Вычисление завершено. Шагов: " << stepCount << "\n\n";
}

int main() 
{
    while (true) 
    {
        std::cout << "=================================\n";
        std::cout << "      МАШИНА ТЬЮРИНГА (АТД)      \n";
        std::cout << "=================================\n";
        std::cout << "1. Запустить демо (Инверсия)\n";
        std::cout << "2. Ручной ввод правил и ленты\n";
        std::cout << "0. Выход\n";
        std::cout << "Выберите действие: ";
        
        std::string line = SafeGetLine();
        std::istringstream iss(line);
        int choice;
        std::string garbage;
        
        if (!(iss >> choice) || (iss >> garbage)) 
        {
            std::cout << "Ошибка: введите ровно одно число (0, 1 или 2)\n";
            continue;
        }

        if (choice == 0)
        {
            std::cout << "Завершение работы\n";
            break;
        } else if (choice == 1) 
        {
            RunDemo();
        } else if (choice == 2) 
        {
            RunManualMode();
        } else 
        {
            std::cout << "Ошибка: неверный выбор. Попробуйте снова\n";
        }
    }

    return 0;
}