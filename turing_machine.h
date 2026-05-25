#ifndef TURING_MACHINE_H
#define TURING_MACHINE_H

#include "array_sequence.h"

enum class Direction 
{ 
    Left = -1, 
    Stay = 0, 
    Right = 1 
};

struct TransitionRule 
{
    int currentState;
    char readSymbol;
    int nextState;
    char writeSymbol;
    Direction moveDirection;

    TransitionRule() : currentState(0), readSymbol('\0'), nextState(0), writeSymbol('\0'), moveDirection(Direction::Stay) {}

    TransitionRule(int curr, char read, int next, char write, Direction dir)
        : currentState(curr), readSymbol(read), nextState(next), writeSymbol(write), moveDirection(dir) {}
};

class ITuringTape 
{
public:
    virtual ~ITuringTape() = default;
    virtual char ReadSymbol(int index) const = 0;
    virtual void WriteSymbol(int index, char symbol) = 0;
};

class TuringMachine 
{
private:
    ITuringTape* tape;
    MutableArraySequence<TransitionRule> rules;
    int currentState;
    int headPosition;
    int terminalState;

public:
    //Машина не владеет лентой
    TuringMachine(ITuringTape* t, int startState, int termState)
        : tape(t), currentState(startState), terminalState(termState), headPosition(0) {}

    void AddRule(int state, char readSym, int nextState, char writeSym, Direction dir) 
    {
        rules.Append(TransitionRule(state, readSym, nextState, writeSym, dir));
    }

    bool Step() 
    {
        if (currentState == terminalState) 
        {
            return false; 
        }

        char currentSymbol = tape->ReadSymbol(headPosition);
        bool ruleFound = false;
        TransitionRule rule;

        for (int i = 0; i < rules.GetLength(); ++i) 
        {
            if (rules[i].currentState == currentState && rules[i].readSymbol == currentSymbol) 
            {
                rule = rules[i];
                ruleFound = true;
                break;
            }
        }

        if (!ruleFound) 
        {
            currentState = terminalState;
            return false; 
        }

        tape->WriteSymbol(headPosition, rule.writeSymbol);
        if (rule.moveDirection == Direction::Right) 
        {
            headPosition += 1;
        } else if (rule.moveDirection == Direction::Left) 
        {
            headPosition -= 1;
        }
        currentState = rule.nextState;
        return true; 
    }

    int GetCurrentState() const 
    { 
        return currentState; 
    }
    int GetHeadPosition() const 
    { 
        return headPosition; 
    }
};

#endif // TURING_MACHINE_H