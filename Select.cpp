#include "Select.h"

Select::Select(const InitData& init) : IScene(init){
    getData().ProblemSet.clear();

    // create problem set
    Array<Problem>ProblemBuffer;
    for(auto FolderName : getData().ProblemFileSet){
        const Folder& folder = getData().Table[FolderName];
        Print << getData().Table[FolderName].value.size();
        for(auto v : folder.value){
            Problem t;

            if (getData().ProblemType == 0){
                t = Problem(v.second.Japanese, v.second.English, v.second.JapaneseEx, v.second.EnglishEx);
            }
            else{
                t = Problem(v.second.English, v.second.Japanese, v.second.EnglishEx, v.second.JapaneseEx);
            }

            ProblemBuffer << t;
        }
    }
    for(auto i : step(getData().NumberProblem)){
        if(!(i % ProblemBuffer.size())) ProblemBuffer.shuffle();
        getData().ProblemSet << ProblemBuffer[i%ProblemBuffer.size()];
    }

    for(auto i : step(4)){
        SelectionButtons << Rect(125 + (i%2)*400, 230 + (i/2)*170, 250, 100);
    }

    CurrentProblemID = -1;

    getData().ACCount=0;

    isinSolving = true;
    ResultStopWatch=Stopwatch(true);

    AC=Texture(U"data/assets/SelectingAC.png", TextureDesc::Mipped);
    WA=Texture(U"data/assets/SelectingWA.png", TextureDesc::Mipped);

    MoveNextButton = Rect(600, 520, 200, 60);
}

/*update---------------------------------------------------------------------*/
void Select::update() {
    if(CurrentProblemID == -1){
        // 問題の作成
        ++CurrentProblemID;
        CreateNewProblem();
        return;
    }

    ClearPrint();
    for(auto i : getData().ProblemSet){
        Print << i.m_question << U" : " << i.m_answer;
    }
    Print << U"Selection : ";
    for(auto i : step(4)){
        Print << CurrentSelection[i];
    }
    Print<<ResultStopWatch.sF();
    Print<<getData().ProblemSet[CurrentProblemID].m_answer;


    for(int32 index = 0;auto Button : SelectionButtons){
        if(Button.mouseOver()) Cursor::RequestStyle(CursorStyle::Hand);
        if(Button.leftReleased() && isinSolving){
            // 正誤判定
            if(getData().ProblemSet[CurrentProblemID].m_answer == CurrentSelection[index]){
                ++getData().ACCount;
                isAC=true;
                getData().ProblemSet[CurrentProblemID].Result=0;
            }
            else{
                isAC=false;
                getData().ProblemSet[CurrentProblemID].Result=1;
            }

            ResultStopWatch.restart();
            isinSolving = false;
            SelectedSelection = index;
        }

        ++index;
    }

    if(!isinSolving){
        if(MoveNextButton.mouseOver()) Cursor::RequestStyle(CursorStyle::Hand);
        if(MoveNextButton.leftReleased()){
            if(CurrentProblemID == getData().NumberProblem - 1) changeScene(U"SelectResult");
            else ++CurrentProblemID;
            isinSolving = true;
            CreateNewProblem();
        }
    }
}

/*draw---------------------------------------------------------------------*/
void Select::draw() const {
    FontAsset(U"TypingProblem")(getData().ProblemSet[CurrentProblemID].m_question).drawAt(GameInfo::Width / 2, 80, Palette::Black);
    FontAsset(U"ItemName")(getData().ProblemSet[CurrentProblemID].m_problemex).drawAt(GameInfo::Width / 2, 130, Palette::Darkgray);

    for(auto i : step(4)){
        SelectionButtons[i].draw().drawFrame(0, 5, RGB(78, 141, 230));
        if(SelectionButtons[i].mouseOver())SelectionButtons[i].draw(RGB(0,0,0,.05));
        if(SelectionButtons[i].leftPressed())SelectionButtons[i].drawFrame(0, 5, RGB(73, 101, 252));
        if(!CurrentSelection.empty())FontAsset(U"ItemName")(CurrentSelection[i]).drawAt(SelectionButtons[i].center(), Palette::Black);
    }

    if(!isinSolving){
        MoveNextButton.draw().drawFrame(5, 0, RGB(73, 195, 252));
        FontAsset(U"ItemName")((CurrentProblemID == getData().NumberProblem - 1 ? U"結果" : U"次へ")).drawAt(MoveNextButton.center(), Palette::Black);

        double Trans = Max(0.0, 2-ResultStopWatch.sF())/2.0;
        if(isAC) AC.resized(400,400).drawAt(Scene::Center(), RGB(252, 98, 117, Trans));
        else{
            WA.resized(400,400).drawAt(Scene::Center(), RGB(37, 101, 252, Trans));
            SelectionButtons[SelectedSelection].draw(RGB(242, 110, 94, .2));
        }
        SelectionButtons[CorrectSelectionIndex].draw(RGB(70, 232, 242, .2));
        FontAsset(U"ItemName")(getData().ProblemSet[CurrentProblemID].m_answerex).drawAt(GameInfo::Width / 2, 180, Palette::Darkgray);
    }
}


/*other--------------------------------------------------------------------*/


void Select::CreateNewProblem(){
    Array<Problem> Selection;

    // ダミーを作成
    Selection = getData().ProblemSet.choice(3);

    String s = getData().ProblemSet[CurrentProblemID].m_answer;

    while(Selection.any([s](Problem p){return (s == p.m_answer);}) || not isCorrectSelection(Selection)){
        Selection = getData().ProblemSet.choice(3);
    }


    CurrentSelection.clear();
    CurrentSelection << getData().ProblemSet[CurrentProblemID].m_answer;
    for(auto i : Selection){
        CurrentSelection << i.m_answer;
    }

    CurrentSelection.shuffle();

    for(int32 index=0; auto i : CurrentSelection){
        if(i==getData().ProblemSet[CurrentProblemID].m_answer){
            CorrectSelectionIndex = index;
            break;
        }
        ++index;
    }
}

bool Select::isCorrectSelection(const Array<Problem> &Selection){
    if(Selection[0].m_answer == Selection[1].m_answer || Selection[0].m_answer == Selection[2].m_answer || Selection[2].m_answer == Selection[1].m_answer) return false;
    return true;
}