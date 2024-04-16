#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdlib.h>

SDL_Window *window;
SDL_Renderer *renderer;
const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;
const int PLAYER_SIZE = 25;
Uint64 ticksElapsed;
int isRunning = SDL_TRUE;
typedef enum Direction { UP, DOWN, LEFT, RIGHT } Direction;

// game state
typedef struct Segment {
  SDL_Rect *rect;       // 16 bytes
  struct Segment *next; // 8 bytes
} Segment;

SDL_Rect init_head = {SCREEN_WIDTH - PLAYER_SIZE, SCREEN_HEIGHT - PLAYER_SIZE,
                      PLAYER_SIZE, PLAYER_SIZE};
SDL_Rect testing_seg = {SCREEN_WIDTH, SCREEN_HEIGHT - PLAYER_SIZE, PLAYER_SIZE,
                        PLAYER_SIZE};
SDL_Rect food = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, PLAYER_SIZE, PLAYER_SIZE};

Segment *snake;

Direction currDirection = LEFT;

// problem: the food and head are on top of eachother
void eatFood() {
  SDL_Log("eating food");
  Segment *new_seg = malloc(sizeof(Segment));
  SDL_Rect new_head = food;
  switch (currDirection) {
  case UP:
    new_head.x -= PLAYER_SIZE;
    break;
  case DOWN:
    new_head.y += PLAYER_SIZE;
    break;
  case LEFT:
    new_head.x -= PLAYER_SIZE;
    break;
  case RIGHT:
    new_head.x += PLAYER_SIZE;
    break;
  default:
    break;
  };
  new_seg->rect = malloc(sizeof(SDL_Rect));
  // TODO: copy the value in &new_head -> new_set->rect
  new_seg->next = snake->next;
  snake->next = new_seg;
};

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

// render the player and the food
void render() {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRect(renderer, snake->rect);
  SDL_RenderFillRect(renderer, snake->next->rect);
  // Segment *curr_seg = snake;
  // // while (curr_seg) {
  // //   SDL_RenderFillRect(renderer, curr_seg->rect);
  // //   curr_seg = curr_seg->next;
  // // }
  SDL_RenderFillRect(renderer, &food);
  SDL_RenderPresent(renderer);
};

// TODO: fix game update
void update(Uint64 deltaTime) {
  Segment *curr = snake;
  SDL_Rect prev_rect = *snake->rect;
  switch (currDirection) {
  case UP:
    snake->rect->y -= PLAYER_SIZE;
    break;
  case DOWN:
    snake->rect->y += PLAYER_SIZE;
    break;
  case LEFT:
    snake->rect->x -= PLAYER_SIZE;
    break;
  case RIGHT:
    snake->rect->x += PLAYER_SIZE;
    break;
  default:
    break;
  }

  while (curr->next) {
    curr = curr->next;
    SDL_Rect temp = *curr->rect;
    *curr->rect = prev_rect;
    prev_rect = temp;
  };

  SDL_Log("original: %d\n", snake->rect->x);
  SDL_Log("buffer (modified) : %d\n", prev_rect.x);

  // TODO: don't think i need to worry about body collisions?
  if (SDL_HasIntersection(snake->rect, &food)) {
    eatFood();
  };
};

int getSnakeLength() {
  int count = 0;
  Segment *curr = snake;
  while (curr) {
    SDL_Log("Segment %d x: %d", count, curr->rect->x);
    SDL_Log("Segment %d y: %d", count, curr->rect->y);
    count++;
    curr = curr->next;
  };
  return count;
};

int main(int argc, char **arv) {
  SDL_Init(SDL_INIT_EVERYTHING);
  window =
      SDL_CreateWindow("Snake Game", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

  // segment 1
  snake = malloc(sizeof(Segment));
  snake->rect = malloc(sizeof(SDL_Rect));
  snake->rect = &init_head;
  snake->next = malloc(sizeof(Segment));

  // segment 2
  snake->next->rect = malloc(sizeof(SDL_Rect));
  snake->next->rect = &testing_seg;
  snake->next->next = NULL;

  SDL_Log("1: %d", snake->rect->x);
  SDL_Log("2: %d", snake->next->rect->x);

  while (isRunning) {
    // TODO: don't really need delta time if using fixed framerate
    Uint64 currTicks = SDL_GetTicks64();
    Uint64 deltaTime = currTicks - ticksElapsed;
    ticksElapsed = SDL_GetTicks64();

    processInput();
    update(deltaTime);
    render();

    SDL_Delay(1000 / 4);

    SDL_Log("%d\n", snake->rect->x);
    SDL_Log("%d\n", snake->next->rect->x);
    // SDL_Log("snake length: %d\n", getSnakeLength());
  };

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
};
