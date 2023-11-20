#include "raylib.h"

void playMusic(Music music);

float timePlayed = 0.0f;

int main(void)
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Menu con Background - Raylib");
    InitAudioDevice();

    enum MenuOption
    {
        START_GAME,
        OPTIONS,
        EXIT
    };

    int selectedOption = START_GAME;

    Music menuMusic = LoadMusicStream("assets/menuMusic.mp3");
    Image bg_image = LoadImage("assets/bg_menu.png");
    Texture2D background = LoadTextureFromImage(bg_image);

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        playMusic(menuMusic);

        if (IsKeyPressed(KEY_DOWN))
        {
            selectedOption = (selectedOption + 1) % 3;
        }
        else if (IsKeyPressed(KEY_UP))
        {
            selectedOption = (selectedOption - 1 + 3) % 3;
        }

        if (IsKeyPressed(KEY_ENTER))
        {
            switch (selectedOption)
            {
            case START_GAME:

                break;

            case OPTIONS:

                break;

            case EXIT:
                CloseWindow();
                break;
            }
        }

        BeginDrawing();

        DrawTextureRec(background, (Rectangle){0, 0, screenWidth, screenHeight}, (Vector2){0, 0}, WHITE);

        DrawText("Retamente", (screenWidth - MeasureText("Retamente", 40)) / 2, 20, 40, DARKGRAY);

        DrawText(selectedOption == START_GAME ? "> Iniciar Juego" : "Iniciar Juego", (screenWidth - MeasureText("Iniciar Juego", 20)) / 2, screenHeight / 2 - 20, 20, DARKGRAY);
        DrawText(selectedOption == OPTIONS ? "> Opciones" : "Opciones", (screenWidth - MeasureText("Opciones", 20)) / 2, screenHeight / 2, 20, DARKGRAY);
        DrawText(selectedOption == EXIT ? "> Salir" : "Salir", (screenWidth - MeasureText("Salir", 20)) / 2, screenHeight / 2 + 20, 20, DARKGRAY);
        EndDrawing();
    }

    UnloadTexture(background);
    UnloadImage(bg_image);
    UnloadMusicStream(menuMusic);
    CloseWindow();

    return 0;
}

void playMusic(Music music)
{
    PlayMusicStream(music);
    UpdateMusicStream(music);

    timePlayed = GetMusicTimePlayed(music) / GetMusicTimeLength(music);

    if (timePlayed >= 0.99f)
        timePlayed = 1.0f;
}