#include "SDL2/SDL.h"

#include <algorithm>
#include <deque>
#include <vector>
#include <time.h>

const int WIDTH = 720, HEIGHT = 480;
const int CELL_SIZE = 30;
bool running = true;

enum Direction {
	DOWN,
	UP,
	RIGHT,
	LEFT
};

SDL_Rect apple = { WIDTH / 3 * 2, HEIGHT / 2, CELL_SIZE, CELL_SIZE };
struct Snake {
	int size = 3;
	Direction moveDirection = RIGHT;
	SDL_Rect head = { WIDTH / 3, HEIGHT / 2, CELL_SIZE, CELL_SIZE };
	std::deque<SDL_Rect> body;

	void Move() {
		switch (moveDirection) {
		case DOWN: {
			head.y += CELL_SIZE;
		} break;
		case UP: {
			head.y -= CELL_SIZE;
		} break;
		case RIGHT: {
			head.x += CELL_SIZE;
		} break;
		case LEFT: {
			head.x -= CELL_SIZE;
		} break;
		}

		
	}
	void Update() {
		body.push_front(head);
		while (body.size() > size) {
			body.pop_back();
		}
	}
	void DetectCollisions() {
		// Wrap around the edges
		if (head.x >= WIDTH) head.x = 0;
		if (head.x < 0) head.x = WIDTH - CELL_SIZE;
		if (head.y >= HEIGHT) head.y = 0;
		if (head.y < 0) head.y = HEIGHT - CELL_SIZE;

		// Apple
		if (head.x == apple.x && head.y == apple.y) {
			size += 1;
			apple.x = rand() % WIDTH / CELL_SIZE * CELL_SIZE;
			apple.y = rand() % HEIGHT / CELL_SIZE * CELL_SIZE;
		}

		// Self Collision
		std::for_each(body.begin(), body.end(), [&](auto& snakeSegment) {
			if (head.x == snakeSegment.x && head.y == snakeSegment.y) {
				size = 3;
			}
		});
	}
} snake;

void Input() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT: running = false; break;
		case SDL_KEYDOWN: {
			if (event.key.keysym.sym == SDLK_DOWN && snake.moveDirection != UP) {
				snake.moveDirection = DOWN;
			}
			else if (event.key.keysym.sym == SDLK_UP && snake.moveDirection != DOWN) {
				snake.moveDirection = UP;
			}
			else if (event.key.keysym.sym == SDLK_RIGHT && snake.moveDirection != LEFT) {
				snake.moveDirection = RIGHT;
			}
			else if (event.key.keysym.sym == SDLK_LEFT && snake.moveDirection != RIGHT) {
				snake.moveDirection = LEFT;
			}
		} break;
		}
	}
}
void DrawGrid(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
	for (int i = 0; i < WIDTH / CELL_SIZE; ++i) {
		SDL_RenderDrawLine(renderer, i * CELL_SIZE, 0, i * CELL_SIZE, HEIGHT);
	}
	for (int i = 0; i < HEIGHT / CELL_SIZE; ++i) {
		SDL_RenderDrawLine(renderer, 0, i * CELL_SIZE, WIDTH, i * CELL_SIZE);
	}
}
void DrawScene(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(renderer, &apple);

	int snakeColor = 255;
	std::for_each(snake.body.begin(), snake.body.end(), [&](auto& snakeSegment) {
		SDL_SetRenderDrawColor(renderer, 0, snakeColor, 0, 255);
	snakeColor = snakeColor - 10 >= 10 ? snakeColor - 10 : 10;
	SDL_RenderFillRect(renderer, &snakeSegment);
		});

	DrawGrid(renderer);

	SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	auto window = SDL_CreateWindow("SDL Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
	auto renderer = SDL_CreateRenderer(window, -1, 0);
	srand(time(0));

	while (running) {
		Input();
		snake.Move();
		snake.DetectCollisions();
		snake.Update();
		DrawScene(renderer);
		SDL_Delay(120);
	}
	return 0;
}