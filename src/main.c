#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define MAX_QUESTIONS 10

const int resolutionsCount = 3;
const int screenWidths[] = {1280, 1600, 1920};
const int screenHeights[] = {720, 1200, 1080};
int currentResolutionIndex = 0;

const int MAX_FPS = 60;
float timePlayed = 0.0f;
int EXIT_FLAG = 1;
int currentQuestion = 1;
int correctAnswers = 0;
int finalScore = 0;
int remainingSeconds;

typedef struct question
{
    char question[100];
    char answer[4][100];
    char correctAnswer;
} Question;
typedef enum
{
    MENU,
    START_GAME,
    OPTIONS,
    CREDITS,
    EXIT
} GameScene;

typedef enum
{
    MUTE_MUSIC,
    MUTE_SOUNDS,
    CHANGE_RESOLUTION,
    BACK_TO_MENU
} OptionAction;

typedef enum
{
    MENU2,
    GAME,
    GAME_OVER
} GameState;

GameScene currentScene = MENU;
typedef struct
{
    Rectangle bounds;
    const char *text;
    GameScene action;
} MenuItem;

typedef struct
{
    Rectangle bounds;
    const char *text;
    OptionAction action;
} OptionItem;

typedef struct
{
    int startTime;
    int countdown;
} Timer;

typedef struct
{
    float startAngle;
    float endAngle;
    Color color;
} RouletteSector;

typedef struct
{
    int countdown;
    double timer;
    bool transition;
} CountdownState;

const char *GetColorName(Color color)
{
    if (color.r == 255 && color.g == 0 && color.b == 0)
        return "RED";
    else if (color.r == 0 && color.g == 0 && color.b == 255)
        return "BLUE";
    // Agrega más condiciones según sea necesario para otros colores
    else
        return "UNKNOWN COLOR";
}

// PROTOTYPES //
//================================================================================================//
void PlayMusic(Music music);
void ChangeResolution();
bool CheckAngleInSector(float angle, RouletteSector sector);
void MenuUpdate(Sound mySound, size_t menuItemsCount, MenuItem menuItems[]);
void MenuDraw(Texture2D background, Texture2D tittleTexture, Image tittle, size_t menuItemsCount, MenuItem menuItems[]);
void StartGameUpdate(int screenWidth, int screenHeight, Sound mySound, Texture2D background, Texture2D gameOver);
void StartGameDraw(Texture2D background);
void OptionsUpdate(Music menuMusic, Sound mySound, size_t menuItemsCount, MenuItem menuItems[]);
void OptionsDraw(Texture2D background, Texture2D optionMenuTexture, Image optionMenu, size_t menuItemsCount, MenuItem menuItems[]);
void CreditsUpdate();
void CreditsDraw();
//================================================================================================//

int main(void)
{
    int screenWidth = screenWidths[currentResolutionIndex];
    int screenHeight = screenHeights[currentResolutionIndex];
    InitWindow(screenWidth, screenHeight, "RetaMente");
    InitAudioDevice();
    SetTargetFPS(MAX_FPS);

    // MENU ITEMS //
    //================================================================================================//
    MenuItem menuItems[] = {
        {{screenWidths[currentResolutionIndex] / 2 - 100, screenHeights[currentResolutionIndex] / 2 - 60, 200, 40}, "Iniciar", START_GAME},
        {{screenWidths[currentResolutionIndex] / 2 - 100, screenHeights[currentResolutionIndex] / 2 - 10, 200, 40}, "Opciones", OPTIONS},
        {{screenWidths[currentResolutionIndex] / 2 - 100, screenHeights[currentResolutionIndex] / 2 + 40, 200, 40}, "Créditos", CREDITS},
        {{screenWidths[currentResolutionIndex] / 2 - 100, screenHeights[currentResolutionIndex] / 2 + 90, 200, 40}, "Salir", EXIT}};
    //================================================================================================//

    // OPTION ITEMS //
    //================================================================================================//
    OptionItem optionItems[] = {
        {{screenWidths[currentResolutionIndex] / 2 - 100, screenHeights[currentResolutionIndex] / 2 - 60, 200, 40}, "Silenciar Musica", MUTE_MUSIC},
        {{screenWidths[currentResolutionIndex] / 2 - 100, screenHeights[currentResolutionIndex] / 2 - 10, 200, 40}, "Silenciar efectos de sonido", MUTE_SOUNDS},
        {{screenWidths[currentResolutionIndex] / 2 - 100, screenHeights[currentResolutionIndex] / 2 + 40, 200, 40}, "Cambiar resolucion", CHANGE_RESOLUTION},
        {{screenWidths[currentResolutionIndex] / 2 - 100, screenHeights[currentResolutionIndex] / 2 + 90, 200, 40}, "Volver al menu", BACK_TO_MENU}};
    //================================================================================================//

    size_t menuItemsCount = sizeof(menuItems) / sizeof(menuItems[0]);
    size_t optionItemsCount = sizeof(optionItems) / sizeof(optionItems[0]);

    // MAIN MENU & OPTIONS MENU //
    //================================================================================================//
    Image tittle = LoadImage("assets/tittle.png");
    Texture2D tittleTexture = LoadTextureFromImage(tittle);
    Music menuMusic = LoadMusicStream("assets/menuMusic.mp3");
    Texture2D background[16];

    for (int i = 1; i < 16; i++)
    {
        Image bgImage = LoadImage(TextFormat("assets/holaBackground/hola%d.png", i));
        background[i - 1] = LoadTextureFromImage(bgImage);
    }

    Image bgGame = LoadImage("assets/bg_game.png");
    Texture2D backgroundGame = LoadTextureFromImage(bgGame);
    Sound menuButton = LoadSound("assets/menuButton.wav");
    Image optionMenu = LoadImage("assets/optionMenu.png");
    Texture2D optionMenuTexture = LoadTextureFromImage(optionMenu);
    Image areYouReadybg = LoadImage("assets/are_you_ready_bg.png");
    Texture2D areYouReadybgTexture = LoadTextureFromImage(areYouReadybg);
    Image areYouReady = LoadImage("assets/are_you_ready.png");
    Texture2D areYouReadyTexture = LoadTextureFromImage(areYouReady);
    Image number3 = LoadImage("assets/3.png");
    Texture2D number3Texture = LoadTextureFromImage(number3);
    Image number2 = LoadImage("assets/2.png");
    Texture2D number2Texture = LoadTextureFromImage(number2);
    Image number1 = LoadImage("assets/1.png");
    Texture2D number1Texture = LoadTextureFromImage(number1);
    Image gameOver = LoadImage("assets/game_over.png");
    Texture2D gameOverTexture = LoadTextureFromImage(gameOver);
    //================================================================================================//

    PlayMusicStream(menuMusic);

    int hola = 0;

    while (!WindowShouldClose() && EXIT_FLAG)
    {
        switch (currentScene)
        {
        case MENU:
            PlayMusic(menuMusic);
            MenuUpdate(menuButton, menuItemsCount, menuItems);
            MenuDraw(background[hola], tittleTexture, tittle, menuItemsCount, menuItems);
            break;

        case START_GAME:
            PlayMusic(menuMusic);
            RunCountdown(3, areYouReadyTexture, number3Texture, number2Texture, number1Texture, areYouReadybgTexture);
            StartGameUpdate(screenWidth, screenHeight, menuButton, backgroundGame, gameOverTexture);
            break;

        case OPTIONS:
            PlayMusic(menuMusic);
            OptionsUpdate(menuMusic, menuButton, optionItemsCount, optionItems);
            OptionsDraw(background[hola], optionMenuTexture, optionMenu, optionItemsCount, optionItems);
            break;

        case CREDITS:
            CreditsUpdate();
            CreditsDraw();
            break;

        case EXIT:
            EXIT_FLAG = 0;
            break;

        default:
            break;
        }
        hola++;

        if (hola == 17)
        {
            hola = 0;
        }
    }

    for (int i = 0; i < 16; i++)
    {
        UnloadTexture(background[i]);
    }
    UnloadMusicStream(menuMusic);
    UnloadSound(menuButton);
    UnloadTexture(tittleTexture);
    UnloadImage(tittle);
    UnloadTexture(backgroundGame);
    UnloadImage(bgGame);
    UnloadTexture(optionMenuTexture);
    UnloadImage(optionMenu);
    UnloadTexture(areYouReadyTexture);
    UnloadImage(areYouReady);
    UnloadTexture(number3Texture);
    UnloadImage(number3);
    UnloadTexture(number2Texture);
    UnloadImage(number2);
    UnloadTexture(number1Texture);
    UnloadImage(number1);
    UnloadTexture(gameOverTexture);
    UnloadImage(gameOver);

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
/*
   Function: PlayMusic
   Description: Plays the music in loop.
   Parameters: Music music
   Returns: nothing
*/
void PlayMusic(Music music)
{
    UpdateMusicStream(music);

    timePlayed = GetMusicTimePlayed(music) / GetMusicTimeLength(music);

    if (timePlayed >= 0.99f)
        timePlayed = 1.0f;
}

/*
    Function: ChangeResolution
    Description: Changes the resolution of the window.
    Parameters: nothing
    Returns: nothing
 */
void ChangeResolution()
{
    currentResolutionIndex = (currentResolutionIndex + 1) % resolutionsCount;
    SetWindowSize(screenWidths[currentResolutionIndex], screenHeights[currentResolutionIndex]);
}

void DrawCenteredTimer(Timer timer, int screenWidth, int screenHeight)
{
    int barHeight = 30;
    int padding = 10;

    int currentTime = GetTime();
    int elapsedSeconds = currentTime - timer.startTime;
    remainingSeconds = timer.countdown - elapsedSeconds;

    float barWidth = (float)elapsedSeconds / timer.countdown * screenWidth;
    Rectangle barComplete = {0, 0, barWidth, barHeight};

    Vector2 timerPosition = {(screenWidth - MeasureText("Tiempo restante: 00s", 20)) / 2, padding};

    DrawRectangleRec(barComplete, RED);
    DrawText(TextFormat("Tiempo restante: %02ds", remainingSeconds), timerPosition.x, timerPosition.y, 20, BLACK);
}

bool CheckAngleInSector(float angle, RouletteSector sector)
{
    return angle >= sector.startAngle && angle < sector.endAngle;
}

void DrawRoulette(RouletteSector *sectors, int sectorCount, float rotation)
{
    float radius = 200.0f;
    float centerX = GetScreenWidth() / 2.0f;
    float centerY = GetScreenHeight() / 2.0f;

    Vector2 center = {centerX, centerY};

    for (int i = 0; i < sectorCount; i++)
    {
        DrawSector(center, radius, sectors[i].startAngle + rotation, sectors[i].endAngle + rotation, sectors[i].color);
    }
}

void DrawSector(Vector2 center, float radius, float startAngle, float endAngle, Color color)
{
    DrawRing(center, radius, radius - 10, startAngle, endAngle, 10, color);
}

void readFile(FILE *fp, Question preguntas[MAX_QUESTIONS])
{
    int j = 0;
    static int ant;
    if (currentQuestion != ant)
    {
        fgets(preguntas[currentQuestion].question, 256, fp);
        while (j < 4)
        {
            fgets(preguntas[currentQuestion].answer[j], 256, fp);
            j++;
        }
        fscanf(fp, "%c", &preguntas[currentQuestion].correctAnswer);
        fgetc(fp);
        ant = currentQuestion;
    }
}

void questionUpdate(char filename[], Texture2D background)
{
    FILE *fp = fopen(filename, "r");

    char answer;
    Question preguntas[MAX_QUESTIONS];

    Color originalColor = BLACK;
    Color hoverColor = YELLOW;

    if (fp == NULL)
    {
        printf("Error al abrir el archivo");
        exit(1);
    }
    else
    {
        Rectangle answerRect[4];

        while (currentQuestion < MAX_QUESTIONS)
        {
            answer = 'x';
            readFile(fp, preguntas);

            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawTextureRec(background, (Rectangle){0, 0, screenWidths[currentResolutionIndex], screenHeights[currentResolutionIndex]}, (Vector2){0, 0}, RAYWHITE);
            DrawText(preguntas[currentQuestion].question, 190, 200, 20, originalColor);

            for (int j = 0; j < 4; j++)
            {
                float textWidth = MeasureText(preguntas[currentQuestion].answer[j], 20);
                answerRect[j] = (Rectangle){190, 250 + j * 50, textWidth, 20};

                Vector2 mousePointLocal = {GetMouseX() - 190, GetMouseY() - answerRect[j].y};

                if (CheckCollisionPointRec(mousePointLocal, (Rectangle){0, 0, textWidth, 20}))
                {
                    DrawText(preguntas[currentQuestion].answer[j], (int)answerRect[j].x, (int)answerRect[j].y, 20, hoverColor);

                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    {
                        if (j == 0)
                        {
                            answer = 'a';
                        }
                        else if (j == 1)
                        {
                            answer = 'b';
                        }
                        else if (j == 2)
                        {
                            answer = 'c';
                        }
                        else if (j == 3)
                        {
                            answer = 'd';
                        }
                        if (answer == preguntas[currentQuestion].correctAnswer)
                        {
                            correctAnswers++;
                        }
                        currentQuestion++;
                    }
                }
                else
                {
                    DrawText(preguntas[currentQuestion].answer[j], (int)answerRect[j].x, (int)answerRect[j].y, 20, originalColor);
                }
            }

            printf("%d\n", correctAnswers);

            EndDrawing();
        }
        fclose(fp);
    }
}

void RunCountdown(int seconds, Texture2D areYouReadyTexture, Texture2D number3Texture, Texture2D number2Texture, Texture2D number1Texture, Texture2D background)
{
    CountdownState countdownState = {seconds, GetTime(), false};

    while (!countdownState.transition)
    {
        if (GetTime() - countdownState.timer >= 1.0)
        {
            countdownState.countdown--;
            countdownState.timer = GetTime();

            if (countdownState.countdown <= 0)
            {
                countdownState.transition = true;
            }
        }

        BeginDrawing();

        DrawTextureRec(background, (Rectangle){0, 0, screenWidths[currentResolutionIndex], screenHeights[currentResolutionIndex]}, (Vector2){0, 0}, RAYWHITE);

        DrawTexture(areYouReadyTexture, GetScreenWidth() / 2 - areYouReadyTexture.width / 2, GetScreenHeight() / 2 - areYouReadyTexture.height - 200 / 2, WHITE);

        if (countdownState.countdown == 3)
        {
            DrawTexture(number3Texture, GetScreenWidth() / 2 - number3Texture.width / 2, GetScreenHeight() / 2 - number3Texture.height / 2, WHITE);
        }
        else if (countdownState.countdown == 2)
        {
            DrawTexture(number2Texture, GetScreenWidth() / 2 - number2Texture.width / 2, GetScreenHeight() / 2 - number2Texture.height / 2, WHITE);
        }
        else if (countdownState.countdown == 1)
        {
            DrawTexture(number1Texture, GetScreenWidth() / 2 - number1Texture.width / 2, GetScreenHeight() / 2 - number1Texture.height / 2, WHITE);
        }
        else
        {
            DrawText("¡Preparado!", GetScreenWidth() / 2 - MeasureText("¡Preparado!", 20) / 2, GetScreenHeight() / 2 + 30, 20, GREEN);
        }

        EndDrawing();
    }
}

void MenuUpdate(Sound mySound, size_t menuItemsCount, MenuItem menuItems[])
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePoint = GetMousePosition();

        for (int i = 0; i < menuItemsCount; i++)
        {
            float textWidth = MeasureText(menuItems[i].text, 40);
            float x = (GetScreenWidth() - textWidth) / 2;

            Rectangle centeredBounds = {
                x,
                menuItems[i].bounds.y,
                textWidth,
                menuItems[i].bounds.height};

            Vector2 mousePointLocal = {
                GetMouseX() - centeredBounds.x,
                GetMouseY() - centeredBounds.y};

            if (CheckCollisionPointRec(mousePoint, centeredBounds))
            {
                PlaySound(mySound);
                switch (menuItems[i].action)
                {
                case START_GAME:
                    currentScene = START_GAME;
                    break;
                case OPTIONS:
                    currentScene = OPTIONS;
                    break;
                case CREDITS:
                    currentScene = CREDITS;
                    break;
                case EXIT:
                    currentScene = EXIT;
                    break;
                }
            }
        }
    }
}

void MenuDraw(Texture2D background, Texture2D tittleTexture, Image tittle, size_t menuItemsCount, MenuItem menuItems[])
{
    BeginDrawing();
    DrawTextureRec(background, (Rectangle){0, 0, screenWidths[currentResolutionIndex], screenHeights[currentResolutionIndex]}, (Vector2){0, 0}, RAYWHITE);
    DrawTexture(tittleTexture, screenWidths[currentResolutionIndex] / 2 - tittle.width / 2, 60, WHITE);
    for (int i = 0; i < menuItemsCount; i++)
    {
        float textWidth = MeasureText(menuItems[i].text, 40);

        float x = (GetScreenWidth() - textWidth) / 2;

        Color textColor = BLACK;

        Vector2 mousePointLocal = {GetMouseX() - x, GetMouseY() - menuItems[i].bounds.y};

        if (CheckCollisionPointRec(mousePointLocal, (Rectangle){0, 0, textWidth, 40}))
        {
            textColor = YELLOW;
        }

        float y = menuItems[i].bounds.y + (menuItems[i].bounds.height - 40) / 2;

        DrawText(menuItems[i].text, (int)x, (int)y, 40, textColor);
    }

    EndDrawing();
}

void StartGameUpdate(int screenWidth, int screenHeight, Sound mySound, Texture2D background, Texture2D gameOver)
{
    Timer timer;
    timer.startTime = GetTime();
    timer.countdown = 20;

    GameState gameState = GAME;

    char *n;
    int num;
    int band = 0;
    Sound rouletteSound = LoadSound("assets/Roulette_Sound.mp3");
    bool shouldClose = false;

    int sectorCount = 8;
    RouletteSector sectors[8] = {
        {0, 45, RED},
        {45, 90, BLUE},
        {90, 135, RED},
        {135, 180, BLUE},
        {180, 225, RED},
        {225, 270, BLUE},
        {270, 315, RED},
        {315, 360, BLUE}};

    float rotationSpeed = 0.0f;
    float rotation = 0.0f;
    bool spinning = false;

    RenderTexture2D arrowTexture = LoadRenderTexture(30, 60);
    BeginTextureMode(arrowTexture);
    ClearBackground(BLANK);

    DrawTriangle((Vector2){0, 0}, (Vector2){15, 60}, (Vector2){30, 0}, BLACK);

    EndTextureMode();

    while (!shouldClose)
    {
        switch (gameState)
        {
        case GAME:
            if (IsKeyPressed(KEY_SPACE) && !spinning)
            {
                spinning = true;
                rotationSpeed = 20.0f + GetRandomValue(-5, 5);

                PlaySound(rouletteSound);
            }

            if (spinning)
            {
                rotation += rotationSpeed;
                rotationSpeed *= 0.99f;

                if (rotationSpeed < 0.1f)
                {
                    spinning = false;
                    rotationSpeed = 0.0f;
                    int sectorIndex = ((int)rotation % 360) / (360 / sectorCount);
                    Color stoppedColor = sectors[sectorIndex].color;
                    currentQuestion = 1;
                    if (ColorToInt(stoppedColor) != ColorToInt(RED))
                    {
                        questionUpdate("MATE.txt", background);
                    }
                    else if (ColorToInt(stoppedColor) != ColorToInt(BLUE))
                    {
                        questionUpdate("ESPA.txt", background);
                    }
                }
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawTextureRec(background, (Rectangle){0, 0, screenWidths[currentResolutionIndex], screenHeights[currentResolutionIndex]}, (Vector2){0, 0}, RAYWHITE);
            DrawRoulette(sectors, sectorCount, rotation);
            DrawCenteredTimer(timer, GetScreenWidth(), GetScreenHeight());

            Vector2 arrowPosition = {GetScreenWidth() / 1.958f - arrowTexture.texture.width / 2.0, GetScreenHeight() / 2.0f - 199.0f};
            DrawTexturePro(arrowTexture.texture, (Rectangle){0, 0, arrowTexture.texture.width, -arrowTexture.texture.height}, (Rectangle){arrowPosition.x, arrowPosition.y, arrowTexture.texture.width, arrowTexture.texture.height}, (Vector2){arrowTexture.texture.width / 2, arrowTexture.texture.height}, 0.0f, WHITE);

            EndDrawing();

            if (GetTime() - timer.startTime >= timer.countdown)
            {
                gameState = GAME_OVER;
            }

            break;

        case GAME_OVER:
            Rectangle buttonRect = {GetScreenWidth() / 2 - 150, GetScreenHeight() / 2 + 250, 310, 50};
            Color buttonColor = WHITE;

            BeginDrawing();
            DrawTextureRec(background, (Rectangle){0, 0, screenWidths[currentResolutionIndex], screenHeights[currentResolutionIndex]}, (Vector2){0, 0}, RAYWHITE);
            DrawTexture(gameOver, GetScreenHeight() / 2, 50, WHITE);

            if (CheckCollisionPointRec(GetMousePosition(), buttonRect))
            {
                buttonColor = YELLOW;
            }

            if (CheckCollisionPointRec(GetMousePosition(), buttonRect) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                PlaySound(mySound);

                shouldClose = true;
                currentScene = MENU;
                correctAnswers = 0;
                currentQuestion = 1;
            }

            DrawText("Volver al Menú", buttonRect.x, buttonRect.y, 40, buttonColor);
            int totalScore = correctAnswers * 100;
            DrawText(TextFormat("%i", totalScore), (GetScreenWidth() - MeasureText(TextFormat("%i", totalScore), 66) - 2) / 2, (GetScreenHeight() - 60) / 2 + 50, 66, BLACK);
            DrawText(TextFormat("%i", totalScore), (GetScreenWidth() - MeasureText(TextFormat("%i", totalScore), 60)) / 2, (GetScreenHeight() - 60) / 2 + 50, 60, RED);

            EndDrawing();

            break;
        }
    }
}

void OptionsUpdate(Music menuMusic, Sound mySound, size_t menuItemsCount, MenuItem menuItems[])
{
    static bool muteMusic = false;
    static bool muteSounds = false;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePoint = GetMousePosition();

        for (int i = 0; i < menuItemsCount; i++)
        {
            float textWidth = MeasureText(menuItems[i].text, 40);
            float x = (GetScreenWidth() - textWidth) / 2;

            Rectangle centeredBounds = {
                x,
                menuItems[i].bounds.y,
                textWidth,
                menuItems[i].bounds.height};

            Vector2 mousePointLocal = {
                GetMouseX() - centeredBounds.x,
                GetMouseY() - centeredBounds.y};

            if (CheckCollisionPointRec(mousePoint, menuItems[i].bounds))
            {
                PlaySound(mySound);
                switch (menuItems[i].action)
                {
                case MUTE_MUSIC:
                    muteMusic = !muteMusic;
                    if (muteMusic)
                    {
                        SetMusicVolume(menuMusic, 0.0f);
                    }
                    else
                    {
                        SetMusicVolume(menuMusic, 1.0f);
                    }
                    break;

                case MUTE_SOUNDS:
                    muteSounds = !muteSounds;
                    if (muteSounds)
                    {
                        SetSoundVolume(mySound, 0.0f);
                    }
                    else
                    {
                        SetSoundVolume(mySound, 1.0f);
                    }
                    break;

                case CHANGE_RESOLUTION:
                    ChangeResolution();
                    break;

                case BACK_TO_MENU:
                    currentScene = MENU;
                    break;
                }
            }
        }
    }
}

void OptionsDraw(Texture2D background, Texture2D optionMenuTexture, Image optionMenu, size_t menuItemsCount, MenuItem menuItems[])
{
    BeginDrawing();
    DrawTextureRec(background, (Rectangle){0, 0, screenWidths[currentResolutionIndex], screenHeights[currentResolutionIndex]}, (Vector2){0, 0}, RAYWHITE);
    DrawTexture(optionMenuTexture, screenWidths[currentResolutionIndex] / 2 - optionMenu.width / 2, 60, WHITE);
    for (int i = 0; i < menuItemsCount; i++)
    {
        float textWidth = MeasureText(menuItems[i].text, 40);

        float x = (GetScreenWidth() - textWidth) / 2;

        Color textColor = BLACK;

        Vector2 mousePointLocal = {GetMouseX() - x, GetMouseY() - menuItems[i].bounds.y};

        if (CheckCollisionPointRec(mousePointLocal, (Rectangle){0, 0, textWidth, 40}))
        {
            textColor = YELLOW;
        }

        float y = menuItems[i].bounds.y + (menuItems[i].bounds.height - 40) / 2;

        DrawText(menuItems[i].text, (int)x, (int)y, 40, textColor);
    }

    EndDrawing();
}

void CreditsUpdate()
{
}

void CreditsDraw()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    EndDrawing();
}
