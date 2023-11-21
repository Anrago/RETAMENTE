#include "raylib.h"
#include <stdio.h>

const int resolutionsCount = 3;
const int screenWidths[] = {1280, 1600, 1920};
const int screenHeights[] = {720, 1200, 1080};
int currentResolutionIndex = 0;

const int MAX_FPS = 60;
float timePlayed = 0.0f;
int EXIT_FLAG = 1;

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

// PROTOTYPES //
//================================================================================================//
void PlayMusic(Music music);
void ChangeResolution();
void MenuUpdate(Sound mySound, size_t menuItemsCount, MenuItem menuItems[]);
void MenuDraw(Texture2D background, Texture2D tittleTexture, Image tittle, size_t menuItemsCount, MenuItem menuItems[]);
void StartGameUpdate();
void StartGameDraw();
void OptionsUpdate(Music menuMusic, Sound mySound, size_t optionItemsCount, OptionItem optionItems[]);
void OptionsDraw(Texture2D background, Texture2D optionMenuTexture, Image optionMenu, size_t optionItemsCount, OptionItem optionItems[]);
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
        {{screenWidths[currentResolutionIndex] / 2 - 100, screenHeights[currentResolutionIndex] / 2 - 60, 200, 40}, "Silenciar Musica"},
        {{screenWidths[currentResolutionIndex] / 2 - 100, screenHeights[currentResolutionIndex] / 2 - 10, 200, 40}, "Silenciar efectos de sonido"},
        {{screenWidths[currentResolutionIndex] / 2 - 100, screenHeights[currentResolutionIndex] / 2 + 40, 200, 40}, "Cambiar resolucion"},
        {{screenWidths[currentResolutionIndex] / 2 - 100, screenHeights[currentResolutionIndex] / 2 + 90, 200, 40}, "Volver al menu"}};
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
            StartGameUpdate();
            StartGameDraw();
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

void MenuDraw(Texture2D background, Texture2D tittleTexture, Image tittle, size_t menuItemsCount, MenuItem menuItems[])
{
    BeginDrawing();
    DrawTextureRec(background, (Rectangle){0, 0, screenWidths[currentResolutionIndex], screenHeights[currentResolutionIndex]}, (Vector2){0, 0}, RAYWHITE);
    DrawTexture(tittleTexture, screenWidths[currentResolutionIndex] / 2 - tittle.width / 2, 60, WHITE);
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

void StartGameUpdate()
{
}

void StartGameDraw()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    EndDrawing();
}

void OptionsUpdate(Music menuMusic, Sound mySound, size_t optionItemsCount, OptionItem optionItems[])
{
    static bool muteMusic = false;
    static bool muteSounds = false;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePoint = GetMousePosition();

        for (int i = 0; i < optionItemsCount; i++)
        {
            if (CheckCollisionPointRec(mousePoint, optionItems[i].bounds))
            {
                PlaySound(mySound);
                switch (optionItems[i].action)
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
                    // ChangeResolution();
                    break;

                case BACK_TO_MENU:
                    currentScene = MENU;
                    break;
                }
            }
        }
    }
}

void OptionsDraw(Texture2D background, Texture2D optionMenuTexture, Image optionMenu, size_t optionItemsCount, OptionItem optionItems[])
{
    BeginDrawing();
    DrawTextureRec(background, (Rectangle){0, 0, screenWidths[currentResolutionIndex], screenHeights[currentResolutionIndex]}, (Vector2){0, 0}, RAYWHITE);
    DrawTexture(optionMenuTexture, screenWidths[currentResolutionIndex] / 2 - optionMenu.width / 2, 60, WHITE);
    for (int i = 0; i < optionItemsCount; i++)
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
