#include "raylib.h"

//define game state
enum GameState {
    INTRO,
    COUNTDOWN,
    GAMEPLAY,
    GAMEOVER
};

//defines a structure for projectile properties
struct Projectile {
    Vector2 position;
    float size;
    Vector2 velocity;
    bool active;
};

//main driver
int main() {
    //initialize window
    const int screenWidth = 400;
    const int screenHeight = 650;
    InitWindow(screenWidth, screenHeight, "Laser Dodger 1.0");

    //init sound
    InitAudioDevice();
    //load sound, found in sfx subfolder in main game folder
    Sound countDownSFX = LoadSound("sfx/countdown.wav");
    Sound gameOverSFX = LoadSound("sfx/game_over.wav");
    Sound laserSFX = LoadSound("sfx/laser.wav");

    //load textures
    //textures background, uses textures subfolder in main folder
    Texture2D backgroundLayer1 = LoadTexture("textures/background_layer1.png");
    Texture2D backgroundLayer2 = LoadTexture("textures/background_layer2.png");

    //textures game objs
    Texture2D playerTexture = LoadTexture("textures/player.png");
    Texture2D enemyTexture = LoadTexture("textures/enemy.png");
    Texture2D projectileTexture = LoadTexture("textures/laserball.png");

    //define player variables and properties, static
    Vector2 playerPosition = {static_cast<float>(screenWidth) / 2, static_cast<float>(screenHeight) - 40};
    const float playerSize = 30.0f;
    float playerSpeed = 7.0f;

    //enemy variables and properties, randomized
    const float enemySize = 20.0f;
    //converting float with int
    Vector2 enemyPosition = {
        static_cast<float>(GetRandomValue(0, static_cast<int>(screenWidth - enemySize))),
        static_cast<float>(screenHeight - 600)
    };
    //min 7 to max 9 speed
    float enemySpeed = GetRandomValue(7, 9);
    //randomized initial direction
    int enemyDirection = GetRandomValue(0, 1) == 0 ? -1 : 1;
    //randomized time (sec) between shots
    float enemyShootTimer = GetRandomValue(300, 800);
    //check used for game state transition
    bool enemyCanShoot = true;

    //initialize projectiles
    const int maxProjectiles = 50;
    Projectile projectiles[maxProjectiles] = {0};

    //initialize and start into intro
    GameState gameState = INTRO;

    //countdown timer, 4 sec (60fps per sec)
    int countdownTimer = 245;

    //round timer variables
    double roundTimer = GetTime();
    int roundSeconds = roundTimer;

    //for gameplay
    //makes player, enemy, and round timer appear
    bool showPlayerAndEnemy = false;
    bool showRoundTimer = false;

    //FPS
    SetTargetFPS(60);

    //main game loop
    while (!WindowShouldClose()) {
        //uses enum game state, transitions
        switch (gameState) {
            //enters INTRO state
            case INTRO:
                //prints title and creds
                DrawText("Made by Franz", screenWidth / 2 - MeasureText("Made by Franz", 20) / 2, screenHeight - 630, 20, RAYWHITE);
                DrawText("Laser Dodger", screenWidth / 2 - MeasureText("Laser Dodger", 35) / 2, screenHeight / 2 - 10, 35, RED);
                DrawText("Press [SPACE] to Start", screenWidth / 2 - MeasureText("Press [SPACE] to Start", 20) / 2, screenHeight - 25, 20, RAYWHITE);
                
                //user presses [SPACE] on keyboard
                if (IsKeyDown(KEY_SPACE)) {
                    //transitions to countdown state
                    gameState = COUNTDOWN;
                    //play sound
                    PlaySound(countDownSFX);
                    //calls bool to make objs appear
                    showPlayerAndEnemy = true;
                }
                break;

            //enters COUNTDOWN state, limited scope
            case COUNTDOWN: {
                //timer set
                int seconds = countdownTimer / 60;
                //draws countdown timer until reaches 0
                if (seconds >= 0) {
                    DrawText(TextFormat("%d", seconds), screenWidth - 165 - MeasureText("%d", 50) / 2, screenHeight / 2 - 50, 50, RED);
                    countdownTimer--;
                    //afterwards, enter GAMEPLAY state
                    if (countdownTimer < 0) {
                        gameState = GAMEPLAY;
                        roundTimer = GetTime();
                    }
                } else {
                    roundSeconds = 0;
                }
                break;
            }

            //enters GAMEPLAY state, limited scope
            case GAMEPLAY: {
                //calls bool to set draw timer
                showRoundTimer = true;
                //update round timer, conver to int
                double currentRoundTime = GetTime() - roundTimer;
                roundSeconds = static_cast<int>(currentRoundTime);

                //draws background layer 1 (planet backdrop)
                DrawTexturePro(backgroundLayer1,
                    (Rectangle){0.0f, 0.0f, static_cast<float>(backgroundLayer1.width), static_cast<float>(backgroundLayer1.height)},
                    (Rectangle){0.0f, 0.0f, static_cast<float>(screenWidth), static_cast<float>(screenHeight)},
                    {}, 0.0f, WHITE);

                //draws background layer 2 (starts foreground)
                DrawTexturePro(backgroundLayer2,
                    (Rectangle){0.0f, 0.0f, static_cast<float>(backgroundLayer2.width), static_cast<float>(backgroundLayer2.height)},
                    (Rectangle){0.0f, 0.0f, static_cast<float>(screenWidth), static_cast<float>(screenHeight)},
                    {}, 0.0f, WHITE);

                //calls bool to draw player and enemy
                if (showPlayerAndEnemy) {
                    //draws player properties
                    DrawTexture(playerTexture, static_cast<int>(playerPosition.x - playerSize / 2), static_cast<int>(playerPosition.y - playerSize / 2), WHITE);

                    //draws enemy properties
                    DrawTexture(enemyTexture, static_cast<int>(enemyPosition.x - enemySize / 2), static_cast<int>(enemyPosition.y - enemySize / 2), WHITE);

                    //draws active projectiles
                    for (int i = 0; i < maxProjectiles; i++) {
                        if (projectiles[i].active) {
                            //updates projectile position
                            float projX = static_cast<float>(projectiles[i].position.x - projectiles[i].size / 2);
                            float projY = static_cast<float>(projectiles[i].position.y - projectiles[i].size / 2);


                            //draws the projectile with texture
                            DrawTextureEx(projectileTexture, { projX, projY }, 0.0f, projectiles[i].size / projectileTexture.width, ORANGE);

                            //checks if projectiles are out of screen, deactivate them
                            if (projectiles[i].position.y > screenHeight || projectiles[i].position.y < 0 || projectiles[i].position.x < 0 || projectiles[i].position.x > screenWidth) {
                                projectiles[i].active = false;
                            }
                        }
                    }

                    //only true in GAMEPLAY
                    if (showRoundTimer) {
                        //draws round timer at the top center
                        DrawText(TextFormat("Timer: %i", roundSeconds), screenWidth / 2 - MeasureText("Timer: %i", 20) / 2, 10, 20, WHITE);
                    }
                }

                //update logic
                //player movement, A / D | left / right
                if (IsKeyDown(KEY_A) && playerPosition.x > playerSize / 2) {
                    playerPosition.x -= playerSpeed;
                }
                if (IsKeyDown(KEY_D) && playerPosition.x < screenWidth - playerSize / 2) {
                    playerPosition.x += playerSpeed;
                }

                //updates enemy movement
                enemyPosition.x += enemySpeed * enemyDirection;
                if (enemyPosition.x <= 0) {
                    //when enemy hits the left edge, reverse direction to the right and bounce
                    enemyDirection = 1;
                    enemyPosition.x = 0;
                } else if (enemyPosition.x + enemySize >= screenWidth) {
                    //when enemy hits the right edge, reverse direction to the left and bounce
                    enemyDirection = -1;
                    enemyPosition.x = screenWidth - enemySize;
                }

                //updates enemy shooting logic
                enemyShootTimer -= GetFrameTime() * 1000;
                if (enemyShootTimer <= 0) {
                    enemyCanShoot = true;
                    //randomize time between shots
                    enemyShootTimer = GetRandomValue(300, 800); 
                }

                //when enemy shoots proj
                if (enemyCanShoot) {
                    //plays sound
                    PlaySound(laserSFX);

                    //find an inactive projectile and activate it
                    for (int i = 0; i < maxProjectiles; i++) {
                        if (!projectiles[i].active) {
                            projectiles[i].active = true;
                            //start from enemy pos
                            projectiles[i].position = enemyPosition;
                            //conversion float int, randomize velocity
                            projectiles[i].velocity = {0.0f, static_cast<float>(GetRandomValue(5, 18))};
                            //randomize size
                            projectiles[i].size = 20;
                            //enemy has shot a proj
                            enemyCanShoot = false;
                            break;
                        }
                    }
                }

                //updates active projectiles, after shooting
                for (int i = 0; i < maxProjectiles; i++) {
                    if (projectiles[i].active) {
                        projectiles[i].position.y += projectiles[i].velocity.y;
                        if (projectiles[i].position.y > screenHeight) {
                            //deactivates proj when out of screen
                            projectiles[i].active = false;
                        }
                    }
                }

                //check for player-projectile collision
                for (int i = 0; i < maxProjectiles; i++) {
                    if (projectiles[i].active) {
                        Rectangle playerRect = {playerPosition.x - playerSize / 2, playerPosition.y - playerSize / 2, playerSize, playerSize};
                        Rectangle projectileRect = {projectiles[i].position.x - projectiles[i].size / 2, projectiles[i].position.y - projectiles[i].size / 2, projectiles[i].size, projectiles[i].size};

                        //when collided
                        if (CheckCollisionRecs(playerRect, projectileRect)) {
                            //plays sound
                            PlaySound(gameOverSFX);

                            //enters GAMEOVER state
                            gameState = GAMEOVER;
                            break;
                        }
                    }
                }
                break;
            }
            
            //enters GAMEOVER state, limited scope
            case GAMEOVER: {
                //displays game over screen, time survived, and restart prompt
                DrawText("Game Over", screenWidth / 2 - MeasureText("Game Over", 50) / 2, screenHeight / 2 - 40, 50, RED);
                DrawText(TextFormat("Survived: %d seconds", roundSeconds), screenWidth / 2 - MeasureText("Survived: %d seconds", 20) / 2, screenHeight / 2 + 10, 20, WHITE);
                DrawText("Press [R] to restart", screenWidth / 2 - MeasureText("Press [R] to restart", 20) / 2, screenHeight / 2 + 65, 20, GREEN);

                //when player presses [R] keyboard
                if (IsKeyDown(KEY_R)) {
                    //reinitializes
                    //plays sound
                    PlaySound(countDownSFX);

                    //resets the game
                    gameState = COUNTDOWN;
                    countdownTimer = 245;

                    //resets the player pos
                    playerPosition.x = static_cast<float>(screenWidth) / 2;

                    //randomize the enemy properties
                    enemyPosition = {static_cast<float>(GetRandomValue(0, screenWidth - static_cast<int>(enemySize))), static_cast<float>(screenHeight - 600)};
                    enemySpeed = GetRandomValue(5, 7);
                    enemyDirection = GetRandomValue(0, 1) == 0 ? -1 : 1;
                    enemyShootTimer = GetRandomValue(500, 1000);
                    enemyCanShoot = true;

                    //reset the projectiles
                    for (int i = 0; i < maxProjectiles; i++) {
                        projectiles[i].active = false;
                    }
                }
                break;
            }
        }
        
        //drawing
        BeginDrawing();
        //background
        ClearBackground(BLACK);
        
        //afterwards, cleanup
        EndDrawing();
    }

    //unload textures
    UnloadTexture(backgroundLayer1);
    UnloadTexture(backgroundLayer2);

    UnloadTexture(playerTexture);
    UnloadTexture(enemyTexture);
    UnloadTexture(projectileTexture);

    //deinitialize
    CloseAudioDevice();
    CloseWindow();

    return 0;
}