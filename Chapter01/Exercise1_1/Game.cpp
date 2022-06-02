#include "Game.h"

const int thickness = 15;
const float paddleH = 100.0f;

Game::Game()
    : mWindow(nullptr),
      mRenderer(nullptr),
      mTicksCount(0),
      mIsRunning(true),
      mLeftPaddleDir(0),
      mRightPaddleDir(0)
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
        "Game Programming in C++ (第1章) 課題1.1", // ウィンドウのタイトル
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

    // ボールの初期位置　画面中央(追加補足)
    mBallPos.x = 1024.0f / 2.0f;
    mBallPos.y = 768.0f / 2.0f;

    // 左側のラケットの初期位置　画面左中央(追加補足)
    mLeftPaddlePos.x = 10.0f;
    mLeftPaddlePos.y = 768.0f / 2.0f;

    // 右側のラケットの初期位置　画面左中央(追加補足)
    mRightPaddlePos.x = 1024.0f - 25.0f;
    mRightPaddlePos.y = 768.0f / 2.0f;

    // ボールの初期進行速度　左下方向に向かって進む(追加補足)
    mBallVel.x = -200.0f;
    mBallVel.y = 235.0f;
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

    // 左側のラケットはW/Sキーに基づいてラケットの進行方向を更新する
    mLeftPaddleDir = 0;
    if (state[SDL_SCANCODE_W])
    {
        mLeftPaddleDir -= 1;
    }
    if (state[SDL_SCANCODE_S])
    {
        mLeftPaddleDir += 1;
    }

    // 右側のラケットはI/Kキーに基づいてラケットの進行方向を更新する
    mRightPaddleDir = 0;
    if (state[SDL_SCANCODE_I])
    {
        mRightPaddleDir -= 1;
    }
    if (state[SDL_SCANCODE_K])
    {
        mRightPaddleDir += 1;
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

    // 進行方向に基づいて左側ラケットの位置を更新する
    if (mLeftPaddleDir != 0)
    {
        mLeftPaddlePos.y += mLeftPaddleDir * 400.0f * deltaTime;
        // ラケットが画面外に動かないか確かめてみよう！
        // ラケットの位置が上部の壁を超えようとする場合は(追加補足)
        if (mLeftPaddlePos.y < (paddleH / 2.0f + thickness))
        {
            // ラケットの長さの半分＋壁の厚み以上には行かないようにする(追加補足)
            mLeftPaddlePos.y = paddleH / 2.0f + thickness;
        }
        // ラケットの位置が下部の壁を超えようとする場合は(追加補足)
        else if (mLeftPaddlePos.y > (768.0f - paddleH / 2.0f - thickness))
        {
            // 画面全体の高さ - ラケットの長さの半分 - 壁の厚み以下には行かないようにする(追加補足)
            mLeftPaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
        }
    }
    // 進行方向に基づいて右側ラケットの位置を更新する
    if (mRightPaddleDir != 0)
    {
        mRightPaddlePos.y += mRightPaddleDir * 400.0f * deltaTime;
        // ラケットが画面外に動かないか確かめてみよう！
        // ラケットの位置が上部の壁を超えようとする場合は(追加補足)
        if (mRightPaddlePos.y < (paddleH / 2.0f + thickness))
        {
            // ラケットの長さの半分＋壁の厚み以上には行かないようにする(追加補足)
            mRightPaddlePos.y = paddleH / 2.0f + thickness;
        }
        // ラケットの位置が下部の壁を超えようとする場合は(追加補足)
        else if (mRightPaddlePos.y > (768.0f - paddleH / 2.0f - thickness))
        {
            // 画面全体の高さ - ラケットの長さの半分 - 壁の厚み以下には行かないようにする(追加補足)
            mRightPaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
        }
    }
    // ボールの速度に従ってボールの位置を更新する
    mBallPos.x += mBallVel.x * deltaTime;
    mBallPos.y += mBallVel.y * deltaTime;

    // 必要に応じてボールをバウンドさせる
    // ラケットと交差したかどうか？
    float leftDiff = mLeftPaddlePos.y - mBallPos.y;
    // 差分の絶対値を取る
    leftDiff = (leftDiff > 0.0f) ? leftDiff : -leftDiff;
    // 必要に応じてボールをバウンドさせる
    // ラケットと交差したかどうか？
    float rightDiff = mRightPaddlePos.y - mBallPos.y;
    // 差分の絶対値を取る
    rightDiff = (rightDiff > 0.0f) ? rightDiff : -rightDiff;
    if (
        // (ラケットとボールとの)Y座標の差は十分小さくなっているか
        leftDiff <= paddleH / 2.0f
        // (ラケットの)X座標は正確な位置にあるか
        && 20.0f <= mBallPos.x && mBallPos.x <= 25.0f &&
        // ボールは左に向かって動いているか
        mBallVel.x < 0.0f)
    {
        // 上記を満たす場合はボールの左右の向きを反転(バウンド)させる(追加補足)
        mBallVel.x *= -1.0f;
    }
    else if (
        rightDiff <= paddleH / 2.0f
        // (ラケットの)X座標は正確な位置にあるか
        && 999.0f <= mBallPos.x && mBallPos.x <= 1004.0f &&
        // ボールは左に向かって動いているか
        mBallVel.x >= 0.0f)
    {
        // 上記を満たす場合はボールの左右の向きを反転(バウンド)させる(追加補足)
        mBallVel.x *= -1.0f;
    }
    // ボールはスクリーン外に出てしまったか？(そうであれば、ゲーム終了となる)
    else if (mBallPos.x <= 0.0f || mBallPos.x > 1024.0f)
    {
        mIsRunning = false;
    }
    // ボール上部はの壁にぶつかったか？
    else if (mBallPos.y <= thickness && mBallVel.y < 0.0f)
    {
        mBallVel.y *= -1.0f;
    }
    // ボールは下部の壁にぶつかったか？
    else if (
        mBallPos.y >= (768 - thickness) && mBallVel.y > 0.0f)
    {
        mBallVel.y *= -1.0f;
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

    // なんとなく勝手に色変
    SDL_SetRenderDrawColor(mRenderer, 255, 255, 64, 255);

    // ラケットを描写する
    SDL_Rect leftPaddle{
        static_cast<int>(mLeftPaddlePos.x),
        static_cast<int>(mLeftPaddlePos.y - paddleH / 2),
        thickness,
        static_cast<int>(paddleH)};
    SDL_RenderFillRect(mRenderer, &leftPaddle);

    SDL_Rect rightPaddle{
        static_cast<int>(mRightPaddlePos.x),
        static_cast<int>(mRightPaddlePos.y - paddleH / 2),
        thickness,
        static_cast<int>(paddleH)};
    SDL_RenderFillRect(mRenderer, &rightPaddle);

    // ボールを描写する
    SDL_Rect ball{
        static_cast<int>(mBallPos.x - thickness / 2),
        static_cast<int>(mBallPos.y - thickness / 2),
        thickness,
        thickness};
    SDL_RenderFillRect(mRenderer, &ball);

    // フロントバッファ(描写済)とバックバッファ(描写前)を交換する
    SDL_RenderPresent(mRenderer);
}

void Game::Shutdown()
{
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}
