/**
 * @file arkanoid.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2023-06-07
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "../factory_test.h"

#define screen_width 240
#define screen_heigh 135

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------
#define PLAYER_MAX_LIFE 5
#define LINES_OF_BRICKS 5
#define BRICKS_PER_LINE 20

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// Vector2, 2 components
typedef struct Vector2
{
    float x; // Vector x component
    float y; // Vector y component
} Vector2;

typedef struct Player
{
    Vector2 position;
    Vector2 size;
    int life;
} Player;

typedef struct Ball
{
    Vector2 position;
    Vector2 speed;
    int radius;
    bool active;
} Ball;

typedef struct Brick
{
    Vector2 position;
    bool active;
} Brick;

#define PLAY_HIT_SOUND() _tone(random(200, 600), 30)

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
// static const int screenWidth = 800;
// static const int screenHeight = 450;
static const int screenWidth = screen_width;
static const int screenHeight = screen_heigh;

static bool gameOver = false;
static bool __pause = false;

static Player player = {0};
static Ball ball = {0};
static Brick brick[LINES_OF_BRICKS][BRICKS_PER_LINE] = {0};
static Vector2 brickSize = {0};

// //------------------------------------------------------------------------------------
// // Module Functions Declaration (local)
// //------------------------------------------------------------------------------------
// static void InitGame(void);         // Initialize game
// static void UpdateGame(void);       // Update game (one frame)
// static void DrawGame(void);         // Draw game (one frame)
// static void UnloadGame(void);       // Unload game
// static void UpdateDrawFrame(void);  // Update and Draw (one frame)

void FactoryTest::_arkanoid_start()
{
    _arkanoid_setup();
    while (1)
    {
        _arkanoid_loop();
        _check_encoder(false);
        if (ball.active)
        {
            if (_check_next())
            {
                break;
            }
        }
    }
}

void FactoryTest::_arkanoid_setup()
{
    _InitGame();

    _enc_pos = 0;
    _enc.setPosition(_enc_pos);
}

static uint32_t time_count = 0;
void FactoryTest::_arkanoid_loop()
{
    if ((millis() - time_count) > 15)
    {
        _UpdateDrawFrame();
        time_count = millis();
    }
}

//------------------------------------------------------------------------------------
// Module Functions Definitions (local)
//------------------------------------------------------------------------------------

#define GetScreenWidth() _canvas->width()

// Initialize game variables
void FactoryTest::_InitGame(void)
{
    brickSize = (Vector2){(float)GetScreenWidth() / BRICKS_PER_LINE, 10};

    // Initialize player
    // player.position = (Vector2){ screenWidth/2, screenHeight };
    player.size = (Vector2){screenWidth / 10 + 5, 5};
    player.position = (Vector2){screenWidth / 2, screenHeight - player.size.y - 6};
    player.life = PLAYER_MAX_LIFE;

    // Initialize ball
    ball.position = (Vector2){screenWidth / 2, screenHeight - player.size.y};
    ball.speed = (Vector2){0, 0};
    ball.radius = 3;
    ball.active = false;

    // Initialize bricks
    int initialDownPosition = brickSize.y / 2;

    for (int i = 0; i < LINES_OF_BRICKS; i++)
    {
        for (int j = 0; j < BRICKS_PER_LINE; j++)
        {
            brick[i][j].position = (Vector2){j * brickSize.x + brickSize.x / 2, i * brickSize.y + initialDownPosition};
            brick[i][j].active = true;
        }
    }
}

// Rectangle, 4 components
typedef struct Rectangle
{
    float x;      // Rectangle top-left corner position x
    float y;      // Rectangle top-left corner position y
    float width;  // Rectangle width
    float height; // Rectangle height
} Rectangle;

// Check collision between circle and rectangle
// NOTE: Reviewed version to take into account corner limit case
bool CheckCollisionCircleRec(Vector2 center, float radius, Rectangle rec)
{
    bool collision = false;

    int recCenterX = (int)(rec.x + rec.width / 2.0f);
    int recCenterY = (int)(rec.y + rec.height / 2.0f);

    float dx = fabsf(center.x - (float)recCenterX);
    float dy = fabsf(center.y - (float)recCenterY);

    if (dx > (rec.width / 2.0f + radius))
    {
        return false;
    }
    if (dy > (rec.height / 2.0f + radius))
    {
        return false;
    }

    if (dx <= (rec.width / 2.0f))
    {
        return true;
    }
    if (dy <= (rec.height / 2.0f))
    {
        return true;
    }

    float cornerDistanceSq =
        (dx - rec.width / 2.0f) * (dx - rec.width / 2.0f) + (dy - rec.height / 2.0f) * (dy - rec.height / 2.0f);

    collision = (cornerDistanceSq <= (radius * radius));

    return collision;
}

// #define IsKeyDown(KEY_LEFT)     (lgfx::gpio_in(PIN_LEFT) == 0)
// #define IsKeyDown(KEY_RIGHT)    (lgfx::gpio_in(PIN_RIGHT) == 0)
// #define IsKeyPressed(KEY_SPACE) (lgfx::gpio_in(PIN_UP) == 0)
static long __old_pos = 0;

// Update game (one frame)
void FactoryTest::_UpdateGame(void)
{
    if (!gameOver)
    {
        // if (IsKeyPressed('P')) pause = !pause;

        if (!__pause)
        {
            // Player movement logic
            // if (IsKeyDown(KEY_LEFT)) player.position.x -= 5;
            // if ((player.position.x - player.size.x/2) <= 0) player.position.x = player.size.x/2;
            // if (IsKeyDown(KEY_RIGHT)) player.position.x += 5;
            // if ((player.position.x + player.size.x/2) >= screenWidth) player.position.x = screenWidth - player.size.x/2;

            // if (lgfx::gpio_in(PIN_LEFT) == 0) {
            //     player.position.x -= 4;
            // }
            // if ((player.position.x - player.size.x/2) <= 0) player.position.x = player.size.x/2;

            // if (lgfx::gpio_in(PIN_RIGHT) == 0) {
            //     player.position.x += 4;
            // }
            // if ((player.position.x + player.size.x/2) >= screenWidth) player.position.x = screenWidth - player.size.x/2;

            // if (_check_encoder()) {

            //     if (_enc.getDirection() == RotaryEncoder::Direction::CLOCKWISE) {
            //         player.position.x -= 4;
            //     }
            //     if ((player.position.x - player.size.x/2) <= 0) player.position.x = player.size.x/2;

            //     else if (_enc.getDirection() == RotaryEncoder::Direction::COUNTERCLOCKWISE) {
            //         player.position.x += 4;
            //     }
            //     if ((player.position.x + player.size.x/2) >= screenWidth) player.position.x = screenWidth - player.size.x/2;

            // }

            // player.position.x += _enc_pos;
            // if ((player.position.x - player.size.x/2) <= 0) player.position.x = player.size.x/2;
            // if ((player.position.x + player.size.x/2) >= screenWidth) player.position.x = screenWidth - player.size.x/2;

            if (_enc_pos < __old_pos)
            {
                player.position.x -= 6;
                if ((player.position.x - player.size.x / 2) <= 0)
                    player.position.x = player.size.x / 2;

                __old_pos = _enc_pos;
            }
            else if (_enc_pos > __old_pos)
            {
                player.position.x += 6;
                if ((player.position.x + player.size.x / 2) >= screenWidth)
                    player.position.x = screenWidth - player.size.x / 2;

                __old_pos = _enc_pos;
            }

            // Ball launching logic
            if (!ball.active)
            {
                // if (IsKeyPressed(KEY_SPACE))
                // {
                //     ball.active = true;
                //     ball.speed = (Vector2){ 0, -5 };
                // }

                // if (lgfx::gpio_in(PIN_UP) == 0)
                // {
                //     ball.active = true;
                //     ball.speed = (Vector2){ 0, -2 };
                // }

                if (_check_next())
                {
                    ball.active = true;
                    ball.speed = (Vector2){0, -2};

                    // PLAY_HIT_SOUND();
                }
            }

            // Ball movement logic
            if (ball.active)
            {
                ball.position.x += ball.speed.x;
                ball.position.y += ball.speed.y;
            }
            else
            {
                // ball.position = (Vector2){ player.position.x, screenHeight*7/8 - 30 };
                ball.position = (Vector2){player.position.x, screenHeight - player.size.y - ball.radius * 4};
            }

            // Collision logic: ball vs walls
            if (((ball.position.x + ball.radius) >= screenWidth) || ((ball.position.x - ball.radius) <= 0))
            {
                ball.speed.x *= -1;
                PLAY_HIT_SOUND();
            }
            if ((ball.position.y - ball.radius) <= 0)
            {
                ball.speed.y *= -1;
                PLAY_HIT_SOUND();
            }
            if ((ball.position.y + ball.radius) >= screenHeight)
            {
                ball.speed = (Vector2){0, 0};
                ball.active = false;

                player.life--;
            }

            // Collision logic: ball vs player
            if (CheckCollisionCircleRec(ball.position,
                                        ball.radius,
                                        (Rectangle){player.position.x - player.size.x / 2,
                                                    player.position.y - player.size.y / 2,
                                                    player.size.x,
                                                    player.size.y}))
            {
                if (ball.speed.y > 0)
                {
                    ball.speed.y *= -1;
                    ball.speed.x = (ball.position.x - player.position.x) / (player.size.x / 2) * 2;
                }

                PLAY_HIT_SOUND();
            }

            // Collision logic: ball vs bricks
            for (int i = 0; i < LINES_OF_BRICKS; i++)
            {
                for (int j = 0; j < BRICKS_PER_LINE; j++)
                {
                    if (brick[i][j].active)
                    {
                        // Hit below
                        if (((ball.position.y - ball.radius) <= (brick[i][j].position.y + brickSize.y / 2)) &&
                            ((ball.position.y - ball.radius) > (brick[i][j].position.y + brickSize.y / 2 + ball.speed.y)) &&
                            ((fabs(ball.position.x - brick[i][j].position.x)) < (brickSize.x / 2 + ball.radius * 2 / 3)) &&
                            (ball.speed.y < 0))
                        {
                            brick[i][j].active = false;
                            ball.speed.y *= -1;

                            PLAY_HIT_SOUND();
                        }
                        // Hit above
                        else if (((ball.position.y + ball.radius) >= (brick[i][j].position.y - brickSize.y / 2)) &&
                                 ((ball.position.y + ball.radius) <
                                  (brick[i][j].position.y - brickSize.y / 2 + ball.speed.y)) &&
                                 ((fabs(ball.position.x - brick[i][j].position.x)) < (brickSize.x / 2 + ball.radius * 2 / 3)) &&
                                 (ball.speed.y > 0))
                        {
                            brick[i][j].active = false;
                            ball.speed.y *= -1;

                            PLAY_HIT_SOUND();
                        }
                        // Hit left
                        else if (((ball.position.x + ball.radius) >= (brick[i][j].position.x - brickSize.x / 2)) &&
                                 ((ball.position.x + ball.radius) <
                                  (brick[i][j].position.x - brickSize.x / 2 + ball.speed.x)) &&
                                 ((fabs(ball.position.y - brick[i][j].position.y)) < (brickSize.y / 2 + ball.radius * 2 / 3)) &&
                                 (ball.speed.x > 0))
                        {
                            brick[i][j].active = false;
                            ball.speed.x *= -1;

                            PLAY_HIT_SOUND();
                        }
                        // Hit right
                        else if (((ball.position.x - ball.radius) <= (brick[i][j].position.x + brickSize.x / 2)) &&
                                 ((ball.position.x - ball.radius) >
                                  (brick[i][j].position.x + brickSize.x / 2 + ball.speed.x)) &&
                                 ((fabs(ball.position.y - brick[i][j].position.y)) < (brickSize.y / 2 + ball.radius * 2 / 3)) &&
                                 (ball.speed.x < 0))
                        {
                            brick[i][j].active = false;
                            ball.speed.x *= -1;

                            PLAY_HIT_SOUND();
                        }
                    }
                }
            }

            // Game over logic
            if (player.life <= 0)
                gameOver = true;
            else
            {
                gameOver = true;

                for (int i = 0; i < LINES_OF_BRICKS; i++)
                {
                    for (int j = 0; j < BRICKS_PER_LINE; j++)
                    {
                        if (brick[i][j].active)
                            gameOver = false;
                    }
                }
            }
        }
    }
    else
    {
        // if (IsKeyPressed(KEY_ENTER))
        // {
        _InitGame();
        gameOver = false;
        // }
    }
}

// Draw game (one frame)
void FactoryTest::_DrawGame(void)
{
    // BeginDrawing();

    // ClearBackground(RAYWHITE);
    _canvas->fillScreen((uint32_t)0xF5C396);

    if (!gameOver)
    {
        // Draw player bar
        // DrawRectangle(player.position.x - player.size.x/2, player.position.y - player.size.y/2, player.size.x, player.size.y,
        // BLACK);
        _canvas->fillRect(player.position.x - player.size.x / 2,
                          player.position.y - player.size.y / 2,
                          player.size.x,
                          player.size.y,
                          (uint32_t)0x754316);

        // Draw player lives
        // for (int i = 0; i < player.life; i++) DrawRectangle(20 + 40*i, screenHeight - 30, 35, 10, LIGHTGRAY);
        for (int i = 0; i < player.life; i++)
            _canvas->fillSmoothCircle(
                10 + (ball.radius * 3) * i, screenHeight - ball.radius * 2, ball.radius, (uint32_t)0x754316);

        // Draw ball
        // DrawCircleV(ball.position, ball.radius, MAROON);
        _canvas->fillSmoothCircle(ball.position.x, ball.position.y, ball.radius, (uint32_t)0x754316);

        // Draw bricks
        for (int i = 0; i < LINES_OF_BRICKS; i++)
        {
            for (int j = 0; j < BRICKS_PER_LINE; j++)
            {
                if (brick[i][j].active)
                {
                    // if ((i + j) % 2 == 0) DrawRectangle(brick[i][j].position.x - brickSize.x/2, brick[i][j].position.y -
                    // brickSize.y/2, brickSize.x, brickSize.y, GRAY); else DrawRectangle(brick[i][j].position.x -
                    // brickSize.x/2, brick[i][j].position.y - brickSize.y/2, brickSize.x, brickSize.y, DARKGRAY);

                    if ((i + j) % 2 == 0)
                        _canvas->fillRect(brick[i][j].position.x - brickSize.x / 2,
                                          brick[i][j].position.y - brickSize.y / 2,
                                          brickSize.x,
                                          brickSize.y,
                                          (uint32_t)0x754316);
                    else
                        _canvas->fillRect(brick[i][j].position.x - brickSize.x / 2,
                                          brick[i][j].position.y - brickSize.y / 2,
                                          brickSize.x,
                                          brickSize.y,
                                          (uint32_t)0x754316);
                }
            }
        }

        // if (pause) DrawText("GAME PAUSED", screenWidth/2 - MeasureText("GAME PAUSED", 40)/2, screenHeight/2 - 40, 40, GRAY);
    }
    // else DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2,
    // GetScreenHeight()/2 - 50, 20, GRAY);

    // EndDrawing();
    _canvas->pushSprite(0, 0);
}

// Unload game variables
void FactoryTest::_UnloadGame(void)
{
    // TODO: Unload all dynamic loaded data (textures, sounds, models...)
}

// Update and Draw (one frame)
void FactoryTest::_UpdateDrawFrame(void)
{
    _UpdateGame();
    _DrawGame();
}
