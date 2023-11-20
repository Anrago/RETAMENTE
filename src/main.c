#include "raylib.h"

int main(void)
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Menu con Background - Raylib");

    // Estructura del menu
    enum MenuOption
    {
        START_GAME,
        OPTIONS,
        EXIT
    };

    int selectedOption = START_GAME;

    Texture2D background = LoadTexture("assets/bg_menu.jpg");

    while (!WindowShouldClose())
    {
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

        // Background
        DrawTexture(background, 0, 0, WHITE);

        // Menu
        DrawText("Retamente", (screenWidth - MeasureText("Retamente", 40)) / 2, 20, 40, DARKGRAY);

        DrawText(selectedOption == START_GAME ? "> Iniciar Juego" : "Iniciar Juego", (screenWidth - MeasureText("Iniciar Juego", 20)) / 2, screenHeight / 2 - 20, 20, DARKGRAY);
        DrawText(selectedOption == OPTIONS ? "> Opciones" : "Opciones", (screenWidth - MeasureText("Opciones", 20)) / 2, screenHeight / 2, 20, DARKGRAY);
        DrawText(selectedOption == EXIT ? "> Salir" : "Salir", (screenWidth - MeasureText("Salir", 20)) / 2, screenHeight / 2 + 20, 20, DARKGRAY);

        BeginDrawing();
        EndDrawing();
    }

    UnloadTexture(background);

    CloseWindow();

    return 0;
}
