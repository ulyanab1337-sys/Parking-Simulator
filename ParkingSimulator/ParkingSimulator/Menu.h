#ifndef MENU_H
#define MENU_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <string>

class Menu {
private:
    static TTF_Font* font;
    static SDL_Texture* menuBackground;
    static SDL_Texture* carSelectionBackground;  // ДОБАВЛЕНО - фон для выбора машины

public:
    static bool initFont(const std::string& fontPath, int fontSize);
    static void closeFont();
    static void drawText(SDL_Renderer* renderer, const std::string& text, int x, int y, SDL_Color color);

    static bool loadMenuBackground(SDL_Renderer* renderer, const std::string& filePath);
    static void unloadMenuBackground();

    // ДОБАВЛЕНЫ НОВЫЕ МЕТОДЫ ДЛЯ ФОНА ВЫБОРА МАШИНЫ
    static bool loadCarSelectionBackground(SDL_Renderer* renderer, const std::string& filePath);
    static void unloadCarSelectionBackground();

    static int showMainMenu(SDL_Renderer* renderer);
    static int showCarSelection(SDL_Renderer* renderer, SDL_Texture* carTextures[4]);
    static void showWinMessage(SDL_Renderer* renderer);
    static void showLoseMessage(SDL_Renderer* renderer);
};

#endif
