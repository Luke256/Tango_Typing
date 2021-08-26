#include "MainSystem.h"
# include <Siv3D.hpp>


MainSystem::MainSystem(const InitData& init) : IScene(init){
    for(const auto &i : FileSystem::DirectoryContents(U"data/", false)){
        if(FileSystem::RelativePath(i, FileSystem::ModulePath())==U"../data/assets/" || FileSystem::RelativePath(i, FileSystem::ModulePath())==U"../data/.DS_Store")continue;
        Folder t;
        getData().Table.emplace(FileSystem::BaseName(i), t);
    }

    Layers.push(U"Folders");

    FolderScroll=0;
    // SelectingFolder=getData().Table.begin()->first;
    SelectingFolder=U"";
    AddWordProcessRunning=false;
    trash=Texture(U"data/assets/trash.png", TextureDesc::Mipped);
    done = Texture(U"data/assets/done.png", TextureDesc::Mipped);
    startTest = Texture(U"data/assets/test.png", TextureDesc::Mipped);
    keybord = Texture(U"data/assets/keybord.png", TextureDesc::Mipped);
    list = Texture(U"data/assets/list.png", TextureDesc::Mipped);

    FolderButtonAdd=Circle(GameInfo::Width - 60,GameInfo::Height - 60, 35);
    FolderButtonStartTest=Circle(GameInfo::Width - 60,GameInfo::Height + 60, 35);
    FolderTransitionSwitchButton=Transition(0.5s,0.5s);

    ItemsActive=false;
    ItemsTransition=Transition(0.5s,0.5s);
    ItemsScroll=0;
    ItemButtonBack=Circle(40,40,25);
    ItemButtonAdd=Circle(GameInfo::Width - 60,GameInfo::Height - 60, 35);

    SelectingWord=U"---";
    DetailActive=false;
    DetailTransition=Transition(0.5s,0.5s);
    DetailButtonBack=Circle(40,40,25);
    DetailScroll=0;

    LobbyActive = false;
    LobbyTransition = Transition(0.2s, 0.2s);
    LobbyStartWithSelect = RoundRect(30, 190, 200, 150, 8);
    LobbyStartWithTyping = RoundRect(270, 190, 200, 150, 8);
    LobbyViewPort = Rect(Arg::center(Scene::Center().x, Scene::Center().y + 50), 500, 370);
    LobbyMaxNumberProblem.text = ToString(getData().NumberProblem);




# if SIV3D_PLATFORM(WINDOWS)
    // 子プロセスで実行するファイルのパス
    AddWordProcessName = U"data/assets/Subprocess.exe";
# elif SIV3D_PLATFORM(MACOS)
    // 子プロセスで実行するファイルのパス
    AddWordProcessName = U"data/assets/Subprocess.app/Contents/MacOS/Subprocess";
# endif
}

/*update---------------------------------------------------------------------*/
void MainSystem::update() {
    // camera.update();
    // {
    //     const auto t = camera.createTransformer();
    ItemsTransition.update(ItemsActive);
    DetailTransition.update(DetailActive);
    FolderTransitionSwitchButton.update(!getData().ProblemFileSet.empty());
    LobbyTransition.update(LobbyActive);

    FolderButtonAdd.setPos(Vec2(GameInfo::Width - 60, GameInfo::Height - 60 + EaseInOutBack(FolderTransitionSwitchButton.value()) * 120));
    FolderButtonStartTest.setPos(Vec2(GameInfo::Width - 60, GameInfo::Height + 60 - EaseInOutBack(FolderTransitionSwitchButton.value()) * 120));

    if(AddWordProcessRunning && !AddingWord.isRunning()){
        AddWordProcessRunning=false;
        for(const auto &i : FileSystem::DirectoryContents(U"data/", false)){
            // assetsと.DS_Storeは無視
            if(FileSystem::RelativePath(i, FileSystem::ModulePath())==U"../data/assets/" || FileSystem::RelativePath(i, FileSystem::ModulePath())==U"../data/.DS_Store")continue;
            Folder t;
            if(!getData().Table.contains(FileSystem::BaseName(i)))getData().Table.emplace(FileSystem::BaseName(i), t);
        }
        LoadWords(SelectingFolder);
    }
    if(AddingWord.isRunning()) return;

    if(Layers.top()==U"Folders"){
        FolderScroll=Max(-(int32)getData().Table.size()*ItemBoxHeight+ItemBoxHeight,Min(0, int32(FolderScroll-Mouse::Wheel()*10)));

        int32 i=0;
        for(auto itr=getData().Table.begin(); itr!=getData().Table.end(); ++itr){
        // for(int32 i=0; auto &item : getData().Table){
            
            if(itr.key()==U"")continue;
            Rect rect=Rect(0,i*ItemBoxHeight+80+FolderScroll,GameInfo::Width, ItemBoxHeight);
            RectF trash_rect(Arg::center(rect.center()+Vec2(GameInfo::Width/2-ItemBoxHeight/2,0)), ItemBoxHeight-20);
            RectF test_rect(Arg::center(rect.center()+Vec2(GameInfo::Width/2-ItemBoxHeight/2*3,0)), ItemBoxHeight-20);
            Circle AddToProblemSet(40, i*ItemBoxHeight+ItemBoxHeight/2+80+FolderScroll, 20);
            if(rect.leftReleased() && !test_rect.mouseOver() && !trash_rect.mouseOver() && !FolderButtonAdd.mouseOver() && Cursor::Pos().y>80 && !AddToProblemSet.mouseOver()){
                // 単語一覧に移動
                SelectingFolder=itr.key();
                Layers.push(U"Items");
                ItemsActive=true;
                // 単語の読み込み
                LoadWords(SelectingFolder);
            }
            if(rect.mouseOver() && Cursor::Pos().y>80){
                Cursor::RequestStyle(CursorStyle::Hand);
            }

            if(trash_rect.leftReleased() && Cursor::Pos().y>80){
                SelectingFolder = itr.key();
                if(System::ShowMessageBox(U"フォルダー「"+SelectingFolder+U"」を本当に削除しますか？　(この操作は取り消せません)", MessageBoxButtons::YesNo) == MessageBoxSelection::Yes)RemoveFolder();
            }
            
            if(test_rect.leftReleased() && Cursor::Pos().y>80){
                itr.value().isInProblemSet = true;
                getData().ProblemFileSet << itr.key();
                Layers.push(U"Lobby");
                LobbyActive = true;
            }

            if(AddToProblemSet.leftReleased() && Cursor::Pos().y > 80){
                String FolderName=itr.key();
                if(itr.value().isInProblemSet){
                    getData().ProblemFileSet.remove_if([FolderName](String s)->bool{return s==FolderName;});
                    itr.value().isInProblemSet=false;
                }
                else{
                    getData().ProblemFileSet << itr.key();
                    itr.value().isInProblemSet=true;
                }
            }
            ++i;
        }

        if(FolderButtonAdd.mouseOver() || FolderButtonStartTest.mouseOver()){
            Cursor::RequestStyle(CursorStyle::Hand);
        }
        if(FolderButtonAdd.leftReleased() && getData().ProblemFileSet.empty()){
            AddingWord=Process::Spawn(AddWordProcessName, Pipe::StdInOut);
            AddingWord.ostream() << 2 << std::endl;
            AddWordProcessRunning=true;
        }
        if(FolderButtonStartTest.leftReleased() && !getData().ProblemFileSet.empty()){
            Layers.push(U"Lobby");
            LobbyActive=true;
        }
    }
    else if(Layers.top()==U"Items"){
        ItemsScroll=Max(-(int32)getData().Table[SelectingFolder].value.size()*ItemBoxHeight+ItemBoxHeight,Min(0, int32(ItemsScroll-Mouse::Wheel()*10)));

        int32 Slide;
        if(ItemsActive)Slide=EaseInQuint(1-ItemsTransition.value())*GameInfo::Width;
        else Slide=EaseOutQuint(1-ItemsTransition.value())*GameInfo::Width;
        {
            const Transformer2D t(Mat3x2::Translate(Slide,0), true);
            int32 i=0;
            for(const auto &item : getData().Table[SelectingFolder].value){
                if(Rect(0,i*ItemBoxHeight+80+ItemsScroll,GameInfo::Width, ItemBoxHeight).leftReleased() && Cursor::Pos().y>80 && !ItemButtonAdd.mouseOver()){
                    Layers.push(U"Detail");
                    DetailActive=true;
                    SelectingWord=item.first;
                }
                if(Rect(0,i*ItemBoxHeight+80+ItemsScroll,GameInfo::Width, ItemBoxHeight).mouseOver() && Cursor::Pos().y>80){
                    Cursor::RequestStyle(CursorStyle::Hand);
                }
                ++i;
            }
            if(ItemButtonBack.mouseOver() || ItemButtonAdd.mouseOver()){
                Cursor::RequestStyle(CursorStyle::Hand);
            }
            if(ItemButtonBack.leftReleased()){
                Layers.pop();
                ItemsActive=false;
                SelectingWord=U"---";
            }
            if(ItemButtonAdd.leftReleased()){
                AddingWord=Process::Spawn(AddWordProcessName, Pipe::StdInOut);
                AddingWord.ostream() << 1 << std::endl;
                AddingWord.ostream() << SelectingFolder << std::endl;
                AddingWord.ostream() << getData().Table[SelectingFolder].NextID() << std::endl;
                AddWordProcessRunning=true;
            }
        }
    }
    else if(Layers.top()==U"Detail"){
        int32 Slide;
        if(DetailActive)Slide=EaseInQuint(1-DetailTransition.value())*GameInfo::Width;
        else Slide=EaseOutQuint(1-DetailTransition.value())*GameInfo::Width;
        {
            const Transformer2D t(Mat3x2::Translate(Slide,0), true);
            if(DetailButtonBack.mouseOver()){
                Cursor::RequestStyle(CursorStyle::Hand);
            }
            if(DetailButtonBack.leftReleased()){
                Layers.pop();
                DetailActive=false;
                LoadWords(SelectingFolder);
            }
        }
        if(SelectingWord!=U"---"){
            DetailScroll=Max((int32)getData().Table[SelectingFolder].value[SelectingWord].ScrollMax,Min(0, int32(DetailScroll-Mouse::Wheel()*10)));
            bool no=false;
            while(!no){
                no=FontAsset(U"DetailDetail")(getData().Table[SelectingFolder].value[SelectingWord].EnglishEx).draw(Rect(50,320,GameInfo::Width-100, getData().Table[SelectingFolder].value[SelectingWord].EnExHeight), Palette::Black);
                if(!no){
                    getData().Table[SelectingFolder].value[SelectingWord].EnExHeight+=30;
                    getData().Table[SelectingFolder].value[SelectingWord].ScrollMax-=30;
                }
            }
            no=false;
            while(!no){
                no=FontAsset(U"DetailDetail")(getData().Table[SelectingFolder].value[SelectingWord].JapaneseEx).draw(Rect(50,320+getData().Table[SelectingFolder].value[SelectingWord].EnExHeight+10,GameInfo::Width-100, getData().Table[SelectingFolder].value[SelectingWord].JaExHeight), Palette::Black);
                if(!no){
                    getData().Table[SelectingFolder].value[SelectingWord].JaExHeight+=30;
                    getData().Table[SelectingFolder].value[SelectingWord].ScrollMax-=30;
                }
            }

            int32 h=getData().Table.at(SelectingFolder).value.at(SelectingWord).EnExHeight+getData().Table.at(SelectingFolder).value.at(SelectingWord).JaExHeight+400+DetailScroll;
            if(Rect(0,h,GameInfo::Width, 60).mouseOver())Cursor::RequestStyle(CursorStyle::Hand);
            if(Rect(0,h,GameInfo::Width, 60).leftReleased()){
                if(System::ShowMessageBox(U"フォルダー「"+SelectingFolder+U"」から単語「"+SelectingWord+U"」を本当に削除しますか？　(この操作は取り消せません)", MessageBoxButtons::YesNo) == MessageBoxSelection::Yes)RemoveWord();
            }
        }
    }
    else if(Layers.top() == U"Lobby"){
        if(MouseL.up() && !LobbyViewPort.movedBy(0, LobbyTransition.value() * -50).mouseOver()){
            LobbyActive=false;
            Layers.pop();
            for(auto i : getData().ProblemFileSet){
                getData().Table[i].isInProblemSet=false;
            }
            getData().ProblemFileSet.clear();
        }
        {
            const Transformer2D t(Mat3x2::Translate(LobbyViewPort.x, LobbyViewPort.y + LobbyTransition.value() * -50), true);
            const ScopedColorMul2D state(ColorF(1,1,1,LobbyTransition.value()));
            // const ScopedViewport2D viewport(LobbyViewPort.movedBy(0,LobbyTransition.value() * -50));
            if (LobbyStartWithSelect.mouseOver() || LobbyStartWithTyping.mouseOver()) Cursor::RequestStyle(CursorStyle::Hand);
            if (SimpleGUI::Button(U"↑", Vec2(300, 30), 50, getData().NumberProblem > 0)) --getData().NumberProblem;
            if (SimpleGUI::Button(U"↓", Vec2(400, 30), 50)) ++getData().NumberProblem;
            if (SimpleGUI::TextBox(LobbyMaxNumberProblem, Vec2(350, 30), 50, 10, true)){
                try{
                    getData().NumberProblem = Parse<int32>(LobbyMaxNumberProblem.text);
                }
                catch(ParseError e){
                    getData().NumberProblem = 0;
                }
            }
            LobbyMaxNumberProblem.text = ToString(getData().NumberProblem);
            if (LobbyStartWithTyping.leftReleased()){
                for(auto i : getData().ProblemFileSet)LoadWords(i);
                for(auto i : getData().ProblemFileSet){
                    getData().Table[i].isInProblemSet=false;
                }
                changeScene(U"Typing");
            }

            if (LobbyStartWithSelect.leftReleased()){
                for(auto i : getData().ProblemFileSet)LoadWords(i);
                for(auto i : getData().ProblemFileSet){
                    getData().Table[i].isInProblemSet=false;
                }
                changeScene(U"Select");
            }

            SimpleGUI::RadioButtons(getData().ProblemType, {U"英語→日本語", U"日本語→英語"}, Vec2(300, 80));
        }
    }

    // }
}

/*draw---------------------------------------------------------------------*/
void MainSystem::draw() const {
    // {
    //     const auto t = camera.createTransformer();
    // Folders
    {
        int32 i=0;

        for(const auto &item : getData().Table){
            if(item.first==U"")continue;
            Rect rect=Rect(0,i*ItemBoxHeight+80+FolderScroll,GameInfo::Width, ItemBoxHeight);
            rect.draw(Palette::White).drawFrame(0,1,Palette::Lightgrey);
            FontAsset(U"ItemName")(item.first).draw(Arg::leftCenter=Vec2(100, i*ItemBoxHeight+ItemBoxHeight/2+80+FolderScroll), Palette::Black);

            Circle AddedProblemSet(40, i*ItemBoxHeight+ItemBoxHeight/2+80+FolderScroll, 20);
            if(item.second.isInProblemSet){
                AddedProblemSet.draw(RGB(73, 195, 252));
                AddedProblemSet.stretched(-5)(done).draw(Palette::White);
            }
            AddedProblemSet.drawFrame(1,0,Palette::Gray);
            if(rect.mouseOver() && Layers.top() == U"Folders"){
                RectF trash_rect(Arg::center(rect.center()+Vec2(GameInfo::Width/2-ItemBoxHeight/2,0)), ItemBoxHeight-20);
                RectF test_rect(Arg::center(rect.center()+Vec2(GameInfo::Width/2-ItemBoxHeight/2*3,0)), ItemBoxHeight-20);
                if(trash_rect.mouseOver())trash_rect(trash).draw(Palette::Red);
                else trash_rect(trash).draw(Palette::Lightgrey);

                if(test_rect.mouseOver()) test_rect(startTest).draw(Palette::Lightblue);
                else test_rect(startTest).draw(Palette::Lightgrey);
            }
            ++i;
        }


        FolderButtonAdd.draw(RGB(73, 195, 252));
        FolderButtonStartTest.draw(RGB(73, 195, 252));
        if(FolderButtonAdd.leftPressed()) FolderButtonAdd.draw(RGB(78, 141, 230, .4));
        if(FolderButtonStartTest.leftPressed()) FolderButtonStartTest.draw(RGB(78, 141, 230, .4));
        FontAsset(U"ItemName")(U"＋").drawAt(FolderButtonAdd.center);
        FolderButtonStartTest.stretched(-10)(startTest).draw(Palette::White);

        Rect(0,0,GameInfo::Width, 80).draw(Palette::White); // ヘッダー2
        Line(0,80,GameInfo::Width, 80).draw(2,RGB(78,141,230)); // ヘッダー2
        FontAsset(U"Heading")(U"単語帳").drawAt(GameInfo::Width/2, 40, Palette::Black); // ヘッダー文字
    }

    // Items
    int32 Slide;
    if(ItemsActive)Slide=EaseInQuint(1-ItemsTransition.value())*GameInfo::Width;
    else Slide=EaseOutQuint(1-ItemsTransition.value())*GameInfo::Width;
    {
        const Transformer2D t(Mat3x2::Translate(Slide,0), true);
        int32 i=0;
        Rect(0,0,GameInfo::Width, GameInfo::Height).draw(Palette::Ghostwhite); // 背景

        for(const auto &item : getData().Table.at(SelectingFolder).value){
            Rect(0,i*ItemBoxHeight+80+ItemsScroll,GameInfo::Width, ItemBoxHeight).draw(Palette::White).drawFrame(0,1,Palette::Lightgrey);
            FontAsset(U"ItemName")(item.first).draw(Arg::leftCenter=Vec2(100, i*ItemBoxHeight+ItemBoxHeight/2+80+ItemsScroll), Palette::Black);
            ++i;
        }

        ItemButtonAdd.draw(RGB(73, 195, 252));
        if(ItemButtonAdd.leftPressed()) ItemButtonAdd.draw(RGB(78, 141, 230, .4));
        FontAsset(U"ItemName")(U"＋").drawAt(ItemButtonAdd.center);

        Rect(0,0,GameInfo::Width, 80).draw(Palette::White); // ヘッダー
        Line(0,80,GameInfo::Width, 80).draw(2,RGB(78,141,230)); // ヘッダー2
        FontAsset(U"Heading")(SelectingFolder).drawAt(GameInfo::Width/2, 40, Palette::Black); // ヘッダー文字
        if(ItemButtonBack.mouseOver())ItemButtonBack.draw(RGB(73,196,252,0.2)); // 戻るボタン1
        if(ItemButtonBack.leftPressed())ItemButtonBack.draw(RGB(73,196,252,0.2)); // 戻るボタン2
        FontAsset(U"ItemName")(U"←").drawAt(ItemButtonBack.center, Palette::Cornflowerblue); // 戻るボタン3
    }


    // Detail
    if(DetailActive)Slide=EaseInQuint(1-DetailTransition.value())*GameInfo::Width;
    else Slide=EaseOutQuint(1-DetailTransition.value())*GameInfo::Width;
    {
        const Transformer2D t(Mat3x2::Translate(Slide,0), true);
        Rect(0,0,GameInfo::Width, GameInfo::Height).draw(Palette::Ghostwhite); // 背景

        FontAsset(U"DetailTitle")(U"意味").draw(30,100+DetailScroll, Palette::Silver);
        Rect(0,150+DetailScroll,GameInfo::Width, 80).draw(Palette::White);
        Line(0,150+DetailScroll,GameInfo::Width, 150+DetailScroll).draw(1,Palette::Silver);
        Line(0,230+DetailScroll,GameInfo::Width, 230+DetailScroll).draw(1,Palette::Silver);
        FontAsset(U"DetailTitle")(U"例文").draw(30,250+DetailScroll, Palette::Silver);
        if(SelectingWord!=U"---"){
            Rect(0,300+DetailScroll, GameInfo::Width, getData().Table.at(SelectingFolder).value.at(SelectingWord).EnExHeight+getData().Table.at(SelectingFolder).value.at(SelectingWord).JaExHeight+40).draw(Palette::White);
            Line(0,300+DetailScroll,GameInfo::Width, 300+DetailScroll).draw(1,Palette::Silver);
            Line(0,getData().Table.at(SelectingFolder).value.at(SelectingWord).EnExHeight+getData().Table.at(SelectingFolder).value.at(SelectingWord).JaExHeight+340+DetailScroll,GameInfo::Width, getData().Table.at(SelectingFolder).value.at(SelectingWord).EnExHeight+getData().Table.at(SelectingFolder).value.at(SelectingWord).JaExHeight+340+DetailScroll).draw(1,Palette::Silver);
            // https://siv3d.github.io/ja-jp/tutorial/font/#814
            // これを参考にしながら折り返しを作る
            FontAsset(U"DetailDetail")(getData().Table.at(SelectingFolder).value.at(SelectingWord).Japanese).draw(Arg::leftCenter=Vec2(50,190+DetailScroll), Palette::Black);
            FontAsset(U"DetailDetail")(getData().Table.at(SelectingFolder).value.at(SelectingWord).EnglishEx).draw(Rect(50,320+DetailScroll,GameInfo::Width-100, getData().Table.at(SelectingFolder).value.at(SelectingWord).EnExHeight), Palette::Black);
            FontAsset(U"DetailDetail")(getData().Table.at(SelectingFolder).value.at(SelectingWord).JapaneseEx).draw(Rect(50,320+DetailScroll+getData().Table.at(SelectingFolder).value.at(SelectingWord).EnExHeight+10,GameInfo::Width-100, getData().Table.at(SelectingFolder).value.at(SelectingWord).EnExHeight), Palette::Black);
            int32 h=getData().Table.at(SelectingFolder).value.at(SelectingWord).EnExHeight+getData().Table.at(SelectingFolder).value.at(SelectingWord).JaExHeight+400+DetailScroll;
            Rect(0,h,GameInfo::Width, 60).draw(Palette::White);
            Line(0,h,GameInfo::Width,h).draw(1,Palette::Silver);
            Line(0,h+60,GameInfo::Width,h+60).draw(1,Palette::Silver);
            FontAsset(U"DetailDetail")(U"単語を削除").drawAt(GameInfo::Width/2, h+30, Palette::Red);
        }


        Rect(0,0,GameInfo::Width, 80).draw(Palette::White); // ヘッダー
        Line(0,80,GameInfo::Width, 80).draw(2,RGB(78,141,230)); // ヘッダー2
        FontAsset(U"Heading")(SelectingWord).drawAt(GameInfo::Width/2, 40, Palette::Black); // ヘッダー文字
        if(DetailButtonBack.mouseOver())DetailButtonBack.draw(RGB(73,196,252,0.2)); // 戻るボタン1
        if(DetailButtonBack.leftPressed())DetailButtonBack.draw(RGB(73,196,252,0.2)); // 戻るボタン2
        FontAsset(U"ItemName")(U"←").drawAt(DetailButtonBack.center, Palette::Cornflowerblue); // 戻るボタン3
    }

    // Lobby
    {
        const ScopedColorMul2D state(ColorF(1,1,1,LobbyTransition.value()));
        LobbyViewPort.movedBy(0, LobbyTransition.value() * -50).draw().drawFrame(2, 0, Palette::Lightgrey);
        {
            const Transformer2D t(Mat3x2::Translate(LobbyViewPort.x, LobbyViewPort.y + LobbyTransition.value() * -50), true);
            // const ScopedViewport2D viewport(LobbyViewPort.movedBy(0, LobbyTransition.value() * -50));
            LobbyStartWithSelect.draw(RGB(61, 92, 255));
            LobbyStartWithTyping.draw(RGB(56, 233, 232));
            // RGB(56, 130, 232)

            Rect(LobbyStartWithSelect.x,LobbyStartWithSelect.y+LobbyStartWithSelect.h-50, LobbyStartWithSelect.w, 50).rounded(0,0,8,8).draw(RGB(56, 130, 232));
            Rect(LobbyStartWithTyping.x,LobbyStartWithTyping.y+LobbyStartWithTyping.h-50, LobbyStartWithTyping.w, 50).rounded(0,0,8,8).draw(RGB(73, 196, 255));

            Vec2 SelectHeadCenter = Rect(LobbyStartWithSelect.x,LobbyStartWithSelect.y+LobbyStartWithSelect.h-50, LobbyStartWithSelect.w, 50).center();
            Vec2 TypingHeadCenter = Rect(LobbyStartWithTyping.x,LobbyStartWithTyping.y+LobbyStartWithTyping.h-50, LobbyStartWithTyping.w, 50).center();

            FontAsset(U"Lobby")(U"選択モード").drawAt(SelectHeadCenter);
            FontAsset(U"Lobby")(U"タイピングモード").drawAt(TypingHeadCenter);

            Vec2 SelectIconCenter = Rect(LobbyStartWithSelect.x, LobbyStartWithSelect.y, LobbyStartWithSelect.w, LobbyStartWithSelect.h - 50).center();
            Vec2 TypingIconCenter = Rect(LobbyStartWithTyping.x, LobbyStartWithTyping.y, LobbyStartWithTyping.w, LobbyStartWithTyping.h - 50).center();

            list.drawAt(SelectIconCenter);
            keybord.drawAt(TypingIconCenter);

            SimpleGUI::Button(U"🔽", Vec2(300, 30), 50, false);
            SimpleGUI::Button(U"🔼", Vec2(400, 30), 50, false);
            FontAsset(U"Lobby")(LobbyMaxNumberProblem.text).drawAt(375, 50, Palette::Black);
            FontAsset(U"Lobby")(U"問題数").drawAt(130, 50, Palette::Black);
            FontAsset(U"Lobby")(U"問題形式").drawAt(130, 120, Palette::Black);
            SimpleGUI::RadioButtons(getData().ProblemType, {U"日本語→英語", U"英語→日本語"}, Vec2(300, 80), unspecified, false);
        }
    }

    // }
}


/* other ---------------*/

void MainSystem::LoadWords(String FolderName){
    // 単語の読み込み
    JSONReader json(U"data/"+FolderName+U".json");
    getData().Table[FolderName].value.clear();
    for(const auto &word : json.objectView()){
        Word w;
        w.English=word.value[U"English"].get<String>();
        w.Japanese=word.value[U"Japanese"].get<String>();
        w.EnglishEx=word.value[U"EnglishEx"].get<String>();
        w.JapaneseEx=word.value[U"JapaneseEx"].get<String>();
        w.EnExHeight=w.JaExHeight=40;
        w.ScrollMax=0;
        w.id=Parse<int32>(word.name);
        getData().Table[FolderName].value.emplace(w.English, w);
        getData().Table[FolderName].ids << w.id;
    }
    getData().Table[FolderName].ids.sort();
    json.close();
}

void MainSystem::RemoveWord(){
    // メモリから削除
    getData().Table[SelectingFolder].value.erase(SelectingWord);

    SelectingWord=U"---";

    // JSONの再構成・保存
    JSONWriter json;
    json.setIndent(' ', 4);

    json.startObject();
    {
        for(const auto &i : getData().Table[SelectingFolder].value){
            const Word &w=i.second;
            json.key(ToString(w.id)).startObject();
            {
                json.key(U"English").write(w.English);
                json.key(U"Japanese").write(w.Japanese);
                json.key(U"EnglishEx").write(w.EnglishEx);
                json.key(U"JapaneseEx").write(w.JapaneseEx);
            }
            json.endObject();
        }
    }
    json.endObject();

    json.save(U"data/"+SelectingFolder+U".json");

    // ウィンドウ切り替え
    DetailActive=false;
    Layers.pop();
}

void MainSystem::RemoveFolder(){
    // メモリから削除
    getData().Table.erase(SelectingFolder);

    // ついでにファイルも削除
    FileSystem::Remove(U"data/"+SelectingFolder+U".json");

    // 初期化
    SelectingFolder=getData().Table.begin()->first;
}