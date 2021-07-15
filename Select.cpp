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
                t = Problem(v.second.English, v.second.Japanese, v.second.JapaneseEx, v.second.EnglishEx);
            }

            ProblemBuffer << t;
        }
    }
    for(auto i : step(getData().NumberProblem)){
        if(!(i % ProblemBuffer.size())) ProblemBuffer.shuffle();
        getData().ProblemSet << ProblemBuffer[i%ProblemBuffer.size()];
    }

    for(auto i : getData().ProblemSet){
        Print << i.m_question << U" : " << i.m_answer;
    }
}

/*update---------------------------------------------------------------------*/
void Select::update() {
    
}

/*draw---------------------------------------------------------------------*/
void Select::draw() const {
    
}


// Array<String> Select::getDummies(){
    
// }