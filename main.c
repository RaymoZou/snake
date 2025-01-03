#include <SDL2/SDL.h>
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <stdlib.h>
#include <time.h>

SDL_Window *window;
SDL_Renderer *renderer;
TTF_Font *font;
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;
const int PLAYER_SIZE = 25;
const int MIN_OPACITY = 20;
const int NUM_ROWS = SCREEN_WIDTH / PLAYER_SIZE;
const int NUM_COLS = SCREEN_HEIGHT / PLAYER_SIZE;
const int FONT_SIZE = 72;
const SDL_Color SNAKE_COLOR = {255, 255, 255,
                               SDL_ALPHA_OPAQUE}; // color of snake body
const SDL_Color FOOD_COLOR = {255, 255, 255,
                              SDL_ALPHA_OPAQUE}; // color of the food
const SDL_Color SCORE_COLOR = {255, 255, 255,
                               SDL_ALPHA_OPAQUE}; // color of the score text
const SDL_Color CLEAR_COLOR = {
    0, 0, 0, SDL_ALPHA_OPAQUE}; // color used for SDL_RenderClear - essentially
                                // the background color
int isRunning = SDL_TRUE;
int score = 0;
char scoreText[20];
typedef enum Direction { NONE, UP, DOWN, LEFT, RIGHT } Direction;

// game state

// a segment is a linked list
typedef struct Segment {
  SDL_Rect rect;
  struct Segment *next;
} Segment;

SDL_Rect food;
Segment *snake;

Direction currDirection = NONE;

// returns true if there is any segment that is intersecting with the food
SDL_bool IsOverlapping(SDL_Rect *food, Segment *curr) {
  while (curr) {
    if (SDL_HasIntersection(food, &curr->rect)) {
      return SDL_TRUE;
    };
    curr = curr->next;
  }
  return SDL_FALSE;
};

// TODO: handle last edge case - when there
// are no more locations available
void spawnFood() {
  do {
    food.x = rand() % (NUM_ROWS)*PLAYER_SIZE;
    food.y = rand() % (NUM_COLS)*PLAYER_SIZE;
  } while (IsOverlapping(&food, snake));
};

void free_segment(Segment *segment) {
  Segment *temp;
  while (snake) {
    temp = snake;
    snake = snake->next;
    free(temp);
  };
}

void restart() {
  free_segment(snake);
  snake = malloc(sizeof(Segment));
  snake->rect.x = SCREEN_WIDTH - PLAYER_SIZE;
  snake->rect.y = SCREEN_HEIGHT - PLAYER_SIZE;
  snake->rect.w = PLAYER_SIZE;
  snake->rect.h = PLAYER_SIZE;
  snake->next = NULL;
  score = 0;
  currDirection = NONE;
};

// problem: the food and head are on top of eachother
void eatFood() {
  SDL_Log("eating food");
  Segment *new_seg = malloc(sizeof(Segment));
  SDL_Rect new_head = food;
  switch (currDirection) {
  case UP:
    new_head.y -= PLAYER_SIZE;
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
  case NONE:
    // do nothing
    break;
  default:
    break;
  };
  new_seg->next = snake->next;
  snake->next = new_seg;
  score++;
  spawnFood();
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
    } else if (event.type == SDL_QUIT) {
      SDL_Log("Quit message received\n");
      isRunning = SDL_FALSE;
    };
  };
}

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

// render the player and the food
void render() {
  SDL_SetRenderDrawColor(renderer, CLEAR_COLOR.r, CLEAR_COLOR.g, CLEAR_COLOR.b, CLEAR_COLOR.a);
  SDL_RenderClear(renderer);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  Segment *curr_seg = snake;
  int curr_opacity = SDL_ALPHA_OPAQUE;
  int decrement = SDL_ALPHA_OPAQUE / getSnakeLength();
  while (curr_seg) {
    SDL_SetRenderDrawColor(renderer, SNAKE_COLOR.r, SNAKE_COLOR.g,
                           SNAKE_COLOR.b, curr_opacity);
    SDL_RenderFillRect(renderer, &curr_seg->rect);
    curr_seg = curr_seg->next;
    curr_opacity -= decrement;
  }
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
  SDL_SetRenderDrawColor(renderer, SNAKE_COLOR.r, SNAKE_COLOR.g, SNAKE_COLOR.b,
                         SNAKE_COLOR.a);
  SDL_RenderFillRect(renderer, &food);

  // render ui
  SDL_Surface *surface =
      TTF_RenderText_Solid(font, SDL_itoa(score, scoreText, 10), SNAKE_COLOR);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_Rect rect = {0, 0, surface->w, surface->h};
  SDL_RenderCopy(renderer, texture, &rect, &rect);

  SDL_RenderPresent(renderer);
};

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

  while (curr->next) {
    curr = curr->next;
    if (SDL_HasIntersection(&curr->rect, &snake->rect)) {
      restart();
      break;
    }
    SDL_Rect temp = curr->rect;
    curr->rect = prev_rect;
    prev_rect = temp;
  };

  // check for food collision
  if (SDL_HasIntersection(&snake->rect, &food)) {
    eatFood();
  };

  // check for edge collisions
  if (snake->rect.x < 0 | snake->rect.x >= SCREEN_WIDTH | snake->rect.y < 0 |
      snake->rect.y >= SCREEN_HEIGHT) {
    restart();
  };
};

void initialize() {
  // seed randomness with time
  srand(time(NULL));
  // initialize ttf
  if (TTF_Init() != 0) {
    SDL_Log("TTF initialization failed: ", TTF_GetError());
    SDL_Quit();
  };
  font = TTF_OpenFont("GroutpixFlow-0vvyP.ttf", FONT_SIZE);
  if (!font) {
    SDL_Log("Error loading font: ", TTF_GetError());
  }

  SDL_Init(SDL_INIT_EVERYTHING);
  window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED_DISPLAY(0),
                            SDL_WINDOWPOS_CENTERED_DISPLAY(0), SCREEN_WIDTH,
                            SCREEN_HEIGHT, 0);
  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

  // initialize food
  food.x = SCREEN_WIDTH / 2;
  food.y = SCREEN_HEIGHT / 2;
  food.w = PLAYER_SIZE;
  food.h = PLAYER_SIZE;

  // initialize snake
  snake = malloc(sizeof(Segment));
  snake->rect.x = SCREEN_WIDTH - PLAYER_SIZE;
  snake->rect.y = SCREEN_HEIGHT - PLAYER_SIZE;
  snake->rect.w = PLAYER_SIZE;
  snake->rect.h = PLAYER_SIZE;
  snake->next = NULL;
};

int main(int argc, char **arv) {
  initialize();

  while (isRunning) {
    processInput();
    update();
    render();

    SDL_Delay(1000 / 8); // in milliseconds

    getSnakeLength();
  };

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_Quit();
  SDL_Quit();
  return 0;
};
