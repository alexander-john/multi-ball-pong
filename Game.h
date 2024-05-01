#pragma once
#include "SDL.h"
#include <vector>

// Vector struct
struct Vector2
{
	float x;
	float y;
};

// Ball struct
struct Ball
{
	Vector2 position;
	Vector2 velocity;
};

class Game
{
public:
	Game();

	// Initialize the game
	bool Initialize();

	// Runs game loop until game is over
	void RunLoop();

	// Shutdown game
	void Shutdown();

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	// Window created by SDL
	SDL_Window* mWindow;

	// Renderer for 2D drawing
	SDL_Renderer* mRenderer;

	// Is game running?
	bool mIsRunning;

	// Left paddle
	Vector2 mLeftPaddle;

	// Right paddle
	Vector2 mRightPaddle;

	// Ticks since start of game
	Uint32 mTicksCount;

	// Direction of left paddle
	int mLeftPaddleDirection;

	// Direction of right paddle
	int mRightPaddleDirection;

	// Ball vector
	std::vector<Ball> mBalls;

	// Balls to create
	int mNumOfBalls;
};