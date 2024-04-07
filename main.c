#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_video.h>

SDL_Window *window;
SDL_Renderer *renderer;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
int isRunning = SDL_TRUE;
typedef enum Direction { UP, DOWN, LEFT, RIGHT } Direction;

// game state
SDL_Rect player = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 100, 100};
Direction currDirection = LEFT;

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
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);
  SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRect(renderer, &player);
  SDL_RenderPresent(renderer);
};

// TODO: implement deltatime
void update() {
  switch (currDirection) {
  case UP:
    player.y--;
    break;
  case DOWN:
    player.y++;
    break;
  case LEFT:
    player.x--;
    break;
  case RIGHT:
    player.x++;
    break;
  }
};

int main(int argc, char **arv) {
  SDL_Init(SDL_INIT_EVERYTHING);
  window =
      SDL_CreateWindow("Snake Game", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

  // draw a rect
  while (isRunning) {
    processInput();
    update();
    render();
  };

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
};
