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

typedef enum
{
    NOTHING,
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

MenuItem menuItems[] = {
    {{screenWidth / 2 - 100, screenHeight / 2 - 60, 200, 40}, "Iniciar", START_GAME},
    {{screenWidth / 2 - 100, screenHeight / 2 - 10, 200, 40}, "Opciones", OPTIONS},
    {{screenWidth / 2 - 100, screenHeight / 2 + 40, 200, 40}, "Créditos", CREDITS},
    {{screenWidth / 2 - 100, screenHeight / 2 + 90, 200, 40}, "Salir", EXIT}};

OptionItem optionItems[] = {
    {{screenWidth / 2 - 100, screenHeight / 2 - 60, 200, 40}, "Silenciar Musica"},
    {{screenWidth / 2 - 100, screenHeight / 2 - 10, 200, 40}, "Silenciar efectos de sonido"},
    {{screenWidth / 2 - 100, screenHeight / 2 + 40, 200, 40}, "Cambiar resolucion"},
    {{screenWidth / 2 - 100, screenHeight / 2 + 90, 200, 40}, "Volver al menu"}};

void PlayMusic(Music music);
void MenuUpdate(Sound mySound);
void MenuDraw(Texture2D background, Texture2D tittleTexture, Image tittle);
void StartGameUpdate();
void StartGameDraw();
void OptionsUpdate(Music menuMusic, Sound mySound);
void OptionsDraw(Texture2D background, Texture2D optionMenuTexture, Image optionMenu);
void CreditsUpdate();
void CreditsDraw();

int main(void)
{
    InitWindow(screenWidth, screenHeight, "retamente");
    InitAudioDevice();
    SetTargetFPS(60);

    Music menuMusic = LoadMusicStream("assets/menuMusic.mp3");
    Image bgImage = LoadImage("assets/bg_menu.png");
    Texture2D background = LoadTextureFromImage(bgImage);
    Sound menuButton = LoadSound("assets/menuButton.wav");
    Image tittle = LoadImage("assets/tittle.png");
    Texture2D tittleTexture = LoadTextureFromImage(tittle);
    Image optionMenu = LoadImage("assets/optionMenu.png");
    Texture2D optionMenuTexture = LoadTextureFromImage(optionMenu);

    PlayMusicStream(menuMusic);

    int EXIT_FLAG = 1;

    while (!WindowShouldClose() && EXIT_FLAG)
    {
        switch (currentScene)
        {
        case MENU:
            PlayMusic(menuMusic);
            MenuUpdate(menuButton);
            MenuDraw(background, tittleTexture, tittle);
            break;
        case START_GAME:
            StartGameUpdate();
            StartGameDraw();
            break;
        case OPTIONS:
            PlayMusic(menuMusic);
            OptionsUpdate(menuMusic, menuButton);
            OptionsDraw(background, optionMenuTexture, optionMenu);
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

void PlayMusic(Music music)
{
    UpdateMusicStream(music);

    timePlayed = GetMusicTimePlayed(music) / GetMusicTimeLength(music);

    if (timePlayed >= 0.99f)
        timePlayed = 1.0f;
}

void MenuUpdate(Sound mySound)
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePoint = GetMousePosition();

        for (int i = 0; i < sizeof(menuItems) / sizeof(menuItems[0]); i++)
        {
            if (CheckCollisionPointRec(mousePoint, menuItems[i].bounds))
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

void MenuDraw(Texture2D background, Texture2D tittleTexture, Image tittle)
{
    BeginDrawing();
    DrawTextureRec(background, (Rectangle){0, 0, screenWidth, screenHeight}, (Vector2){0, 0}, RAYWHITE);
    DrawTexture(tittleTexture, screenWidth / 2 - tittle.width / 2, 60, WHITE);
    for (int i = 0; i < sizeof(menuItems) / sizeof(menuItems[0]); i++)
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

void StartGameUpdate()
{
}

void StartGameDraw()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    EndDrawing();
}

void OptionsUpdate(Music menuMusic, Sound mySound)
{
    static bool muteMusic = false;
    static bool muteSounds = false;
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePoint = GetMousePosition();

        for (int i = 0; i < sizeof(menuItems) / sizeof(menuItems[0]); i++)
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

                    break;

                case BACK_TO_MENU:
                    currentScene = MENU;
                    break;
                }
            }
        }
    }
}

void OptionsDraw(Texture2D background, Texture2D optionMenuTexture, Image optionMenu)
{
    BeginDrawing();
    DrawTextureRec(background, (Rectangle){0, 0, screenWidth, screenHeight}, (Vector2){0, 0}, RAYWHITE);
    DrawTexture(optionMenuTexture, screenWidth / 2 - optionMenu.width / 2, 60, WHITE);
    for (int i = 0; i < sizeof(optionItems) / sizeof(optionItems[0]); i++)
    {
        float textWidth = MeasureText(optionItems[i].text, 40);

        float x = optionItems[i].bounds.x + (optionItems[i].bounds.width - textWidth) / 2;

        Color textColor = BLACK;

        if (CheckCollisionPointRec(GetMousePosition(), optionItems[i].bounds))
        {
            textColor = YELLOW;
        }

        DrawText(optionItems[i].text, x, optionItems[i].bounds.y + 10, 40, textColor);
    }

    EndDrawing();
}

void CreditsUpdate()
{
}

void CreditsDraw()
{
}
