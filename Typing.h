#pragma once
#include "Main.h"

class Typing : public App::Scene{
private:
    Stopwatch MainClock;
    Stopwatch ProblemClock;
    Stopwatch NotKeyStreak;
    Stopwatch TypingClock;
    Array<Problem>ProblemSet;
    const double TypingTimeLimit = 4;

    bool ShowHint;
    int32 CurrentProblemIndex;
    String Input;
    String CurrentTypedString;
    int32 CurrentTypedLength;
    
    
public:
    Typing(const InitData& init);
    void update() override;
    void draw() const override;
};
