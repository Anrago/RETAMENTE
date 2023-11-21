#include "raylib.h"
#include <math.h>

const int screenWidth = 1280;
const int screenHeight = 720;
float timePlayed = 0.0f;

typedef enum
{
    MENU,
    START_GAME,
    OPTIONS,
    CREDITS,
    EXIT
} GameScene;

GameScene currentScene = MENU;
typedef struct
{
    Rectangle bounds;
    const char *text;
    GameScene action;
} MenuItem;

MenuItem menuItems[] = {
    {{screenWidth / 2 - 100, screenHeight / 2 - 60, 200, 40}, "Iniciar", START_GAME},
    {{screenWidth / 2 - 100, screenHeight / 2 - 10, 200, 40}, "Opciones", OPTIONS},
    {{screenWidth / 2 - 100, screenHeight / 2 + 40, 200, 40}, "Créditos", CREDITS},
    {{screenWidth / 2 - 100, screenHeight / 2 + 90, 200, 40}, "Salir", EXIT}};

void PlayMusic(Music music);
void StartGameUpdate();
void StartGameDraw();
void OptionsUpdate();
void OptionsDraw();
void CreditsUpdate();
void CreditsDraw();




typedef struct
{
    float startAngle;
    float endAngle;
    Color color;
} RouletteSector;

// Dibujar ruleta
void DrawRoulette(RouletteSector *sectors, int sectorCount, float rotation)
{
    float radius = 200.0f;
    Vector2 center = {screenWidth / 2.0f, screenHeight / 2.0f};

    for (int i = 0; i < sectorCount; i++)
    {
        DrawSector(center, radius, sectors[i].startAngle + rotation, sectors[i].endAngle + rotation, sectors[i].color);
    }

    DrawCircle(center.x, center.y, radius - 10, RAYWHITE);
}

// Dibujar cada sector de la ruleta
void DrawSector(Vector2 center, float radius, float startAngle, float endAngle, Color color)
{
    DrawRing(center, radius, radius - 10, startAngle, endAngle, 10, color);
}









int main(void)
{
    
    InitWindow(screenWidth, screenHeight, "retamente");
    InitAudioDevice();

    Music menuMusic = LoadMusicStream("assets/menuMusic.mp3");
    Image bgImage = LoadImage("assets/bg_menu.png");
    Texture2D background = LoadTextureFromImage(bgImage);

    PlayMusicStream(menuMusic);

    int EXIT_FLAG = 1;

    PlayMusic(menuMusic);
    SetTargetFPS(60);

    while (!WindowShouldClose() && EXIT_FLAG)
    {

        switch (currentScene)
        {
        case MENU:
            PlayMusic(menuMusic);
            MenuUpdate();
            MenuDraw(background);
            break;
        case START_GAME:
            StartGameUpdate();
            break;
        case OPTIONS:
            OptionsUpdate();
            OptionsDraw();
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
    CloseAudioDevice();
    CloseWindow();

    return 0;
}

void PlayMusic(Music music)
{
    UpdateMusicStream(music);

    timePlayed = GetMusicTimePlayed(music) / GetMusicTimeLength(music);

    if (timePlayed >= 0.99f)
        timePlayed = 1.0f;
}

void MenuUpdate()
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePoint = GetMousePosition();

        for (int i = 0; i < sizeof(menuItems) / sizeof(menuItems[0]); i++)
        {
            if (CheckCollisionPointRec(mousePoint, menuItems[i].bounds))
            {
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
                default:
                    break;
                }
            }
        }
    }
}

void MenuDraw(Texture2D background)
{
    BeginDrawing();
    DrawTextureRec(background, (Rectangle){0, 0, screenWidth, screenHeight}, (Vector2){0, 0}, RAYWHITE);

    for (int i = 0; i < sizeof(menuItems) / sizeof(menuItems[0]); i++)
    {
        DrawRectangleRec(menuItems[i].bounds, RAYWHITE);
        DrawText(menuItems[i].text, menuItems[i].bounds.x + 20, menuItems[i].bounds.y + 10, 20, BLACK);
    }
    EndDrawing();
}

void StartGameUpdate()
{
     Texture2D arrowTexture = LoadTexture("assets/Flecha.png");

    // Definir los sectores de la ruleta
    int sectorCount = 2;
    RouletteSector sectors[2] = {
        {0, 180, RED},
        {180, 360, BLUE}
        // {90, 135, GREEN},
        // {135, 180, YELLOW},
        // {180, 225, ORANGE},
        // {225, 270, PINK},
        // {270, 315, SKYBLUE},
        // {315, 360, PURPLE},
    };

    float rotationSpeed = 0.0f;
    float rotation = 0.0f;
    bool spinning = false;

    while (!WindowShouldClose())
    {
        // Actualizar
        if (IsKeyPressed(KEY_SPACE))
        {
            spinning = true;
            rotationSpeed = 20.0f + GetRandomValue(-5, 5); // Velocidad de rotación más rápida y un poco de variación
        }

        if (spinning)
        {
            rotation += rotationSpeed;
            rotationSpeed *= 0.99f;

            if (rotationSpeed < 0.1f)
            {
                spinning = false;
                rotationSpeed = 0.0f;
            }
        }

        // Dibujar
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRoulette(sectors, sectorCount, rotation);

        // Dibujar flecha indicando la posición
        Vector2 arrowPosition = {screenWidth / 2.0f - arrowTexture.width / 2, screenHeight / 2.0f - 230.0f};
        DrawTextureEx(arrowTexture, arrowPosition, 0.0f, 1.0f, WHITE);

        // Obtener el color de la ruleta en la posición de la flecha
        Color arrowColor = GetPixelColor((int)arrowPosition.x + arrowTexture.width / 2, (int)arrowPosition.y + arrowTexture.height / 2);

        // Comprobar si el color es ROJO (puedes cambiar esto según tu diseño de ruleta)
        if (arrowColor.r == 255 && arrowColor.g == 0 && arrowColor.b == 0)
        {
            DrawText("Arrow is on RED!", 10, 10, 20, RED);
        }

        EndDrawing();
    }

    // De-inicialización
    UnloadTexture(arrowTexture);
}

void StartGameDraw()
{
}

void OptionsUpdate()
{
}

void OptionsDraw()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    EndDrawing();
}

void CreditsUpdate()
{
}

void CreditsDraw()
{
}
