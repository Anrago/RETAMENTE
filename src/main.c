#include "raylib.h"

float timePlayed = 0.0f;
typedef enum MenuOption
{
    START_GAME,
    OPTIONS,
    CREDITS,
    EXIT
} MenuOption;

typedef struct
{
    Rectangle bounds;
    const char *text;
    MenuOption action;
} MenuItem;

void PlayMusic(Music music);
void StartGameUpdate();
void StartGameDraw();
void OptionsUpdate();
void OptionsDraw();
void CreditsUpdate();
void CreditsDraw();

int main(void)
{
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Simple Menu - Raylib");
    InitAudioDevice();

    // Define las opciones del menú
    MenuItem menuItems[] = {
        {{screenWidth / 2 - 100, screenHeight / 2 - 60, 200, 40}, "Iniciar", START_GAME},
        {{screenWidth / 2 - 100, screenHeight / 2 - 10, 200, 40}, "Opciones", OPTIONS},
        {{screenWidth / 2 - 100, screenHeight / 2 + 40, 200, 40}, "Créditos", CREDITS},
        {{screenWidth / 2 - 100, screenHeight / 2 + 90, 200, 40}, "Salir", EXIT}};

    Music menuMusic = LoadMusicStream("build/assets/menuMusic.mp3");
    Image bgImage = LoadImage("build/assets/bg_menu.png");
    Texture2D background = LoadTextureFromImage(bgImage);

    int EXIT_FLAG = 1;

    SetTargetFPS(60);

    while (!WindowShouldClose() && EXIT_FLAG)
    {
        PlayMusic(menuMusic);

        BeginDrawing();
        DrawTextureRec(background, (Rectangle){0, 0, screenWidth, screenHeight}, (Vector2){0, 0}, RAYWHITE);

        for (int i = 0; i < sizeof(menuItems) / sizeof(menuItems[0]); i++)
        {
            DrawRectangleRec(menuItems[i].bounds, RAYWHITE);
            DrawText(menuItems[i].text, menuItems[i].bounds.x + 20, menuItems[i].bounds.y + 10, 20, BLACK);
        }

        // Verifica clics en el menú
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
                        // Funciones del juego
                        break;
                    case OPTIONS:
                        // Funciones de opciones
                        break;
                    case CREDITS:
                        // Funciones de créditos
                        break;
                    case EXIT:
                        EXIT_FLAG = 0;
                        break;
                    default:
                        break;
                    }
                }
            }
        }

        EndDrawing();
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
    PlayMusicStream(music);
    UpdateMusicStream(music);

    timePlayed = GetMusicTimePlayed(music) / GetMusicTimeLength(music);

    if (timePlayed >= 0.99f)
        timePlayed = 1.0f;
}

void StartGameUpdate()
{
}

void StartGameDraw()
{
}

void OptionsUpdate()
{
}

void OptionsDraw()
{
}

void CreditsUpdate()
{
}

void CreditsDraw()
{
}
