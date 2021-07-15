# pragma once
#include <Siv3D.hpp>

struct Problem{
    String m_question;
    String m_answer;
    String m_jaex;
    String m_enex;
    int32 Result;
    Problem(String problem, String answer) : m_question(problem),m_answer(answer),Result(2){}
    Problem(String problem, String answer, String jaex, String enex):
    m_question(problem),
    m_answer(answer),
    m_jaex(jaex),
    m_enex(enex){}
    Problem(){}
};

struct Word{
    String English, Japanese, EnglishEx, JapaneseEx;
    int32 id, EnExHeight, JaExHeight, ScrollMax;
};

struct Folder{
    HashTable<String, Word>value;
    Array<int32>ids;
    bool isInProblemSet;
    Folder():isInProblemSet(false){}
    int32 NextID(){
        if(ids.size())return ids.back()+1;
        else return 1;
    }
};

inline ColorF RGB(const int32 &r, const int32 &g, const int32 &b, const double &a=1){
    return ColorF(r/255.0, g/255.0, b/255.0, a);
}

struct GameData{
    String WindowMode=U"ManageWord";
    HashTable<String, Folder>Table;
    Array<String>ProblemFileSet;
    Array<Problem>ProblemSet;
    int32 NumberProblem = 10;
    int32 ACCount = 0;
    size_t ProblemType = 0;
    int32 CorrectTypeCount = 0;
    int32 AllTypeCount = 0;
    double TypingStopwatch = 1;
};

using App = SceneManager<String, GameData>;

namespace GameInfo{
    const ColorF BackgroundColor=Palette::Ghostwhite;
    const int32 Width=900;
    const int32 Height=600;
};
