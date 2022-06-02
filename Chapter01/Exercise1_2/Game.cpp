#include "Game.h"

const int thickness = 15;
const float paddleH = 100.0f;

Game::Game()
    : mWindow(nullptr),
      mRenderer(nullptr),
      mTicksCount(0),
      mIsRunning(true),
      mPaddleDir(0)
{
}

bool Game::Initialize()
{
    // SDLを初期化する
    int sdlResult = SDL_Init(SDL_INIT_VIDEO);
    if (sdlResult != 0)
    {
        SDL_Log("SDLを初期化できません: %s", SDL_GetError());
        return false;
    }

    // SDLのウィンドウを生成する
    mWindow = SDL_CreateWindow(
        "Game Programming in C++ (第1章) 課題1.2", // ウィンドウのタイトル
        100,                                       // 画面左上を起点としたウィンドウのX座標位置
        100,                                       // 画面左上を起点としたウィンドウのY座標位置
        1024,                                      // ウィンドウの幅サイズ
        768,                                       // ウィンドウの高さサイズ
        0                                          // フラグ(0はフラグなし)
    );

    if (!mWindow)
    {
        SDL_Log("ウィンドウの生成に失敗しました: %s", SDL_GetError());
        return false;
    }

    // SDLのレンダラーを生成
    mRenderer = SDL_CreateRenderer(
        mWindow,                        // レンダラーの生成のために指定するウィンドウ
        -1,                             // 通常は-1を設定する
        SDL_RENDERER_ACCELERATED        // ハードウェア・アクセラレーションを使用(追加補足)
            | SDL_RENDERER_PRESENTVSYNC // 垂直同期を有効(追加補足)
    );

    if (!mRenderer)
    {
        SDL_Log("レンダラーの生成に失敗しました: %s", SDL_GetError());
        return false;
    }

    // ボールそれぞれに値設定
    for (int i = 0; i < BALLS; i++)
    {
        std::mt19937 gen(rnd());                         // 擬似乱数生成エンジンの準備
        std::uniform_int_distribution<> dist(-300, 300); // ボールの初期の進行方向をランダムにする
        // ボールの初期位置　画面中央(追加補足)
        mBallPos[i].x = 1024.0f / 2.0f;
        mBallPos[i].y = 768.0f / 2.0f;

        // ボールの初期進行速度　左下方向に向かって進む(追加補足)
        mBallVel[i].x = (float)dist(gen);
        mBallVel[i].y = (float)dist(gen);
    }

    // ラケットの初期位置　画面左中央(追加補足)
    mPaddlePos.x = 10.0f;
    mPaddlePos.y = 768.0f / 2.0f;

    return true;
};

void Game::RunLoop()
{
    while (mIsRunning)
    {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        // もしSDL_QUITイベントを受け取ったら、ループを終了する
        case SDL_QUIT:
            mIsRunning = false;
            break;
        }
    }

    // キーボード入力の受付をスタートする
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    // エスケープを押したもループ終了する
    if (state[SDL_SCANCODE_ESCAPE])
    {
        mIsRunning = false;
    }

    // W/Sキーに基づいてラケットの進行方向を更新する
    mPaddleDir = 0;
    if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP])
    {
        mPaddleDir -= 1;
    }
    if (state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN])
    {
        mPaddleDir += 1;
    }
}

void Game::UpdateGame()
{
    // 直近のフレームから16ミリ秒経過するまで待機
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
        ;

    // デルタタイムは直近のフレームから経過したシステム時間(チック)のこと
    // (秒数に変換される)
    float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;

    // デルタタイムの最高値を超えないよう値を抑える
    if (deltaTime > 0.05f)
    {
        deltaTime = 0.05f;
    }

    // チック数のカウントを更新する(次のフレームのため)
    mTicksCount = SDL_GetTicks();

    // 進行方向に基づいてラケットの位置を更新する
    if (mPaddleDir != 0)
    {
        mPaddlePos.y += mPaddleDir * 500.0f * deltaTime;
        // ラケットが画面外に動かないか確かめてみよう！
        // ラケットの位置が上部の壁を超えようとする場合は(追加補足)
        if (mPaddlePos.y < (paddleH / 2.0f + thickness))
        {
            // ラケットの長さの半分＋壁の厚み以上には行かないようにする(追加補足)
            mPaddlePos.y = paddleH / 2.0f + thickness;
        }
        // ラケットの位置が下部の壁を超えようとする場合は(追加補足)
        else if (mPaddlePos.y > (768.0f - paddleH / 2.0f - thickness))
        {
            // 画面全体の高さ - ラケットの長さの半分 - 壁の厚み以下には行かないようにする(追加補足)
            mPaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
        }
    }

    // ボールそれぞれに設定
    for (int i = 0; i < BALLS; i++)
    {
        // ボールの速度に従ってボールの位置を更新する
        mBallPos[i].x += mBallVel[i].x * deltaTime;
        mBallPos[i].y += mBallVel[i].y * deltaTime;

        // 必要に応じてボールをバウンドさせる
        // ラケットと交差したかどうか？
        float diff = mPaddlePos.y - mBallPos[i].y;
        // 差分の絶対値を取る
        diff = (diff > 0.0f) ? diff : -diff;
        if (
            // (ラケットとボールとの)Y座標の差は十分小さくなっているか
            diff <= paddleH / 2.0f
            // (ラケットの)X座標は正確な位置にあるか
            && 20.0f <= mBallPos[i].x && mBallPos[i].x <= 25.0f &&
            // ボールは左に向かって動いているか
            mBallVel[i].x < 0.0f)
        {
            // 上記を満たす場合はボールの左右の向きを反転(バウンド)させる(追加補足)
            mBallVel[i].x *= -1.0f;
        }
        // ボールはスクリーン外に出てしまったか？(そうであれば、ゲーム終了となる)
        else if (mBallPos[i].x <= 0.0f)
        {
            mIsRunning = false;
        }
        // ボール上部はの壁にぶつかったか？
        else if (mBallPos[i].x >= (1024.0f - thickness) && mBallVel[i].x > 0.0f)
        {
            mBallVel[i].x *= -1.0f;
        }
        // ボールは右の壁とぶつかったか？
        if (mBallPos[i].y <= thickness && mBallVel[i].y < 0.0f)
        {
            mBallVel[i].y *= -1;
        }
        // ボールは下部の壁にぶつかったか？
        else if (
            mBallPos[i].y >= (768 - thickness) && mBallVel[i].y > 0.0f)
        {
            mBallVel[i].y *= -1;
        }
    }
}

void Game::GenerateOutput()
{
    // 青色を描写色にセットする
    // (とありましたが、そのままではつまらなかったので色変してます)
    SDL_SetRenderDrawColor(
        mRenderer,
        0,   // R(赤)
        128, // G(緑)
        0,   // B(青)
        255  // A(アルファ値・透明度のこと)
    );

    // バックバッファをクリアする
    SDL_RenderClear(mRenderer);

    // 壁を描写する
    SDL_SetRenderDrawColor(mRenderer, 255, 128, 255, 255);

    // 上部の壁を描写する
    SDL_Rect wall{
        0,                                // 画面左上からのX座標の位置
        0,                                // 画面左上からのY座標の位置
        1024,                             // 幅
        thickness};                       // 高さ
    SDL_RenderFillRect(mRenderer, &wall); // 塗りつぶしの長方形を描写(追加補足)

    // 下部の壁を描写する
    wall.y = 768 - thickness;
    SDL_RenderFillRect(mRenderer, &wall);

    // 右の壁を描写する
    wall.x = 1024 - thickness;
    wall.y = 0;
    wall.w = thickness;
    wall.h = 1024;
    SDL_RenderFillRect(mRenderer, &wall);

    // なんとなく勝手に色変
    SDL_SetRenderDrawColor(mRenderer, 255, 255, 64, 255);

    // ラケットを描写する
    SDL_Rect paddle{
        static_cast<int>(mPaddlePos.x),
        static_cast<int>(mPaddlePos.y - paddleH / 2),
        thickness,
        static_cast<int>(paddleH)};
    SDL_RenderFillRect(mRenderer, &paddle);

    for (int i = 0; i < BALLS; i++)
    {
        // ボールを描写する
        SDL_Rect ball{
            static_cast<int>(mBallPos[i].x - thickness / 2),
            static_cast<int>(mBallPos[i].y - thickness / 2),
            thickness,
            thickness};
        SDL_RenderFillRect(mRenderer, &ball);
    }

    // フロントバッファ(描写済)とバックバッファ(描写前)を交換する
    SDL_RenderPresent(mRenderer);
}

void Game::Shutdown()
{
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}
