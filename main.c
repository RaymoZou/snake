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

// TODO: game restart on collision
// TODO: score UI using SDL_ttf

// game state
typedef struct Segment {
  SDL_Rect rect;
  struct Segment *next;
} Segment;

SDL_Rect init_head = {SCREEN_WIDTH - PLAYER_SIZE, SCREEN_HEIGHT - PLAYER_SIZE,
                      PLAYER_SIZE, PLAYER_SIZE};

// TODO: reset the food to new unoccupied, random position when collected
SDL_Rect food = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, PLAYER_SIZE, PLAYER_SIZE};
Segment *snake;

Direction currDirection = LEFT;

void restart() { 
    SDL_Log("restarting game...\n");
    // Segment *head = snake;
    // Segment *temp;
    // while (head) {
    //     temp = head;
    //     head = head->next;
    //     free(temp);
    // };
};

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
  Segment *curr_seg = snake;
  while (curr_seg) {
    SDL_RenderFillRect(renderer, &curr_seg->rect);
    curr_seg = curr_seg->next;
  }
  SDL_RenderFillRect(renderer, &food);
  SDL_RenderPresent(renderer);
};

// TODO: check for body collisions b/w head and body
void update() {
  Segment *curr = snake;
  SDL_Rect prev_rect = snake->rect;
  switch (currDirection) {
  case UP:
    snake->rect.y -= PLAYER_SIZE;
    break;
  case DOWN:
    snake->rect.y += PLAYER_SIZE;
    break;
  case LEFT:
    snake->rect.x -= PLAYER_SIZE;
    break;
  case RIGHT:
    snake->rect.x += PLAYER_SIZE;
    break;
  default:
    break;
  }

  // check for edge collisions
  if (snake->rect.x<0 | snake->rect.x>= SCREEN_WIDTH) {
    restart();
  };
  if (snake->rect.y<0 | snake->rect.y>= SCREEN_HEIGHT) {
    restart();
  };

  while (curr->next) {
    curr = curr->next;
    SDL_Rect temp = curr->rect;
    curr->rect = prev_rect;
    prev_rect = temp;
  };

  // TODO: don't think i need to worry about body collisions?
  if (SDL_HasIntersection(&snake->rect, &food)) {
    eatFood();
  };
};

int getSnakeLength() {
  int count = 0;
  Segment *curr = snake;
  while (curr) {
    SDL_Log("Segment %d x: %d, y: %d\n", count, curr->rect.x, curr->rect.y);
    count++;
    curr = curr->next;
  };
  return count;
};

int main(int argc, char **arv) {

  SDL_Init(SDL_INIT_EVERYTHING);
  window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED_DISPLAY(0),
                            SDL_WINDOWPOS_CENTERED_DISPLAY(0), SCREEN_WIDTH,
                            SCREEN_HEIGHT, 0);
  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

  // segment 1
  snake = malloc(sizeof(Segment));
  snake->rect = init_head;
  snake->next = NULL;

  while (isRunning) {
    processInput();
    update();
    render();

    SDL_Delay(1000 / 8);

    getSnakeLength();
  };

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
};
