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
  SDL_Rect head;        // 16 bytes
  struct Segment *next; // 8 bytes
} Segment;

SDL_Rect init_head = {SCREEN_WIDTH - PLAYER_SIZE, SCREEN_HEIGHT - PLAYER_SIZE,
                      PLAYER_SIZE, PLAYER_SIZE};
SDL_Rect testing_seg = {SCREEN_WIDTH, SCREEN_HEIGHT - PLAYER_SIZE, PLAYER_SIZE,
                        PLAYER_SIZE};
Segment *snake;

SDL_Rect food = {0, 0, PLAYER_SIZE, PLAYER_SIZE};
Direction currDirection = LEFT;
const int MOVE_SPEED = PLAYER_SIZE;

// void eatFood() {
//   // In Player:
//   // head gets a new SDL_Rect
//   // tail becomes old head
//   SDL_Rect old_head = snake->head;
//   while (snake->next == NULL) {
//     snake->next = malloc(sizeof(Segment));
//     snake->next->head = old_head;
//   }
// };

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
  Segment *currSeg = snake;
  SDL_RenderFillRect(renderer, &snake->head);
  while (currSeg != NULL) {
    SDL_RenderFillRect(renderer, &currSeg->head);
    currSeg = currSeg->next;
  };
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRect(renderer, &food);
  SDL_RenderPresent(renderer);
};

void update(Uint64 deltaTime) {
  switch (currDirection) {
  case UP:
    snake->head.y -= MOVE_SPEED;
    break;
  case DOWN:
    snake->head.y += MOVE_SPEED;
    break;
  case LEFT:
    snake->next->head.x = snake->head.x;
    snake->head.x -= MOVE_SPEED;
    break;
  case RIGHT:
    snake->head.x += MOVE_SPEED;
    break;
  default:
    // do nothing
    break;
  }

  // TODO: don't think i need to worry about body collisions?
  if (SDL_HasIntersection(&snake->head, &food)) {
    SDL_Log("food and player are intersecting\n");
    // eatFood();
  };
};

int main(int argc, char **arv) {
  SDL_Init(SDL_INIT_EVERYTHING);
  window =
      SDL_CreateWindow("Snake Game", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

  // initialize player
  snake = malloc(sizeof(Segment));
  snake->head = init_head;
  snake->next = malloc(sizeof(Segment));
  snake->next->head = testing_seg;

  while (isRunning) {
    Uint64 currTicks = SDL_GetTicks64();
    Uint64 deltaTime = currTicks - ticksElapsed;
    ticksElapsed = SDL_GetTicks64();

    processInput();
    update(deltaTime);
    render();

    SDL_Delay(1000 / 2);

    SDL_Log("%d\n", snake->head.x);
    SDL_Log("%d\n", snake->next->head.x);
  };

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
};
