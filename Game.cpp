#include "Game.h"
#include <cstdlib>

const int thickness = 15;
const float paddleHeight = 100.0f;
const int windowWidth = 1024;
const int windowHeight = 768;
const int offset = 50;
const int numOfBalls = 5;

Game::Game():
	mIsRunning(true),
	mWindow(nullptr),
	mRenderer(nullptr),
	mLeftPaddle{},
	mRightPaddle{},
	mTicksCount{},
	mLeftPaddleDirection{},
	mRightPaddleDirection{},
	mNumOfBalls(numOfBalls)
{
}

bool Game::Initialize()
{
	// Initialize SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	// Create SDL Window
	mWindow = SDL_CreateWindow(
		"Multi Ball Pong",
		100,
		100,
		windowWidth,
		windowHeight,
		0
	);
	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	// Create SDL renderer
	mRenderer = SDL_CreateRenderer(
		mWindow,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}

	// Initial left paddle location
	mLeftPaddle.x = offset;
	mLeftPaddle.y = (windowHeight / 2.0f) - (paddleHeight / 2.0f);

	// Initial right paddle location
	mRightPaddle.x = windowWidth - thickness - offset;
	mRightPaddle.y = (windowHeight / 2.0f) - (paddleHeight / 2.0f);

	// Create balls
	for (int i = 0; i < mNumOfBalls; ++i) {
		Ball ball{};

		ball.position = { (windowWidth / 2.0f) - thickness, (windowHeight / 2.0f) - thickness };

		// Set random velocity
		float vx = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 300.0f - 100.0f; // Random float between -100 and 200
		float vy = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 300.0f - 100.0f; // Random float between -100 and 200
		ball.velocity = { vx, vy };

		mBalls.push_back(ball);
	}


	return true;
}

void Game::RunLoop()
{
	while (this->mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::Shutdown()
{
	// Destroy the rendering context for a window and free associated textures
	SDL_DestroyRenderer(mRenderer);
	// Destroy a window
	SDL_DestroyWindow(mWindow);
	// Clean up all initialized subsystems
	SDL_Quit();
}

void Game::ProcessInput()
{
	// General event structure
	SDL_Event event;
	// Poll for currently pending events.
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// If we get an SDL_QUIT event, end loop
			// Clean up all initialized subsystems.
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}

	// Get state of keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);

	// If escape is pressed end loop
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	// Update left paddle direction based on W/S keys
	mLeftPaddleDirection = 0;
	if (state[SDL_SCANCODE_W])
	{
		mLeftPaddleDirection -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		mLeftPaddleDirection += 1;
	}

	// Update right paddle direction based on UP/DOWN keys
	mRightPaddleDirection = 0;
	if (state[SDL_SCANCODE_UP])
	{
		mRightPaddleDirection -= 1;
	}
	if (state[SDL_SCANCODE_DOWN])
	{
		mRightPaddleDirection += 1;
	}
}

void Game::UpdateGame()
{
	// 30fps
	float fps30 = 1.0f / 30.0f; // 0.03333333333 seconds per frame
	// 60fps
	float fps60 = 1.0f / 60.0f; // 0.01666666666 seconds per frame

	// Wait until 16ms (.016 * 1000) has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + (fps60 * 1000.0f)));

	// Delta time is the difference in ticks from last frame in seconds
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;

	// Limit maximum delta time
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// Update left paddle position based on direction
	if (mLeftPaddleDirection != 0.0f)
	{
		// Roughly 300 pixels/sec
		// Roughly 4.8 pixels a frame
		mLeftPaddle.y += mLeftPaddleDirection * 300.0f * deltaTime;
		// If left paddle hits top screen
		if (mLeftPaddle.y < thickness)
		{
			// Keep it in place
			mLeftPaddle.y = thickness;
		}
		// If left paddle hits bottom screen
		else if (mLeftPaddle.y > 768.0f - paddleHeight - thickness)
		{
			// Keep it in place
			mLeftPaddle.y = 768.0f - paddleHeight - thickness;
		}
	}

	// Update right paddle position based on direction
	if (mRightPaddleDirection != 0.0f)
	{
		// Roughly 300 pixels/sec
		// Roughly 4.8 pixels a frame
		mRightPaddle.y += mRightPaddleDirection * 300.0f * deltaTime;
		// If right paddle hits top screen
		if (mRightPaddle.y < thickness)
		{
			// Keep it in place
			mRightPaddle.y = thickness;
		}
		// If right paddle hits bottom screen
		else if (mRightPaddle.y > 768.0f - paddleHeight - thickness)
		{
			// Keep it in place
			mRightPaddle.y = 768.0f - paddleHeight - thickness;
		}
	}

	// For each ball
	for (Ball& ball : mBalls) {
		// Update ball position based on velocity and delta time
		ball.position.x += ball.velocity.x * deltaTime;
		ball.position.y += ball.velocity.y * deltaTime;

		if (
			// If Ball is moving to the left
			ball.velocity.x < 0.0f &&
			// And hits left paddle x
			ball.position.x <= offset + thickness &&
			// And hits left paddle y
			mLeftPaddle.y - ball.position.y <= thickness &&
			mLeftPaddle.y - ball.position.y >= -paddleHeight)
		{
			// Change velocity
			ball.velocity.x *= -1.0f;
		}
		else if (
			// If ball is moving to the right
			ball.velocity.x > 0.0f &&
			// And hits right paddle x
			ball.position.x >= windowWidth - offset - 2.0f * thickness &&
			// And hits right paddle y
			mRightPaddle.y - ball.position.y <= thickness &&
			mRightPaddle.y - ball.position.y >= -paddleHeight)
		{
			// Change velocity
			ball.velocity.x *= -1.0f;
		}
		// If ball hits the left screen
		else if (ball.position.x <= 0.0f)
		{
			// End game
			mIsRunning = false;
		}
		// If ball hits the right screen
		else if (ball.position.x >= windowWidth + thickness)
		{
			// End game
			mIsRunning = false;
		}
		// If ball hits top wall
		else if (ball.position.y <= thickness && ball.velocity.y < 0.0f)
		{
			// Change velocity
			ball.velocity.y *= -1.0f;
		}
		// If ball hits bottom wall
		else if (ball.position.y >= (windowHeight - thickness - thickness / 2.0f) &&
			ball.velocity.y > 0.0f)
		{
			// Change velocity
			ball.velocity.y *= -1.0f;
		}
	}

	// Update tick counts
	mTicksCount = SDL_GetTicks();
}

void Game::GenerateOutput()
{
	// Set background color
	SDL_SetRenderDrawColor(mRenderer, 110, 195, 212, 255);
	// Clear the current rendering target with the drawing color
	SDL_RenderClear(mRenderer);

	// Set wall color
	SDL_SetRenderDrawColor(mRenderer, 163, 230, 170, 255);
	// Create and position top wall
	SDL_Rect topWall{ 0, 0, windowWidth, thickness };
	// Draw top wall
	SDL_RenderFillRect(mRenderer, &topWall);
	// Create and position bottom wall
	SDL_Rect bottomWall{ 0, windowHeight - thickness, windowWidth, thickness };
	// Draw bottom wall
	SDL_RenderFillRect(mRenderer, &bottomWall);

	// Set paddle color
	SDL_SetRenderDrawColor(mRenderer, 184, 118, 81, 255);
	// Create and position left paddle
	SDL_Rect paddleLeft{
		static_cast<int>(mLeftPaddle.x),
		static_cast<int>(mLeftPaddle.y),
		thickness,
		static_cast<int>(paddleHeight)
	};
	// Draw left paddle
	SDL_RenderFillRect(mRenderer, &paddleLeft);

	// Create and position right paddle
	SDL_Rect paddleRight{
		static_cast<int>(mRightPaddle.x),
		static_cast<int>(mRightPaddle.y),
		thickness,
		static_cast<int>(paddleHeight)
	};
	// Draw right paddle
	SDL_RenderFillRect(mRenderer, &paddleRight);

	// Set ball color
	SDL_SetRenderDrawColor(mRenderer, 232, 139, 167, 255);
	// For each ball
	for (const Ball& ball : mBalls) {
		// Create and position ball
		SDL_Rect ballRect;
		ballRect.x = static_cast<int>(ball.position.x);
		ballRect.y = static_cast<int>(ball.position.y);
		ballRect.w = thickness;
		ballRect.h = thickness;

		// Set render color
		SDL_SetRenderDrawColor(mRenderer, 232, 139, 167, 255);

		// Render ball
		SDL_RenderFillRect(mRenderer, &ballRect);
	}

	// Update the screen with any rendering performed since the previous call
	SDL_RenderPresent(mRenderer);
}
