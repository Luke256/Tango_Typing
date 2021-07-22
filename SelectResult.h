#pragma once
#include "Main.h"

class SelectResult : public App::Scene{
private:
    const int32 WordsResultScrollMax = getData().ProblemSet.size() * FontAsset(U"ItemName").height();


    Rect WordsResultViewPort;
    int32 WordsResultScroll;

    RoundRect RetryButton;
    RoundRect ToHomeButton;
    
public:
    SelectResult(const InitData& init);
    void update() override;
    void draw() const override;
};