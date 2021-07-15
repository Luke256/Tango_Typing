#pragma once
#include "Main.h"

class TypingResult : public App::Scene{
private:
    const int32 WordsResultScrollMax = getData().ProblemSet.size() * FontAsset(U"ItemName").height();

    double TpS; // Type per Second

    Rect WordsResultViewPort;
    int32 WordsResultScroll;

    RoundRect RetryButton;
    RoundRect ToHomeButton;
    
public:
    TypingResult(const InitData& init);
    void update() override;
    void draw() const override;
};