#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define MAX_QUESTIONS 50

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

GameState gameState;

GameScene currentScene = MENU;

typedef struct
{
    Rectangle bounds;
    Texture2D image;
    GameScene action;
} MenuItem;

typedef struct
{
    Rectangle bounds;
    Texture2D image;
    GameScene action;
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

bool isMouseOverStartGame = false;
bool isMouseOverOptions = false;
bool isMouseOverCredits = false;
bool isMouseOverExit = false;

Music GameMusic;
Font myFont;
Timer timer;

const char *GetColorName(Color color)
{
    if (color.r == 255 && color.g == 0 && color.b == 0)
        return "RED";
    else if (color.r == 0 && color.g == 0 && color.b == 255)
        return "BLUE";
    else
        return "UNKNOWN COLOR";
}

// PROTOTYPES //
//================================================================================================//
void PlayMusic(Music music);
void ChangeResolution();
bool CheckAngleInSector(float angle, RouletteSector sector);
void MenuUpdate(Sound mySound, Texture2D startGameTexture, Texture2D optionsTexture, Texture2D creditsTexture, Texture2D exitTexture);
void MenuDraw(Music gameMusic, Texture2D background, Texture2D tittleTexture, Image tittle, Texture2D startGameTexture, Texture2D optionsTexture, Texture2D creditsTexture, Texture2D exitTexture);
void StartGameUpdate(Music gameMusic, int screenWidth, int screenHeight, Sound mySound, Texture2D background, Texture2D gameOver, Texture2D backToMenu, Texture2D score, Texture2D highscore);
void StartGameDraw(Texture2D background);
void OptionsUpdate(Music menuMusic, Music gameMusic, Sound mySound, Texture2D startGameTexture, Texture2D optionsTexture, Texture2D creditsTexture, Texture2D exitTexture);
void OptionsDraw(Texture2D background, Texture2D optionMenuTexture, Texture2D startGameTexture, Texture2D optionsTexture, Texture2D creditsTexture, Texture2D exitTexture);
void CreditsUpdate();
void CreditsDraw();
//================================================================================================//

int main(void)
{
    int screenWidth = screenWidths[currentResolutionIndex];
    int screenHeight = screenHeights[currentResolutionIndex];
    InitWindow(screenWidth, screenHeight, "RetaMente");
    InitAudioDevice();
    SetWindowIcon(LoadImage("assets/icon.png"));
    SetTargetFPS(MAX_FPS);

    // MENU ITEMS //
    //================================================================================================//
    Image mainMenu1 = LoadImage("assets/mainMenu/start_game.png");
    Texture2D mainMenu1Texture = LoadTextureFromImage(mainMenu1);
    Image mainMenu2 = LoadImage("assets/mainMenu/options.png");
    Texture2D mainMenu2Texture = LoadTextureFromImage(mainMenu2);
    Image mainMenu3 = LoadImage("assets/mainMenu/credits.png");
    Texture2D mainMenu3Texture = LoadTextureFromImage(mainMenu3);
    Image mainMenu4 = LoadImage("assets/mainMenu/exit.png");
    Texture2D mainMenu4Texture = LoadTextureFromImage(mainMenu4);
    //================================================================================================//

    // OPTION ITEMS //
    //================================================================================================//
    Image optionMenu1 = LoadImage("assets/optionsMenu/music.png");
    Texture2D optionMenu1Texture = LoadTextureFromImage(optionMenu1);
    Image optionMenu2 = LoadImage("assets/optionsMenu/music_effects.png");
    Texture2D optionMenu2Texture = LoadTextureFromImage(optionMenu2);
    Image optionMenu3 = LoadImage("assets/optionsMenu/change_resolution.png");
    Texture2D optionMenuu3Texture = LoadTextureFromImage(optionMenu3);
    Image optionMenu4 = LoadImage("assets/optionsMenu/go_to_menu.png");
    Texture2D optionMenu4Texture = LoadTextureFromImage(optionMenu4);
    //================================================================================================//

    // MAIN MENU & OPTIONS MENU //
    //================================================================================================//
    myFont = LoadFont("assets/font.ttf");
    Image tittle = LoadImage("assets/tittle.png");
    Texture2D tittleTexture = LoadTextureFromImage(tittle);
    Music menuMusic = LoadMusicStream("assets/menuMusic.mp3");
    Music gameMusic = LoadMusicStream("assets/gameMusic.mp3");
    Texture2D background[16];

    for (int i = 1; i < 16; i++)
    {
        Image bgImage = LoadImage(TextFormat("assets/holaBackground/hola%d.png", i));
        background[i - 1] = LoadTextureFromImage(bgImage);
    }
    Image bgGame = LoadImage("assets/bg_game.png");
    Texture2D backgroundGame = LoadTextureFromImage(bgGame);
    Sound menuButton = LoadSound("assets/menuButton.wav");
    Sound countdownGame = LoadSound("assets/countdown.mp3");
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
    Image score = LoadImage("assets/score.png");
    Texture2D scoreTexture = LoadTextureFromImage(score);
    Image highscore = LoadImage("assets/highscore.png");
    Texture2D highscoreTexture = LoadTextureFromImage(highscore);
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
            MenuUpdate(menuButton, mainMenu1Texture, mainMenu2Texture, mainMenu3Texture, mainMenu4Texture);
            MenuDraw(gameMusic, background[hola], tittleTexture, tittle, mainMenu1Texture, mainMenu2Texture, mainMenu3Texture, mainMenu4Texture);
            break;

        case START_GAME:
            RunCountdown(countdownGame, 3, areYouReadyTexture, number3Texture, number2Texture, number1Texture, areYouReadybgTexture);
            PlayMusic(gameMusic);
            StartGameUpdate(gameMusic, screenWidth, screenHeight, menuButton, backgroundGame, gameOverTexture, optionMenu4Texture, scoreTexture, highscoreTexture);
            break;

        case OPTIONS:
            PlayMusic(menuMusic);
            OptionsUpdate(menuMusic, gameMusic, menuButton, optionMenu1Texture, optionMenu2Texture, optionMenuu3Texture, optionMenu4Texture);
            OptionsDraw(background[hola], optionMenuTexture, optionMenu1Texture, optionMenu2Texture, optionMenuu3Texture, optionMenu4Texture);
            break;

        case CREDITS:
            PlayMusic(menuMusic);
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
    UnloadTexture(areYouReadybgTexture);
    UnloadImage(areYouReadybg);
    UnloadFont(myFont);

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

    DrawTextEx(myFont, TextFormat("Tiempo restante: %02d", remainingSeconds), (Vector2){timerPosition.x, timerPosition.y}, 20, 2, WHITE);
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
    static int ant = -1;

    if (currentQuestion != ant && currentQuestion < MAX_QUESTIONS)
    {
        fseek(fp, 0, SEEK_SET); // Coloca el cursor al principio del archivo

        // Avanza hasta la posición de la pregunta actual
        for (int i = 0; i <= currentQuestion; ++i)
        {
            fgets(preguntas[i].question, 256, fp);
            for (int j = 0; j < 4; j++)
            {
                fgets(preguntas[i].answer[j], 256, fp);
            }
            fscanf(fp, "%c", &preguntas[i].correctAnswer);
            fgetc(fp);
        }

        ant = currentQuestion;
    }
}

void questionUpdate(char filename[], Texture2D background)
{
    FILE *fp = fopen(filename, "r");
    Sound Correct = LoadSound("assets/CORRECTO.mp3");
    Sound Incorrect = LoadSound("assets/INCORRECTO.mp3");
    static long int filePos = 0;
    char answer; // Inicializa la respuesta con un valor que no corresponde a ninguna opción válida
    fseek(fp, filePos, SEEK_SET);
    Question preguntas[MAX_QUESTIONS];
    int i = 0;
    timer.startTime = GetTime();
    timer.countdown = 2;
    Color originalColor = BLACK;
    Color hoverColor = YELLOW;

    if (fp == NULL)
    {
        printf("Error al abrir el archivo");
        exit(1);
    }
    else
    {
        Rectangle answerRect[4]; // Rectángulos que representan las áreas de las respuestas

        while (i < 5 && (GetTime() - timer.startTime < timer.countdown))
        {
            UpdateMusicStream(GameMusic);
            answer = 'x';
            readFile(fp, preguntas);

            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawTextureRec(background, (Rectangle){0, 0, screenWidths[currentResolutionIndex], screenHeights[currentResolutionIndex]}, (Vector2){0, 0}, RAYWHITE);
            DrawCenteredTimer(timer, GetScreenWidth(), GetScreenHeight());
            // Calcular dinámicamente la posición y la anchura del texto en función del tamaño de la pantalla
            float textWidth = MeasureText(preguntas[currentQuestion].question, 20);
            float x = (GetScreenWidth() - textWidth) / 2;
            float y = GetScreenHeight() / 3.5 - 50; // Ajustar la posición vertical según sea necesario

            DrawTextEx(myFont, preguntas[currentQuestion].question, (Vector2){x, y}, 20, 1, originalColor);

            // Dibujar las respuestas y verificar el color según la posición del ratón
            for (int j = 0; j < 4; j++)
            {
                textWidth = MeasureText(preguntas[currentQuestion].answer[j], 20);
                answerRect[j] = (Rectangle){(GetScreenWidth() - textWidth) / 2, y + 50 + j * 50, textWidth, 20};

                Vector2 mousePointLocal = {GetMouseX() - answerRect[j].x, GetMouseY() - answerRect[j].y};

                if (CheckCollisionPointRec(mousePointLocal, (Rectangle){0, 0, textWidth, 20}))
                {
                    DrawText(preguntas[currentQuestion].answer[j], (int)answerRect[j].x, (int)answerRect[j].y, 20, hoverColor);

                    // Si se hizo clic en la respuesta, actualiza la variable 'answer'
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
                            PlaySound(Correct);
                            correctAnswers++;
                        }
                        else
                        {
                            PlaySound(Incorrect);
                        }
                        filePos = ftell(fp);
                        i++; // Mueve i antes del incremento de currentQuestion
                        currentQuestion++;
                    }
                }
                else
                {
                    DrawText(preguntas[currentQuestion].answer[j], (int)answerRect[j].x, (int)answerRect[j].y, 20, originalColor);
                }
            }
            EndDrawing();
        }
        if (GetTime() - timer.startTime >= timer.countdown)
        {
            gameState = GAME_OVER;
        }
        fclose(fp);
    }
}

void RunCountdown(Sound countdown, int seconds, Texture2D areYouReadyTexture, Texture2D number3Texture, Texture2D number2Texture, Texture2D number1Texture, Texture2D background)
{
    PlaySound(countdown);
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

void MenuUpdate(Sound mySound, Texture2D startGameTexture, Texture2D optionsTexture, Texture2D creditsTexture, Texture2D exitTexture)
{
    Vector2 mousePoint = GetMousePosition();

    // Verifica si el mouse está sobre las texturas de las opciones
    float textureWidth = (float)startGameTexture.width;
    float textureHeight = (float)startGameTexture.height;

    float x = (GetScreenWidth() - textureWidth) / 2;
    float y = screenHeights[currentResolutionIndex] / 2 - 60;
    Rectangle startGameBounds = {x, y, textureWidth, textureHeight};
    isMouseOverStartGame = CheckCollisionPointRec(mousePoint, startGameBounds);

    textureWidth = (float)optionsTexture.width;
    textureHeight = (float)optionsTexture.height;
    y += 80; // Ajusta la posición vertical para la siguiente opción
    Rectangle optionsBounds = {x, y, textureWidth, textureHeight};
    isMouseOverOptions = CheckCollisionPointRec(mousePoint, optionsBounds);

    textureWidth = (float)creditsTexture.width;
    textureHeight = (float)creditsTexture.height;
    y += 80; // Ajusta la posición vertical para la siguiente opción
    Rectangle creditsBounds = {x, y, textureWidth, textureHeight};
    isMouseOverCredits = CheckCollisionPointRec(mousePoint, creditsBounds);

    textureWidth = (float)exitTexture.width;
    textureHeight = (float)exitTexture.height;
    y += 80; // Ajusta la posición vertical para la siguiente opción
    Rectangle exitBounds = {x, y, textureWidth, textureHeight};
    isMouseOverExit = CheckCollisionPointRec(mousePoint, exitBounds);

    // Maneja los eventos de clic según la opción sobre la que esté el mouse
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        PlaySound(mySound);

        if (isMouseOverStartGame)
        {
            currentScene = START_GAME;
        }
        else if (isMouseOverOptions)
        {
            currentScene = OPTIONS;
        }
        else if (isMouseOverCredits)
        {
            currentScene = CREDITS;
        }
        else if (isMouseOverExit)
        {
            currentScene = EXIT;
        }
    }
}

void MenuDraw(Music gameMusic, Texture2D background, Texture2D tittleTexture, Image tittle, Texture2D startGameTexture, Texture2D optionsTexture, Texture2D creditsTexture, Texture2D exitTexture)
{
    BeginDrawing();
    DrawTextureRec(background, (Rectangle){0, 0, screenWidths[currentResolutionIndex], screenHeights[currentResolutionIndex]}, (Vector2){0, 0}, RAYWHITE);
    DrawTexture(tittleTexture, screenWidths[currentResolutionIndex] / 2 - tittle.width / 2, 60, WHITE);

    float x = (GetScreenWidth() - startGameTexture.width) / 2;
    float y = screenHeights[currentResolutionIndex] / 2 - 60;

    if (isMouseOverStartGame)
    {
        float scale = 1.0 + 0.1f * sin(2.0f * GetTime());
        DrawTextureEx(startGameTexture, (Vector2){x, y}, 0.0f, scale, YELLOW);
    }
    else
    {
        DrawTexture(startGameTexture, x, y, WHITE);
    }

    y += 80;
    if (isMouseOverOptions)
    {
        float scale = 1.0 + 0.1f * sin(2.0f * GetTime());
        DrawTextureEx(optionsTexture, (Vector2){x, y}, 0.0f, scale, YELLOW);
    }
    else
    {
        DrawTexture(optionsTexture, x, y, WHITE);
    }

    y += 80;
    if (isMouseOverCredits)
    {
        float scale = 1.0 + 0.1f * sin(2.0f * GetTime());
        DrawTextureEx(creditsTexture, (Vector2){x, y}, 0.0f, scale, YELLOW);
    }
    else
    {
        DrawTexture(creditsTexture, x, y, WHITE);
    }

    y += 80;
    if (isMouseOverExit)
    {
        float scale = 1.0 + 0.1f * sin(2.0f * GetTime());
        DrawTextureEx(exitTexture, (Vector2){x, y}, 0.0f, scale, YELLOW);
    }
    else
    {
        DrawTexture(exitTexture, x, y, WHITE);
    }

    EndDrawing();
}

void StartGameUpdate(Music gameMusic, int screenWidth, int screenHeight, Sound mySound, Texture2D background, Texture2D gameOver, Texture2D backToMenu, Texture2D score, Texture2D highscore)
{
    gameState = GAME;

    char *n;
    int num;
    int band = 1;
    int band2 = 1;
    Sound rouletteSound = LoadSound("assets/Roulette_Sound.mp3");
    Sound Victory = LoadSound("assets/VICTORIAA.mp3");
    bool shouldClose = false;
    GameMusic = LoadMusicStream("MUSICGAME.mp3");
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
    PlayMusicStream(GameMusic);
    while (!shouldClose)
    {
        UpdateMusicStream(GameMusic);
        switch (gameState)
        {
        case GAME:
            if (IsKeyPressed(KEY_SPACE) && !spinning)
            {
                //! No se reproduce la música
                spinning = true;
                rotationSpeed = 20.0f + GetRandomValue(-5, 5);
                PlayMusicStream(gameMusic);
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
            Image pressSpace = LoadImage("assets/space.png");
            Texture2D pressSpaceTexture = LoadTextureFromImage(pressSpace);
            DrawTextureRec(pressSpaceTexture, (Rectangle){0, 0, pressSpace.width, pressSpace.height}, (Vector2){GetScreenWidth() / 2 - pressSpace.width / 2, GetScreenHeight() / 2 + 300}, WHITE);

            Vector2 arrowPosition = {GetScreenWidth() / 1.958f - arrowTexture.texture.width / 2.0, GetScreenHeight() / 2.0f - 199.0f};
            DrawTexturePro(arrowTexture.texture, (Rectangle){0, 0, arrowTexture.texture.width, -arrowTexture.texture.height}, (Rectangle){arrowPosition.x, arrowPosition.y, arrowTexture.texture.width, arrowTexture.texture.height}, (Vector2){arrowTexture.texture.width / 2, arrowTexture.texture.height}, 0.0f, WHITE);

            EndDrawing();

            break;

        case GAME_OVER:
        {

            if (band == 1)
            {
                StopMusicStream(GameMusic);
                band = 0;
            }
            if (band2 == 1)
            {
                PlaySound(Victory);
                band2 = 0;
            }

            Rectangle buttonRect = {GetScreenWidth() / 2 - 150, GetScreenHeight() / 2 + 200, 310, 50};
            Color buttonColor = WHITE;

            Vector2 mousePoint = GetMousePosition();
            float textureWidth = (float)backToMenu.width;
            float textureHeight = (float)backToMenu.height;
            float x = (GetScreenWidth() - textureWidth) / 2;
            float y = screenHeights[currentResolutionIndex] / 2 + 159;
            textureWidth = (float)backToMenu.width;
            textureHeight = (float)backToMenu.height;
            Rectangle exitBounds = {x, y, textureWidth, textureHeight};
            bool isMouseOverExit = CheckCollisionPointRec(mousePoint, exitBounds);

            BeginDrawing();
            DrawTextureRec(background, (Rectangle){0, 0, screenWidths[currentResolutionIndex], screenHeights[currentResolutionIndex]}, (Vector2){0, 0}, RAYWHITE);
            DrawTexture(gameOver, GetScreenWidth() / 2 - gameOver.width / 2, 50, WHITE);

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

            y += 0;

            if (isMouseOverExit)
            {
                float scale = 1.0 + 0.1f * sin(2.0f * GetTime());
                DrawTextureEx(backToMenu, (Vector2){x, y}, 0.0f, scale, YELLOW);
            }
            else
            {
                DrawTexture(backToMenu, x, y, WHITE);
            }
            
            int totalScore = correctAnswers * 100;
            int HighScore=0;
            FILE *Score = fopen("HighScore.dll", "rb");
            fread(&HighScore, sizeof(int), 1, Score);
            fclose(Score);
            float scoreTextWidth = MeasureText(TextFormat("%i", totalScore), 65);
            float scoreX = (GetScreenWidth() - scoreTextWidth) / 2;
            float scoreY = (GetScreenHeight() - 60) / 2 - 70;
            float scoreY2 = (GetScreenHeight() + 300) / 2 - 70;

            DrawTexture(score, GetScreenWidth() / 2 - score.width / 2, GetScreenHeight() / 2 - 150, WHITE);
            DrawTextEx(myFont, TextFormat("%i", totalScore), (Vector2){(GetScreenWidth() - MeasureText(TextFormat("%i", totalScore), 60)) / 2, scoreY}, 60, 1, RED);

            if (totalScore < HighScore)
            {
                DrawTexture(highscore, GetScreenWidth() / 2 - highscore.width / 2, GetScreenHeight() / 2 + score.height - 20, WHITE);
                DrawTextEx(myFont, TextFormat("%i", HighScore), (Vector2){(GetScreenWidth() - MeasureText(TextFormat("%i", HighScore), 60)) / 2, scoreY2}, 60, 1, RED);
            }
            else
            {
                DrawTexture(highscore, GetScreenWidth() / 2 - highscore.width / 2, GetScreenHeight() / 2 + score.height - 20, WHITE);
                DrawTextEx(myFont, TextFormat("%i", totalScore), (Vector2){(GetScreenWidth() - MeasureText(TextFormat("%i", totalScore), 60)) / 2, scoreY2}, 60, 1, RED);
                Score = fopen("HighScore.dll", "wb");
                fwrite(&totalScore, sizeof(int), 1, Score);
                fclose(Score);
            }

            EndDrawing();

            break;
        }
        }
    }
    StopMusicStream(GameMusic);
    UnloadMusicStream(GameMusic);
}

void OptionsUpdate(Music menuMusic, Music gameMusic, Sound mySound, Texture2D startGameTexture, Texture2D optionsTexture, Texture2D creditsTexture, Texture2D exitTexture)
{
    static bool muteMusic = false;
    static bool muteSounds = false;

    Vector2 mousePoint = GetMousePosition();

    float textureWidth = (float)startGameTexture.width;
    float textureHeight = (float)startGameTexture.height;

    float x = (GetScreenWidth() - textureWidth) / 2;
    float y = screenHeights[currentResolutionIndex] / 2 - 60;
    Rectangle startGameBounds = {x, y, textureWidth, textureHeight};
    isMouseOverStartGame = CheckCollisionPointRec(mousePoint, startGameBounds);

    textureWidth = (float)optionsTexture.width;
    textureHeight = (float)optionsTexture.height;
    y += 80;
    Rectangle optionsBounds = {x, y, textureWidth, textureHeight};
    isMouseOverOptions = CheckCollisionPointRec(mousePoint, optionsBounds);

    textureWidth = (float)creditsTexture.width;
    textureHeight = (float)creditsTexture.height;
    y += 80;
    Rectangle creditsBounds = {x, y, textureWidth, textureHeight};
    isMouseOverCredits = CheckCollisionPointRec(mousePoint, creditsBounds);

    textureWidth = (float)exitTexture.width;
    textureHeight = (float)exitTexture.height;
    y += 80;
    Rectangle exitBounds = {x, y, textureWidth, textureHeight};
    isMouseOverExit = CheckCollisionPointRec(mousePoint, exitBounds);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        PlaySound(mySound);
        if (isMouseOverStartGame)
        {
            muteMusic = !muteMusic;
            if (muteMusic)
            {
                SetMusicVolume(GameMusic, 0.0f);
                SetMusicVolume(menuMusic, 0.0f);
            }
            else
            {
                SetMusicVolume(GameMusic, 1.0f);
                SetMusicVolume(menuMusic, 1.0f);
            }
        }
        else if (isMouseOverOptions)
        {
            muteSounds = !muteSounds;
            if (muteSounds)
            {
                // Assuming SetSoundVolume takes a Sound instead of a Sound ID
                SetSoundVolume(mySound, 0.0f);
            }
            else
            {
                SetSoundVolume(mySound, 1.0f);
            }
        }
        else if (isMouseOverCredits)
        {
            ChangeResolution();
        }
        else if (isMouseOverExit)
        {
            currentScene = MENU;
        }
    }
}

void OptionsDraw(Texture2D background, Texture2D optionMenuTexture, Texture2D startGameTexture, Texture2D optionsTexture, Texture2D creditsTexture, Texture2D exitTexture)
{
    BeginDrawing();
    DrawTextureRec(background, (Rectangle){0, 0, screenWidths[currentResolutionIndex], screenHeights[currentResolutionIndex]}, (Vector2){0, 0}, RAYWHITE);
    DrawTexture(optionMenuTexture, screenWidths[currentResolutionIndex] / 2 - optionMenuTexture.width / 2, 60, WHITE);

    float x = (GetScreenWidth() - startGameTexture.width) / 2;
    float y = screenHeights[currentResolutionIndex] / 2 - 60;

    bool isMouseOverStartGame = CheckCollisionPointRec(GetMousePosition(), (Rectangle){x, y, (float)startGameTexture.width, (float)startGameTexture.height});

    if (isMouseOverStartGame)
    {
        float scale = 1.0 + 0.1f * sin(2.0f * GetTime());
        DrawTextureEx(startGameTexture, (Vector2){x, y}, 0.0f, scale, YELLOW);
    }
    else
    {
        DrawTexture(startGameTexture, x, y, WHITE);
    }

    y += 80;

    bool isMouseOverOptions = CheckCollisionPointRec(GetMousePosition(), (Rectangle){x, y, (float)optionsTexture.width, (float)optionsTexture.height});
    if (isMouseOverOptions)
    {
        float scale = 1.0 + 0.1f * sin(2.0f * GetTime());
        DrawTextureEx(optionsTexture, (Vector2){x, y}, 0.0f, scale, YELLOW);
    }
    else
    {
        DrawTexture(optionsTexture, x, y, WHITE);
    }

    y += 80;

    bool isMouseOverCredits = CheckCollisionPointRec(GetMousePosition(), (Rectangle){x, y, (float)creditsTexture.width, (float)creditsTexture.height});
    if (isMouseOverCredits)
    {
        float scale = 1.0 + 0.1f * sin(2.0f * GetTime());
        DrawTextureEx(creditsTexture, (Vector2){x, y}, 0.0f, scale, YELLOW);
    }
    else
    {
        DrawTexture(creditsTexture, x, y, WHITE);
    }

    y += 80;

    bool isMouseOverExit = CheckCollisionPointRec(GetMousePosition(), (Rectangle){x, y, (float)exitTexture.width, (float)exitTexture.height});
    if (isMouseOverExit)
    {
        float scale = 1.0 + 0.1f * sin(2.0f * GetTime());
        DrawTextureEx(exitTexture, (Vector2){x, y}, 0.0f, scale, YELLOW);
    }
    else
    {
        DrawTexture(exitTexture, x, y, WHITE);
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

/*--------------------------------------------------------------------------------
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⣀⠙⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⣿⣷⣄⠙⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⢹⣿⣿⣷⣄⠈⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡄⢸⣿⣿⣿⣿⣷⣦⡈⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠿⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⢸⣿⣿⣿⣿⣿⣿⣿⣦⡈⠻⣿⠀⠈⠻⡿⠛⢿⣿⣿⡿⠿⠛⠉⢉⣀⡤⠀⣼⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣦⡀⠀⠓⣄⠀⢠⠀⠉⣁⣤⣴⣶⣿⣿⡟⠀⣼⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⡿⠇⢀⠉⠻⠿⣿⣿⣿⣿⣇⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡏⠀⠘⠛⠋⠉⢹⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⡀⠠⣤⣴⣿⣶⣤⣀⠙⠻⣿⣿⠀⣿⣿⣿⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠁⠐⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠻
⡿⠂⢈⣿⣿⣿⣿⣿⣷⣦⣄⠉⠀⠛⠉⣠⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣶⣶⣶⣶⣶⣶⣶⣶⣿⣿⣿⣿⣿⣿⣿⣿⡿⠗⢀⣴
⣧⣄⡈⠙⢿⣿⣿⣿⣿⣿⣿⡟⠀⣠⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠋⢀⣴⣿⣿
⣿⣿⣿⣆⠈⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠋⣠⣶⣿⣿⣿⣿
⣿⣿⣿⣿⣆⠘⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠋⡀⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠁⣠⣾⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣆⠈⠟⢀⣿⣿⣿⣿⣿⣿⡟⠀⣼⣧⠀⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠋⢁⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⠇⢠⣿⣿⣿⣿⢿⣿⣿⠁⢸⣿⣿⡆⠘⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠟⠋⣁⣿⣿⣿⣿⣿⠀⠻⠋⣠⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⡿⠀⣾⣿⣿⣿⣿⠀⢻⣿⡆⠘⣿⣿⡇⠀⢹⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠛⢉⣠⣴⠀⣿⣿⣿⣿⣿⣿⡇⠀⠾⠿⠿⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⡇⢰⣿⣿⣿⣿⣿⠀⡀⠻⣿⣦⡈⠛⠻⠷⠀⢿⣿⣿⣿⣿⣿⣿⡿⠛⠁⣠⣴⣿⣿⡇⢀⣿⣿⣿⣿⣿⣿⡇⢰⣶⣶⣦⣤⣤⣤⣤⣤⣈⠉⠉⢻⣿⣿
⣿⣿⣿⣿⣿⠁⢸⣿⣿⣿⣿⣿⠀⣷⣄⠙⠻⣿⣷⣶⣶⣶⣾⣿⣿⣿⣿⠟⠁⣠⣦⣰⣿⣿⣿⠟⢀⣾⣿⣿⣿⣿⣿⣿⣇⣸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣦⡄⠉⢻⣿
⣿⣿⣿⣿⣿⠀⢸⣿⣿⣿⣿⣿⠀⢻⡟⢀⣤⡈⠙⠻⢿⣿⣿⣿⣿⣿⣿⣶⣦⣀⡉⠙⠋⠉⣁⣴⣿⡿⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠿⠇⢠⣴⣾⣿
⣿⣿⣿⣿⣿⠀⢸⣿⣿⣿⣿⣿⣧⠀⠃⣸⣿⣿⣷⠂⣠⣈⡉⠛⠛⠿⠿⢿⣿⣿⣿⣿⣿⠿⠿⠛⠁⣠⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠛⠋⣁⣤⣤⣄⣼⣿⣿⣿
⣿⣿⣿⣿⣿⠇⠸⣿⣿⣿⣿⣿⣿⣷⣄⠙⠿⣿⡏⢀⣿⣿⣿⣿⣶⣦⠀⣠⣤⣤⣤⡄⢀⣤⡔⠀⣼⣿⣿⣿⣿⣿⡟⠀⠿⠟⠋⢉⣀⣤⣶⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⠃⢀⠀⢻⣿⣿⣿⣿⣿⣿⣿⣷⣄⡈⠁⠸⣿⣿⣿⣿⣿⡟⠀⣿⣿⣿⣿⠃⠸⠋⣠⣾⣿⣿⣿⣿⣿⣿⠃⣠⣴⣶⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⡏⢀⣿⣆⠈⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⣤⣀⡉⠙⠛⠻⠃⠸⠿⠟⠛⠋⣀⣤⣾⣿⣿⣿⣿⣿⣿⣿⠃⢠⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⡇⢸⣿⣿⣷⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⣶⣶⣶⣶⣶⣶⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠃⢠⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣇⠈⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡉⠙⠻⠁⣰⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣄⠈⢿⣿⣿⣿⣧⠈⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⡀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣦⣄⠈⠛⢿⡿⠷⠄⠈⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣷⡀⠘⠑⠀⠀⠀⣦⣀⠉⠛⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⣦⣤⣾⣿⣿⣿⣿⣶⣤⠀⠈⠉⠉⠉⠉⢉⡉⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⢠⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣄⠈⠻⠃⢠⣿⣿⣄⠘⣿⡿⣵⣝⣫⡎⢿⢿⠟⢀⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⣶⣾⣿⣿⣿⡄⠸⣿⣿⣿⣿⣿⡿⠁⣠⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣄⠈⠛⠛⠋⢁⣤⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
----------------------------------------------------------------------------------*/
