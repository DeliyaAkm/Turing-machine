#ifndef TURING_TAPE_H
#define TURING_TAPE_H

#include "array_sequence.h"
#include "turing_machine.h"

class TuringTape : public ITuringTape 
{
private:
    char blankSymbol;
    
    //Два массива для имитации бесконечности в обе стороны
    MutableArraySequence<char> positiveHalf;
    MutableArraySequence<char> negativeHalf;

    //Всп-ный метод для динамического расширения ленты
    void EnsureCapacity(MutableArraySequence<char>& half, int requiredIndex) 
    {
        while (half.GetLength() <= requiredIndex) 
        {
            half.Append(blankSymbol); 
        }
    }

public:
    TuringTape(char blank = '_') : blankSymbol(blank) {}

    char ReadSymbol(int index) const override 
    {
        if (index >= 0) 
        {
            if (index < positiveHalf.GetLength()) 
            {
                return positiveHalf.Get(index);
            }
            return blankSymbol; 
        } else 
        {
            int negIndex = -index - 1; 
            if (negIndex < negativeHalf.GetLength()) 
            {
                return negativeHalf.Get(negIndex);
            }
            return blankSymbol;
        }
    }

    void WriteSymbol(int index, char symbol) override 
    {
        if (index >= 0) 
        {
            EnsureCapacity(positiveHalf, index);
            positiveHalf[index] = symbol; 
        } else 
        {
            int negIndex = -index - 1;
            EnsureCapacity(negativeHalf, negIndex);
            negativeHalf[negIndex] = symbol;
        }
    }
};

#endif // TURING_TAPE_H