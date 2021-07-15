#pragma once
#include "Main.h"
#include <Siv3D.hpp>

class MainSystem : public App::Scene{
    Texture trash, done, startTest, keybord, list;
    const int32 ItemBoxHeight=60;
    std::stack<String>Layers;
    ChildProcess AddingWord;
    String AddWordProcessName;
    bool AddWordProcessRunning;

    String SelectingFolder;
    String SelectingWord;

    Transition FolderTransitionSwitchButton;
    int32 FolderScroll;
    Circle FolderButtonAdd;
    Circle FolderButtonStartTest;


    bool ItemsActive;
    Transition ItemsTransition;
    int32 ItemsScroll;
    Circle ItemButtonBack;
    Circle ItemButtonAdd;

    bool DetailActive;
    Transition DetailTransition;
    Circle DetailButtonBack;
    int32 DetailScroll;

    bool LobbyActive;
    Transition LobbyTransition;
    RoundRect LobbyStartWithSelect;
    RoundRect LobbyStartWithTyping;
    Rect LobbyViewPort;
    TextEditState LobbyMaxNumberProblem;


    // Camera2D camera;

public:
    MainSystem(const InitData& init);
    void update() override;
    void draw() const override;
    void LoadWords(String FolderName);
    void RemoveWord();
    void RemoveFolder();
};

