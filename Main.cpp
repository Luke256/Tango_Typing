#include "Main.h"
#include "MainSystem.h"
#include "Typing.h"
#include "TypingResult.h"
#include "Select.h"
#include "SelectResult.h"

void Main()
{
    Scene::SetBackground(GameInfo::BackgroundColor);
    Scene::Resize(GameInfo::Width, GameInfo::Height);
    Window::Resize(GameInfo::Width, GameInfo::Height);

    App manager;
    manager.setFadeColor(Palette::White);
    manager.add<MainSystem>(U"MainSystem");
    manager.add<Typing>(U"Typing");
    manager.add<TypingResult>(U"TypingResult");
    manager.add<Select>(U"Select");
    manager.add<SelectResult>(U"SelectResult");

    FontAsset::Register(U"ItemName", 40, Typeface::Light);
    FontAsset::Register(U"Heading", 50, Typeface::Light);
    FontAsset::Register(U"DetailTitle", 20, Typeface::Light);
    FontAsset::Register(U"DetailDetail", 30, Typeface::Light);
    FontAsset::Register(U"Lobby", 25, Typeface::Light);
    FontAsset::Register(U"TypingCountdown", 50, Typeface::Light);
    FontAsset::Register(U"TypingProblem", 50, Typeface::Light);
    FontAsset::Register(U"ResultToHomeButton", 40);


    while (System::Update())
    {
        if(!manager.update()) break;
    }
}
