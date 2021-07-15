#include "TypingResult.h"

TypingResult::TypingResult(const InitData& init) : IScene(init){
    TpS = getData().CorrectTypeCount / (double)getData().TypingStopwatch;

    RetryButton = RoundRect(45, 500, 360, 60, 30);
    ToHomeButton = RoundRect(495, 500, 360, 60, 30);

    WordsResultViewPort = Rect(45, 100, GameInfo::Width - 90, GameInfo::Height / 2 - 100);
    WordsResultScroll = 0;
}

/*update---------------------------------------------------------------------*/
void TypingResult::update() {
    ClearPrint();
    Print << Cursor::Pos();
    Print << U"Typed Count:" << getData().CorrectTypeCount;
    Print << U"All Type:" << getData().AllTypeCount;
    Print << U"TpS:" << TpS;
    Print << U"Number of Problems:" << getData().NumberProblem;
    Print << U"ACCount:" << getData().ACCount;

    if (WordsResultViewPort.mouseOver()){
        WordsResultScroll -= Mouse::Wheel() * 10;
        WordsResultScroll = Min(0, Max(-WordsResultScrollMax, WordsResultScroll));
    }

    if(RetryButton.mouseOver() || ToHomeButton.mouseOver()) Cursor::RequestStyle(CursorStyle::Hand);

    if (RetryButton.leftReleased()){
        changeScene(U"Typing");
    }
    
    if(ToHomeButton.leftReleased()){
        changeScene(U"MainSystem");
    }
}

/*draw---------------------------------------------------------------------*/
void TypingResult::draw() const {
    Rect(0,0,GameInfo::Width, 80).draw(Palette::White); // ヘッダー
    Line(0,80,GameInfo::Width, 80).draw(2,RGB(78,141,230)); // ヘッダー2
    FontAsset(U"Heading")(U"テスト結果").drawAt(GameInfo::Width/2, 40, Palette::Black); // ヘッダー文字

    WordsResultViewPort.drawFrame(0, 2, Palette::Gray);
    {
        // ビューポートの適用
        const ScopedViewport2D viewport(WordsResultViewPort);
        const Transformer2D t(Mat3x2::Translate(0, WordsResultScroll), true);

        for(int32 i=0; auto problem : getData().ProblemSet){
            ColorF FontColor;
            if (problem.Result == 0) FontColor = RGB(92, 184, 92);
            else if (problem.Result == 1) FontColor = RGB(240, 173, 78);
            else FontColor = RGB(199, 199, 199);

            FontAsset(U"ItemName")(problem.m_question + U":" + problem.m_answer).draw(10, 10 + i * (FontAsset(U"ItemName").height()+5), FontColor);
            ++i;
        }
    }


    Vec2 penPos(400, GameInfo::Height / 2);
    // 文字単位で描画を制御するためのループ
    for (const auto& glyph : FontAsset(U"DetailDetail")(U"緑:正解 黄:ヒントあり 灰:不正解")){
        // 何文字目かに応じて色を変える
        ColorF color;

        if(glyph.index < 4) color = RGB(92, 184, 92);
        else if (glyph.index < 12) color = RGB(240, 173, 78);
        else color = RGB(199, 199, 199);

        // 文字のテクスチャをペンの位置に文字ごとのオフセットを加算して描画
        glyph.texture.draw(penPos + glyph.offset, color);

        // ペンの X 座標を文字の幅の分進める
        penPos.x += glyph.xAdvance;
    }


    Rect ScoreArea(WordsResultViewPort.x + WordsResultViewPort.w - 200, 100, 200, 90);
    ColorF DetailColor = RGB(73, 101, 252);
    ScoreArea.draw(Palette::White).drawFrame(0, 2, Palette::Gray);
    FontAsset(U"DetailDetail")(U"{}/{}問正解\n({:.2f}%)"_fmt(getData().ACCount, getData().NumberProblem, (double)getData().ACCount / getData().NumberProblem * 100)).drawAt(ScoreArea.center(), Palette::Black);
    FontAsset(U"DetailDetail")(U"正しく打った回数").drawAt(180, GameInfo::Height / 2 + 70, Palette::Black);
    FontAsset(U"ItemName")(U"{}回"_fmt(getData().CorrectTypeCount)).drawAt(180, GameInfo::Height / 2 + 140, DetailColor);

    FontAsset(U"DetailDetail")(U"平均タイプ数").drawAt(450, GameInfo::Height / 2 + 70, Palette::Black);
    FontAsset(U"ItemName")(U"{:.2f}回/秒"_fmt(TpS)).drawAt(450, GameInfo::Height / 2 + 140, DetailColor);

    FontAsset(U"DetailDetail")(U"ミスタイプ数").drawAt(720, GameInfo::Height / 2 + 70, Palette::Black);
    FontAsset(U"ItemName")(U"{}回"_fmt(getData().AllTypeCount - getData().CorrectTypeCount)).drawAt(720, GameInfo::Height / 2 + 140, DetailColor);

    RetryButton.draw(Palette::White);
    RetryButton.drawFrame(2, 0, RGB(73, 195, 252));
    if(RetryButton.mouseOver()) RetryButton.draw(RGB(73, 195, 252, 0.3));
    FontAsset(U"ItemName")(U"リトライ").drawAt(RetryButton.center(), RGB(78, 141, 230));
    ToHomeButton.draw(RGB(73, 195, 252));
    FontAsset(U"ResultToHomeButton")(U"ホームに戻る").drawAt(ToHomeButton.center(), Palette::White);
}