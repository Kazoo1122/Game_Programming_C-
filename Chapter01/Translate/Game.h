#pragma once
#include "SDL2/SDL.h"

// Vector2構造体はX/Y座標を保存するもの
// (現在値を取る)
struct Vector2
{
  float x;
  float y;
};

// Gameクラス
class Game
{
public:
  Game();
  // ゲームを初期化する
  bool Initialize();
  // ゲームが終わるまでループを稼働させる
  void RunLoop();
  // ゲームを終了する
  void Shutdown();

private:
  // ゲームループのためのヘルパー関数
  void ProcessInput();
  void UpdateGame();
  void GenerateOutput();

  // SDLを使ってウィンドウを生成する
  SDL_Window *mWindow;
  // 描写をレンダリングする
  SDL_Renderer *mRenderer;
  // ゲーム開始から起算したチック数
  Uint32 mTicksCount;
  // ゲームを続けるかどうか
  bool mIsRunning;

  // Pong(ポン)の仕様
  // ラケットの進行方向
  int mPaddleDir;
  // ラケットの位置
  Vector2 mPaddlePos;
  // ボールの位置
  Vector2 mBallPos;
  // ボールの速度
  Vector2 mBallVel;
};