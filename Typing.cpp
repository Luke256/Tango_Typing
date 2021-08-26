#include "Typing.h"

Typing::Typing(const InitData& init) : IScene(init){
    // create problem set
    Array<Problem>ProblemBuffer;
    for(auto FolderName : getData().ProblemFileSet){
        const Folder& folder = getData().Table[FolderName];
        for(auto v : folder.value){
            Problem t(v.second.Japanese, v.second.English);
            ProblemBuffer << t;
        }
    }
    for(auto i : step(getData().NumberProblem)){
        if(!(i % ProblemBuffer.size())) ProblemBuffer.shuffle();
        ProblemSet << ProblemBuffer[i%ProblemBuffer.size()];
    }

    CurrentProblemIndex=-1;

    getData().AllTypeCount = 0;
    getData().CorrectTypeCount = 0;
    getData().ACCount = 0;

    ProblemClock.set(Duration(TypingTimeLimit));
    MainClock.start();
}

/*update---------------------------------------------------------------------*/
void Typing::update() {
    Input = U"";
    TextInput::UpdateText(Input);
    getData().AllTypeCount += Input.length();

    // update starts after 5s left
    if (MainClock.s() < 5){
        TypingClock.restart();
        return;
    }


    for(char c : Input){
        c= ToLower(c);
        if (CurrentTypedLength < ProblemSet[CurrentProblemIndex].m_answer.length() && c == ToLower(ProblemSet[CurrentProblemIndex].m_answer[CurrentTypedLength])){
            CurrentTypedString = CurrentTypedString + c;
            ++CurrentTypedLength;
            ++getData().CorrectTypeCount;
            NotKeyStreak.restart();
            break;
        }
    }

    if(ProblemClock.s() >= TypingTimeLimit || CurrentTypedLength == ProblemSet[CurrentProblemIndex].m_answer.length()){
        if(ProblemClock.s() < TypingTimeLimit){
            ++getData().ACCount;
            ProblemSet[CurrentProblemIndex].Result = (ShowHint ? 1 : 0);
        }

        ++CurrentProblemIndex;
        CurrentTypedLength = 0;
        CurrentTypedString=U"";
        if(CurrentProblemIndex >= ProblemSet.size()){
            getData().TypingStopwatch = TypingClock.sF();
            getData().ProblemSet = ProblemSet;
            changeScene(U"TypingResult");
        }


        NotKeyStreak.restart();
        ProblemClock.restart();
        ShowHint = false;
    }

    if (NotKeyStreak.sF() >= 1.5) ShowHint = true;

}

/*draw---------------------------------------------------------------------*/
void Typing::draw() const {
    Line(GameInfo::Width / 2, 200, GameInfo::Width / 2 - (TypingTimeLimit - ProblemClock.sF()) / 5.0 * 300, 200).draw(3, RGB(68, 197, 252));
    Line(GameInfo::Width / 2, 200, GameInfo::Width / 2 + (TypingTimeLimit - ProblemClock.sF()) / 5.0 * 300, 200).draw(3, RGB(68, 197, 252));

    if(CurrentProblemIndex+1 && CurrentProblemIndex < ProblemSet.size()){
        FontAsset(U"TypingProblem")(ProblemSet[CurrentProblemIndex].m_question).drawAt(GameInfo::Width / 2, 100, Palette::Black);
        if (ShowHint && CurrentTypedLength < ProblemSet[CurrentProblemIndex].m_answer.length()){
            Vec2 LeftTop = FontAsset(U"TypingProblem")(ProblemSet[CurrentProblemIndex].m_answer).regionAt(GameInfo::Width / 2, 300).pos;
            int32 i = 0;
            for(auto c : FontAsset(U"TypingProblem")(ProblemSet[CurrentProblemIndex].m_answer)){
                if(i < CurrentTypedLength) FontAsset(U"TypingProblem")(ProblemSet[CurrentProblemIndex].m_answer[i]).draw(LeftTop, Palette::Gray);
                else{
                    const ScopedColorMul2D state(RGB(255,0,0, 1 - Max(0, i - CurrentTypedLength + 1) / 4.0));
                    FontAsset(U"TypingProblem")(ProblemSet[CurrentProblemIndex].m_answer[i]).draw(LeftTop, Palette::Gray);
                }
                LeftTop.x += c.xAdvance;
                ++i;
            }
            // FontAsset(U"TypingProblem")(CurrentTypedString).drawAt(GameInfo::Width / 2, 300, Palette::Darkgray);
        }
        //  if(ProblemClock.s() < TypingTimeLimit - 1)
        else FontAsset(U"TypingProblem")(CurrentTypedString).drawAt(GameInfo::Width / 2, 300, Palette::Gray);
    }

    // Work in only the begining of scene.
    if (MainClock.s() < 5){
        Rect(0, 0, GameInfo::Width, GameInfo::Height).draw(ColorF(1, 1, 1, 0.3));
        if(MainClock.s() >=1){
            FontAsset(U"TypingCountdown")(4-MainClock.s() ? ToString(4 - MainClock.s()) : U"スタート").drawAt(Scene::Center(), ColorF(0,0,0, 1.0 - (MainClock.ms() % 1000) / 1000.0));

            Line(GameInfo::Width / 2, 200, GameInfo::Width / 2 - Max(0.0, 5 - MainClock.sF()) / 5.0 * 300, 200).draw(3, RGB(68, 197, 252));
            Line(GameInfo::Width / 2, 200, GameInfo::Width / 2 + Max(0.0, 5 - MainClock.sF()) / 5.0 * 300, 200).draw(3, RGB(68, 197, 252));
        }
    }
}