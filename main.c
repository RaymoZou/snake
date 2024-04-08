#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>

SDL_Window *window;
SDL_Renderer *renderer;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const float PLAYER_SIZE = 25.0f;
Uint64 ticksElapsed;
int isRunning = SDL_TRUE;
typedef enum Direction { UP, DOWN, LEFT, RIGHT } Direction;

// game state
SDL_FRect player = {SCREEN_WIDTH - PLAYER_SIZE, SCREEN_HEIGHT - PLAYER_SIZE,
                    PLAYER_SIZE, PLAYER_SIZE};
SDL_FRect food = {0, 0, PLAYER_SIZE, PLAYER_SIZE};
Direction currDirection = LEFT;
const float MOVE_SPEED = 0.20f;

void processInput() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.scancode) {
      case SDL_SCANCODE_UP:
        currDirection = UP;
        break;
      case SDL_SCANCODE_DOWN:
        currDirection = DOWN;
        break;
      case SDL_SCANCODE_RIGHT:
        currDirection = RIGHT;
        break;
      case SDL_SCANCODE_LEFT:
        currDirection = LEFT;
        break;
      case SDL_SCANCODE_ESCAPE:
        isRunning = SDL_FALSE;
        break;
      default:
        break;
      }
    };
  };
}

void render() {
  // clear frame buffer
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);
  // draw player
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRectF(renderer, &player);
  // draw food
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRectF(renderer, &food);
  // present buffer
  SDL_RenderPresent(renderer);
};

void update(Uint64 deltaTime) {
  switch (currDirection) {
  case UP:
    player.y -= MOVE_SPEED * deltaTime;
    break;
  case DOWN:
    player.y += MOVE_SPEED * deltaTime;
    break;
  case LEFT:
    player.x -= MOVE_SPEED * deltaTime;
    break;
  case RIGHT:
    player.x += MOVE_SPEED * deltaTime;
    break;
  default:
    // do nothing
    break;
  }

  // TODO: replace with SDL_HasIntersectionF
  if (SDL_HasIntersectionF(&player, &food)) {
    SDL_Log("food and player are intersecting uwu\n");
  };
};

int main(int argc, char **arv) {
  SDL_Init(SDL_INIT_EVERYTHING);
  window =
      SDL_CreateWindow("Snake Game", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

  // draw a rect
  while (isRunning) {
    // get deltatime
    Uint64 currTicks = SDL_GetTicks64();
    Uint64 deltaTime = currTicks - ticksElapsed;
    ticksElapsed = SDL_GetTicks64();

    processInput();
    update(deltaTime);
    render();
  };

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
};
