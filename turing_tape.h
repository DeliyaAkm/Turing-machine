#ifndef TURING_TAPE_H
#define TURING_TAPE_H

#include "array_sequence.h"
#include "option.h"
#include "lazy_sequence.h"
#include "turing_machine.h" 

class TuringTape : public ITuringTape 
{
private:
    char blankSymbol;
    
    const Sequence<char>* positiveSource;
    const Sequence<char>* negativeSource;

    //Кэш записи
    MutableArraySequence<Option<char>> positiveWrites;
    MutableArraySequence<Option<char>> negativeWrites;

    void EnsureCapacity(MutableArraySequence<Option<char>>& half, int requiredIndex) 
    {
        while (half.GetLength() <= requiredIndex) 
        {
            half.Append(Option<char>()); 
        }
    }

public:
    //Tape принимает владение ленивыми последовательностями
    TuringTape(const Sequence<char>* posSrc, const Sequence<char>* negSrc, char blank = '_') 
        : blankSymbol(blank), positiveSource(posSrc), negativeSource(negSrc) {}

    ~TuringTape() override 
    {
        delete positiveSource;
        delete negativeSource;
    }

    char ReadSymbol(int index) const override 
    {
        if (index >= 0) 
        {
            //Проверяем, меняла ли машина этот символ
            if (index < positiveWrites.GetLength() && positiveWrites.Get(index).HasValue()) 
            {
                return positiveWrites.Get(index).GetValue();
            }
            //Иначе запрашиваем у ленивой последовательности
            if (positiveSource) 
            {
                try 
                { 
                    return positiveSource->Get(index); 
                } 
                catch (const IndexOutOfRangeException&) 
                { 
                    return blankSymbol; 
                }
            }
            return blankSymbol; 
        } else 
        {
            int negIndex = -index - 1; 
            if (negIndex < negativeWrites.GetLength() && negativeWrites.Get(negIndex).HasValue()) 
            {
                return negativeWrites.Get(negIndex).GetValue();
            }
            if (negativeSource) 
            {
                try 
                { 
                    return negativeSource->Get(negIndex); 
                } 
                catch (const IndexOutOfRangeException&) 
                { 
                    return blankSymbol; 
                }
            }
            return blankSymbol;
        }
    }

    void WriteSymbol(int index, char symbol) override 
    {
        if (index >= 0) 
        {
            EnsureCapacity(positiveWrites, index);
            positiveWrites[index] = Option<char>(symbol); 
        } else 
        {
            int negIndex = -index - 1;
            EnsureCapacity(negativeWrites, negIndex);
            negativeWrites[negIndex] = Option<char>(symbol);
        }
    }
};

#endif // TURING_TAPE_H