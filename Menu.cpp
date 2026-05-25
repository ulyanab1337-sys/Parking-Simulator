#include "Menu.h"
#include <SDL_image.h>
#include <iostream>

TTF_Font* Menu::font = nullptr;
SDL_Texture* Menu::menuBackground = nullptr;
SDL_Texture* Menu::carSelectionBackground = nullptr;

bool Menu::initFont(const std::string& fontPath, int fontSize) {
    if (TTF_Init() == -1) {
        std::cout << "Ошибка TTF_Init: " << TTF_GetError() << std::endl;
        return false;
    }

    font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (font == nullptr) {
        std::cout << "Ошибка загрузки шрифта: " << TTF_GetError() << std::endl;
        return false;
    }

    return true;
}

bool Menu::loadMenuBackground(SDL_Renderer* renderer, const std::string& filePath) {
    SDL_Surface* surf = IMG_Load(filePath.c_str());
    if (surf == nullptr) {
        std::cout << "Не загружен фон меню: " << filePath << " - " << IMG_GetError() << std::endl;
        return false;
    }
    menuBackground = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    std::cout << "Фон меню загружен: " << filePath << std::endl;
    return menuBackground != nullptr;
}

void Menu::unloadMenuBackground() {
    if (menuBackground != nullptr) {
        SDL_DestroyTexture(menuBackground);
        menuBackground = nullptr;
    }
}

bool Menu::loadCarSelectionBackground(SDL_Renderer* renderer, const std::string& filePath) {
    SDL_Surface* surf = IMG_Load(filePath.c_str());
    if (surf == nullptr) {
        std::cout << "Не загружен фон выбора машины: " << filePath << " - " << IMG_GetError() << std::endl;
        return false;
    }
    carSelectionBackground = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    std::cout << "Фон выбора машины загружен: " << filePath << std::endl;
    return carSelectionBackground != nullptr;
}

void Menu::unloadCarSelectionBackground() {
    if (carSelectionBackground != nullptr) {
        SDL_DestroyTexture(carSelectionBackground);
        carSelectionBackground = nullptr;
    }
}

void Menu::closeFont() {
    if (font != nullptr) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    TTF_Quit();
}

void Menu::drawText(SDL_Renderer* renderer, const std::string& text, int x, int y, SDL_Color color) {
    if (font == nullptr) return;

    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if (surface == nullptr) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = surface->w;
    rect.h = surface->h;

    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

int Menu::showMainMenu(SDL_Renderer* renderer) {
    SDL_Rect easyRect;
    easyRect.x = 270;
    easyRect.y = 530;
    easyRect.w = 200;
    easyRect.h = 50;

    SDL_Rect mediumRect;
    mediumRect.x = 555;
    mediumRect.y = 530;
    mediumRect.w = 200;
    mediumRect.h = 50;

    SDL_Rect hardRect;
    hardRect.x = 895;
    hardRect.y = 530;
    hardRect.w = 200;
    hardRect.h = 50;

    bool selecting = true;
    int selected = 0;

    while (selecting) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                return 0;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                int mouseX = e.button.x;
                int mouseY = e.button.y;

                if (mouseX >= easyRect.x && mouseX <= easyRect.x + easyRect.w &&
                    mouseY >= easyRect.y && mouseY <= easyRect.y + easyRect.h) {
                    selected = 1;
                    selecting = false;
                }
                if (mouseX >= mediumRect.x && mouseX <= mediumRect.x + mediumRect.w &&
                    mouseY >= mediumRect.y && mouseY <= mediumRect.y + mediumRect.h) {
                    selected = 2;
                    selecting = false;
                }
                if (mouseX >= hardRect.x && mouseX <= hardRect.x + hardRect.w &&
                    mouseY >= hardRect.y && mouseY <= hardRect.y + hardRect.h) {
                    selected = 3;
                    selecting = false;
                }
            }
        }

        // ОТРИСОВКА С ФОНОМ
        if (menuBackground != nullptr) {
            SDL_RenderCopy(renderer, menuBackground, NULL, NULL);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 30, 30, 50, 255);
            SDL_RenderClear(renderer);
        }

        SDL_Color easyColor;
        easyColor.r = 100;
        easyColor.g = 255;
        easyColor.b = 100;
        easyColor.a = 255;
        drawText(renderer, "Easy", 270, 530, easyColor);

        SDL_Color mediumColor;
        mediumColor.r = 255;
        mediumColor.g = 255;
        mediumColor.b = 100;
        mediumColor.a = 255;
        drawText(renderer, "Medium", 555, 530, mediumColor);

        SDL_Color hardColor;
        hardColor.r = 255;
        hardColor.g = 100;
        hardColor.b = 100;
        hardColor.a = 255;
        drawText(renderer, "Hard", 895, 530, hardColor);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    return selected;
}

int Menu::showCarSelection(SDL_Renderer* renderer, SDL_Texture* carTextures[4]) {
    SDL_Rect carRects[4];

    carRects[0].x = 135;
    carRects[0].y = 200;
    carRects[0].w = 250;
    carRects[0].h = 441;

    carRects[1].x = 350;
    carRects[1].y = 200;
    carRects[1].w = 260;
    carRects[1].h = 428;

    carRects[2].x = 600;
    carRects[2].y = 200;
    carRects[2].w = 245;
    carRects[2].h = 422;

    carRects[3].x = 850;
    carRects[3].y = 200;
    carRects[3].w = 255;
    carRects[3].h = 410;

    bool selecting = true;
    int selected = -1;

    while (selecting) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                return 0;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                int mouseX = e.button.x;
                int mouseY = e.button.y;

                for (int i = 0; i < 4; i++) {
                    if (mouseX >= carRects[i].x && mouseX <= carRects[i].x + carRects[i].w &&
                        mouseY >= carRects[i].y && mouseY <= carRects[i].y + carRects[i].h) {
                        selected = i;
                        selecting = false;
                    }
                }
            }
        }

        // ОТРИСОВКА С ФОНОМ ДЛЯ ВЫБОРА МАШИНЫ
        if (carSelectionBackground != nullptr) {
            SDL_RenderCopy(renderer, carSelectionBackground, NULL, NULL);
        }
        else if (menuBackground != nullptr) {
            SDL_RenderCopy(renderer, menuBackground, NULL, NULL);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
            SDL_RenderClear(renderer);
        }


        for (int i = 0; i < 4; i++) {

            if (carTextures[i] != nullptr) {
                SDL_RenderCopy(renderer, carTextures[i], NULL, &carRects[i]);
            }
        }

        SDL_Color white;
        white.r = 255;
        white.g = 255;
        white.b = 255;
        white.a = 255;
        drawText(renderer, "Red", 200, 610, white);
        drawText(renderer, "Blue", 430, 610, white);
        drawText(renderer, "Yellow", 650, 610, white);
        drawText(renderer, "Pink", 930, 610, white);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    return selected;
}

void Menu::showWinMessage(SDL_Renderer* renderer) {
    Uint32 startTime = SDL_GetTicks();
    while (SDL_GetTicks() - startTime < 3000) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return;
        }

        SDL_SetRenderDrawColor(renderer, 0, 100, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Color winColor;
        winColor.r = 255;
        winColor.g = 215;
        winColor.b = 0;
        winColor.a = 255;
        drawText(renderer, "YOU WIN!", 560, 350, winColor);
        drawText(renderer, "Perfect parking!", 540, 420, winColor);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

void Menu::showLoseMessage(SDL_Renderer* renderer) {
    Uint32 startTime = SDL_GetTicks();
    while (SDL_GetTicks() - startTime < 3000) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return;
        }

        SDL_SetRenderDrawColor(renderer, 100, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Color loseColor;
        loseColor.r = 255;
        loseColor.g = 100;
        loseColor.b = 100;
        loseColor.a = 255;
        drawText(renderer, "YOU LOSE!", 560, 350, loseColor);
        drawText(renderer, "Time is over!", 540, 420, loseColor);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}