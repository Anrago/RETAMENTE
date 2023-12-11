#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_QUESTIONS 100

const int resolutionsCount = 3;
const int screenWidths[] = {1280, 1600, 1920};
const int screenHeights[] = {720, 1200, 1080};
int currentResolutionIndex = 0;

const int MAX_FPS = 60;
float timePlayed = 0.0f;
int EXIT_FLAG = 1;

long filePosition = 0;
int currentQuestion = 1;
int correctAnswers = 0;
typedef struct question
{
    char question[100];
    char answer[4][100];
    char correctAnswer;
} Tquestion;

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
    float startAngle;
    float endAngle;
    Color color;
} RouletteSector;

/*
--------------------------------------------------------------------------------------------------
*/
bool CheckAngleInSector(float angle, RouletteSector sector)
{
    return angle >= sector.startAngle && angle < sector.endAngle;
}

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
void MenuUpdate(Sound mySound, size_t menuItemsCount, MenuItem menuItems[]);
void MenuDraw(Texture2D background, Texture2D tittleTexture, Image tittle, size_t menuItemsCount, MenuItem menuItems[]);
void StartGameUpdate(int screenWidth, int screenHeight);
void StartGameDraw();
void OptionsUpdate(Music menuMusic, Sound mySound, size_t menuItemsCount, MenuItem menuItems[]);
void OptionsDraw(Texture2D background, Texture2D optionMenuTexture, Image optionMenu, size_t menuItemsCount, MenuItem menuItems[]);
void CreditsUpdate();
void CreditsDraw();
void DrawRoulette(RouletteSector *sectors, int sectorCount, float rotation);
void DrawSector(Vector2 center, float radius, float startAngle, float endAngle, Color color);
void readFile(FILE *fp, Tquestion preguntas[MAX_QUESTIONS]);
void questionUpdate(char filename[]);
//================================================================================================//

int main(void)
{
    srand(time(NULL));
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
    Image bgImage = LoadImage("assets/bg_menu.png");
    Texture2D background = LoadTextureFromImage(bgImage);
    Sound menuButton = LoadSound("assets/menuButton.wav");
    Image optionMenu = LoadImage("assets/optionMenu.png");
    Texture2D optionMenuTexture = LoadTextureFromImage(optionMenu);
    //================================================================================================//

    PlayMusicStream(menuMusic);

    while (!WindowShouldClose() && EXIT_FLAG)
    {
        switch (currentScene)
        {
        case MENU:
            PlayMusic(menuMusic);
            MenuUpdate(menuButton, menuItemsCount, menuItems);
            MenuDraw(background, tittleTexture, tittle, menuItemsCount, menuItems);
            break;
        case START_GAME:
            StartGameUpdate(screenWidth, screenHeight);

            break;
        case OPTIONS:
            PlayMusic(menuMusic);
            OptionsUpdate(menuMusic, menuButton, optionItemsCount, optionItems);
            OptionsDraw(background, optionMenuTexture, optionMenu, optionItemsCount, optionItems);
            break;
        case CREDITS:
            currentScene = CREDITS;
            break;
        case EXIT:
            EXIT_FLAG = 0;
            break;
        default:
            break;
        }
    }

    UnloadTexture(background);
    UnloadImage(bgImage);
    UnloadMusicStream(menuMusic);
    UnloadSound(menuButton);

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

void MenuUpdate(Sound mySound, size_t menuItemsCount, MenuItem menuItems[])
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePoint = GetMousePosition();

        for (int i = 0; i < menuItemsCount; i++)
        {
            float textWidth = MeasureText(menuItems[i].text, 40);
            float x = (GetScreenWidth() - textWidth) / 2;

            // Ajustar la posición de la colisión para centrar horizontalmente
            Rectangle centeredBounds = {
                x,
                menuItems[i].bounds.y,
                textWidth,
                menuItems[i].bounds.height};

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

        // Centrar horizontalmente el texto utilizando la resolución de la pantalla
        float x = (GetScreenWidth() - textWidth) / 2;

        Color textColor = BLACK;

        // Ajustar la posición del mouse al sistema de coordenadas local del rectángulo
        Vector2 mousePointLocal = {GetMouseX() - x, GetMouseY() - menuItems[i].bounds.y};

        if (CheckCollisionPointRec(mousePointLocal, (Rectangle){0, 0, textWidth, 40}))
        {
            textColor = YELLOW;
        }

        // Ajustar la posición y para centrar verticalmente el texto en el área definida
        float y = menuItems[i].bounds.y + (menuItems[i].bounds.height - 40) / 2;

        DrawText(menuItems[i].text, (int)x, (int)y, 40, textColor);
    }

    EndDrawing();
}

void StartGameUpdate(int screenWidth, int screenHeight)
{
    char *n;
    int num;
    Color raund;
    int band = 0;
    Sound mySound = LoadSound("assets/Roulette Sound.mp3");
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

    // Crear una textura simple para representar la flecha
    RenderTexture2D arrowTexture = LoadRenderTexture(30, 60);
    BeginTextureMode(arrowTexture);
    ClearBackground(BLANK);

    // Dibujar la flecha en la textura
    DrawTriangle((Vector2){0, 0}, (Vector2){15, 60}, (Vector2){30, 0}, BLACK);
    EndTextureMode();

    while (!WindowShouldClose())
    {
        // Actualizar
        if (IsKeyPressed(KEY_SPACE) && !spinning)
        {
            spinning = true;
            rotationSpeed = 20.0f + GetRandomValue(-5, 5); // Velocidad de rotación más rápida y un poco de variación
            PlaySound(mySound);
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
                    questionUpdate("MATE.txt");
                }
                else if (ColorToInt(stoppedColor) != ColorToInt(BLUE))
                {
                    questionUpdate("ESPA.txt");
                }
            }
        }

        // Dibujar
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRoulette(sectors, sectorCount, rotation);

        // Dibujar flecha indicando la posición (ajustar las coordenadas según sea necesario)
        Vector2 arrowPosition = {GetScreenWidth() / 1.958f - arrowTexture.texture.width / 2.0, GetScreenHeight() / 2.0f - 199.0f};
        DrawTexturePro(arrowTexture.texture, (Rectangle){0, 0, arrowTexture.texture.width, -arrowTexture.texture.height}, (Rectangle){arrowPosition.x, arrowPosition.y, arrowTexture.texture.width, arrowTexture.texture.height}, (Vector2){arrowTexture.texture.width / 2, arrowTexture.texture.height}, 0.0f, WHITE);

        EndDrawing();
    }

    // De-inicialización
    UnloadRenderTexture(arrowTexture);
}

void StartGameDraw()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    EndDrawing();
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

        float x = menuItems[i].bounds.x + (menuItems[i].bounds.width - textWidth) / 2;

        Color textColor = BLACK;

        if (CheckCollisionPointRec(GetMousePosition(), menuItems[i].bounds))
        {
            textColor = YELLOW;
        }

        DrawText(menuItems[i].text, x, menuItems[i].bounds.y + 10, 40, textColor);
    }

    EndDrawing();
}

void CreditsUpdate()
{
}

void CreditsDraw()
{
}

/*
--------------------------------------------------------------------------------------------------
*/
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

    // Ajuste para centrar correctamente la ruleta
}

void DrawSector(Vector2 center, float radius, float startAngle, float endAngle, Color color)
{
    DrawRing(center, radius, radius - 10, startAngle, endAngle, 10, color);
}

void readFile(FILE *fp, Tquestion preguntas[MAX_QUESTIONS])
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

void questionUpdate(char filename[])
{
    FILE *fp = fopen(filename, "r");
    Sound Correct = LoadSound("assets/CORRECTO.mp3");
    Sound Incorrect = LoadSound("assets/INCORRECTO.mp3");
    static long int filePos = 0;
    char answer; // Inicializa la respuesta con un valor que no corresponde a ninguna opción válida
    fseek(fp, filePos, SEEK_SET);
    Tquestion preguntas[MAX_QUESTIONS];
    int i = 0;

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

        while (i < 5 && !WindowShouldClose())
        {
            answer = 'x';
            readFile(fp, preguntas);

            BeginDrawing();
            ClearBackground(RAYWHITE);

            // Calcular dinámicamente la posición y la anchura del texto en función del tamaño de la pantalla
            float textWidth = MeasureText(preguntas[currentQuestion].question, 20);
            float x = (GetScreenWidth() - textWidth) / 2;
            float y = GetScreenHeight() / 3.5 - 50; // Ajustar la posición vertical según sea necesario

            DrawText(preguntas[currentQuestion].question, x, y, 20, originalColor);

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
        fclose(fp);
    }
}
