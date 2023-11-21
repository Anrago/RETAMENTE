#include "raylib.h"

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
            currentScene = START_GAME;
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
