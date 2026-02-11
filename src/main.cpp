#include <raylib.h>
#include <stdio.h>
#include <deque>
#include <raymath.h>

Color GBGREEN = {251, 241, 199, 255};
Color GBDARKGREEN = {121, 116, 14, 255};
Color GBLIGHTGREEN = {152, 151, 26, 255};
Color GRUVORANGE = {175, 58, 3, 255};

const int cellSize = 30;
const int cellCount = 25;
int offset = 75;

double lastUpdateTime = 0;

bool ElementInDeque(Vector2 element, std::deque<Vector2> deque) {
  for (unsigned int i = 0; i < deque.size(); i++) {
    if (Vector2Equals(deque[i], element)) {
      return true;
    }
  }
  return false;
}

bool eventTriggered(double interval) {
  double currentTime = GetTime();
  if(currentTime - lastUpdateTime >= interval) {
    lastUpdateTime = currentTime;
    return true;
  }
  return false;
}

class Snake {
public:
  std::deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
  Vector2 direction = {1, 0};
  bool addSegment = false;

  void Draw() {
    Color localColor = GBLIGHTGREEN;
    for (int i = 0; i < body.size(); i++) {
      float x = body[i].x;
      float y = body[i].y;
      if (i != 0) {
        localColor = GBDARKGREEN;
      }
      Rectangle segment = Rectangle{offset + x * cellSize, offset + y * cellSize, (float)cellSize, (float)cellSize};
      DrawRectangleRounded(segment, 0.5, 6, localColor);
    }
  }

  void Update() {
    body.push_front(Vector2Add(body[0], direction));
    if (addSegment == true) {
      addSegment = false;
    } else {
      body.pop_back();
    }
  }

  void reset() {
    body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
    direction = {1,0};
  }
};

class Food {
public:
  Vector2 position = {5, 6};
  Texture2D texture;

  Food(std::deque<Vector2> snakeBody) {
    Image sprite = LoadImage("assets/foodSprite.png");
    texture = LoadTextureFromImage(sprite);
    UnloadImage(sprite);
    position = GenRandomPos(snakeBody);
  }

 // ~Food() {
 //   UnloadTexture(texture);
 // }

  void Draw() {
    DrawTexture(texture, offset + position.x * cellSize, offset + position.y * cellSize, WHITE); 
  }

  Vector2 GenerateRandomCell() {
    float x = GetRandomValue(0, cellCount -1);
    float y = GetRandomValue(0, cellCount -1);
    return Vector2{x, y};
  }

  Vector2 GenRandomPos(std::deque<Vector2> snakeBody) {
    Vector2 position = GenerateRandomCell();
    while (ElementInDeque(position, snakeBody)) {
      position = GenerateRandomCell();
    }
    return position;
  }

};

class Game {
public:
  Snake player = Snake();
  Food food = Food(player.body);
  bool running = true;
  bool paused = false;
  int score = 0;
  Sound eatSound;
  Sound wallSound;

  Game() {
    InitAudioDevice();
    eatSound = LoadSound("sounds/eat.mp3");
    wallSound = LoadSound("sounds/wall.mp3");

    SetSoundVolume(eatSound, (float)0.5);
    SetSoundVolume(wallSound, (float)0.5);
  }

  ~Game() {
    UnloadSound(eatSound);
    UnloadSound(wallSound);
    CloseAudioDevice();
  }


  void Draw() {
    food.Draw();
    player.Draw();

    if (paused) {
      DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){GBGREEN.r, GBGREEN.g, GBGREEN.b, 100});
    }

  }

  void Update() {
    if (running) {
      player.Update();
      CheckCollisionWithFood();
      CheckCollisionWithEdge();
      CheckCollisionWithTail();
    }
  }

  void CheckCollisionWithFood() {
    if (Vector2Equals(player.body[0], food.position)) {
      printf("Eating Food.\n");
      food.position = food.GenRandomPos(player.body);
      player.addSegment = true;
      score++;
      PlaySound(eatSound);
    }
  }

  void CheckCollisionWithEdge() {
    if (player.body[0].x == cellCount || player.body[0].x == -1) {
      GameOver();
    }
    if (player.body[0].y == cellCount || player.body[0].y == -1) {
      GameOver();
    }
  }

  void GameOver() {
    printf("Game Over!\n");
    player.reset();
    food.position = food.GenRandomPos(player.body);
    running = false;
    score = 0;
    PlaySound(wallSound);
  }

  void CheckCollisionWithTail() {
    std::deque<Vector2> headlessBody = player.body;
    headlessBody.pop_front();

    if (ElementInDeque(player.body[0], headlessBody)) {
      GameOver();
    }
  }

};

int main() {
  InitWindow(2*offset + cellSize*cellCount, 2*offset + cellSize*cellCount, "Snake Game");
  Image icon = LoadImage("assets/foodSprite.png");
  SetWindowIcon(icon);
  SetTargetFPS(120);

  Game game = Game();

  Music music = LoadMusicStream("sounds/track1.ogg");
  music.looping = true;
  PlayMusicStream(music);

  while (!WindowShouldClose()) {
    ClearBackground(GBGREEN);
    UpdateMusicStream(music);
    
    if (eventTriggered(0.2) && game.paused == false) {
      game.Update();
    }

    if (IsKeyPressed(KEY_UP) && game.player.direction.y != 1 && !game.paused) {
      game.player.direction = {0, -1};
      game.running = true;
    }
    if (IsKeyPressed(KEY_DOWN) && game.player.direction.y != -1 && !game.paused) {
      game.player.direction = {0, 1};
      game.running = true;
    }
    if (IsKeyPressed(KEY_LEFT) && game.player.direction.x != 1 && !game.paused) {
      game.player.direction = {-1, 0};
      game.running = true;
    }
    if (IsKeyPressed(KEY_RIGHT) && game.player.direction.x != -1 && !game.paused) {
      game.player.direction = {1, 0};
      game.running = true;
    }
    if (IsKeyPressed(KEY_Q)) {
      if (game.paused == true) {
        game.paused = false;
      } else {
        game.paused = true;
      }
    }

    // Drawing
    BeginDrawing();
    DrawRectangleLinesEx(Rectangle{(float)offset-5, (float)offset-5, (float)cellSize*cellCount+10, (float)cellSize*cellCount+10}, 5, GRUVORANGE);
    if (!game.paused) {
      DrawText("GRUVY SNAKE", offset - 5, 20, 40, GRUVORANGE);
    } else {
      DrawText("PAUSED", offset - 5, 20, 40, GRUVORANGE);
    }
    DrawText(TextFormat("SCORE: %i", game.score), offset - 5, offset + cellSize * cellCount + 10, 40, GRUVORANGE);
    game.Draw();
    EndDrawing();
  }
 
  UnloadImage(icon);
  UnloadMusicStream(music);
  CloseWindow();

  return 0;
}
