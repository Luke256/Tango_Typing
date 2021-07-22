#pragma once
#include "Main.h"

class Select : public App::Scene{
private:
    int32 CurrentProblemID;

    Array<String>CurrentSelection;
    Array<Rect>SelectionButtons;

    Rect MoveNextButton;
    
    bool isinSolving; // 選択前か、選択後(リザルト)か
    bool isAC;
    int32 SelectedSelection; // リザルトに使う
    int32 CorrectSelectionIndex; // これもリザルト
    Stopwatch ResultStopWatch;

    Texture AC, WA;

public:
    Select(const InitData& init);
    void update() override;
    void draw() const override;
    void CreateNewProblem();
    bool isCorrectSelection(const Array<Problem> &Selection);
};