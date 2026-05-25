#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <string>
#include <vector>
#include "Menu.h"
#include "Car.h"

using namespace std;

struct ObstacleSimple {
    SDL_Rect rect;
    SDL_Rect imageRect;
    SDL_Texture* texture;
    int centerX, centerY;
};

struct ObstacleData {
    string file;
    int centerX, centerY;
    int hitW, hitH;
};

// Функция для запуска уровня
// Возвращает:
//   0 - возврат в меню выбора уровня (ESC или победа)
//   1 - перезапустить этот же уровень (R)
//   2 - выход из игры (крестик)
int runLevel(SDL_Renderer* renderer, int level, int carColor,
    SDL_Texture* bgTexture, SDL_Texture* parkTexture, SDL_Rect parkingZone,
    SDL_Texture* winTexture, SDL_Texture* loseTexture, SDL_Texture* menuLoseTexture,
    const vector<ObstacleData>& obstaclesDataEasy,
    const vector<ObstacleData>& obstaclesDataMedium,
    const vector<ObstacleData>& obstaclesDataHard) {

    // ========== НАСТРОЙКА СЛОЖНОСТИ ==========
    float speed = 3.0f;
    float rotationSpeed = 5.0f;
    float timeLimit = 0.0f;
    vector<ObstacleData> currentObstacles;

    if (level == 1) {
        speed = 2.5f;
        rotationSpeed = 4.0f;
        timeLimit = 0.0f;
        currentObstacles = obstaclesDataEasy;
    }
    else if (level == 2) {
        speed = 4.0f;
        rotationSpeed = 5.0f;
        timeLimit = 0.0f;
        currentObstacles = obstaclesDataMedium;
    }
    else if (level == 3) {
        speed = 6.0f;
        rotationSpeed = 6.0f;
        timeLimit = 45.0f;
        currentObstacles = obstaclesDataHard;
    }

    // ========== ЗАГРУЗКА ПРЕПЯТСТВИЙ ==========
    vector<ObstacleSimple> obstacles;
    for (const auto& data : currentObstacles) {
        SDL_Surface* surf = IMG_Load(data.file.c_str());
        if (surf != nullptr) {
            ObstacleSimple obs;
            obs.texture = SDL_CreateTextureFromSurface(renderer, surf);
            obs.centerX = data.centerX;
            obs.centerY = data.centerY;
            obs.rect.x = data.centerX - data.hitW / 2;
            obs.rect.y = data.centerY - data.hitH / 2;
            obs.rect.w = data.hitW;
            obs.rect.h = data.hitH;
            obs.imageRect.x = data.centerX - surf->w / 2;
            obs.imageRect.y = data.centerY - surf->h / 2;
            obs.imageRect.w = surf->w;
            obs.imageRect.h = surf->h;
            obstacles.push_back(obs);
            SDL_FreeSurface(surf);
        }
    }

    // ========== ЗАГРУЗКА ИГРОВОЙ МАШИНЫ ==========
    Car car;
    string carGameFile;
    switch (carColor) {
    case 0: carGameFile = "res/car_red_game.png"; break;
    case 1: carGameFile = "res/car_blue_game.png"; break;
    case 2: carGameFile = "res/car_yellow_game.png"; break;
    case 3: carGameFile = "res/car_pink_game.png"; break;
    default: carGameFile = "res/car_red_game.png"; break;
    }
    car.loadTexture(renderer, carGameFile);
    car.setScreenSize(1280, 731);
    car.setPosition(100.0f, 100.0f);
    car.setSpeed(speed);

    // ========== ИГРОВОЙ ЦИКЛ ==========
    bool levelActive = true;
    bool win = false;
    bool lose = false;
    bool collision = false;
    Uint32 startTime = SDL_GetTicks();

    bool keyUp = false;
    bool keyDown = false;
    bool keyLeft = false;
    bool keyRight = false;

    const int BORDER_WIDTH = 80;

    while (levelActive) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return 2; // Выход из игры
            }
            if (!win && !lose) {
                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_UP) keyUp = true;
                    else if (event.key.keysym.sym == SDLK_DOWN) keyDown = true;
                    else if (event.key.keysym.sym == SDLK_LEFT) keyLeft = true;
                    else if (event.key.keysym.sym == SDLK_RIGHT) keyRight = true;
                }
                if (event.type == SDL_KEYUP) {
                    if (event.key.keysym.sym == SDLK_UP) keyUp = false;
                    else if (event.key.keysym.sym == SDLK_DOWN) keyDown = false;
                    else if (event.key.keysym.sym == SDLK_LEFT) keyLeft = false;
                    else if (event.key.keysym.sym == SDLK_RIGHT) keyRight = false;
                }
            }
        }

        if (!win && !lose) {
            float oldX = car.getX();
            float oldY = car.getY();

            if (keyUp) car.moveForward();
            if (keyDown) car.moveBackward();
            if (keyLeft) car.rotateLeft(rotationSpeed);
            if (keyRight) car.rotateRight(rotationSpeed);

            SDL_Rect carRect = car.getRect();
            collision = false;

            // Проверка столкновений с препятствиями
            for (size_t i = 0; i < obstacles.size(); i++) {
                if (carRect.x < obstacles[i].rect.x + obstacles[i].rect.w &&
                    carRect.x + carRect.w > obstacles[i].rect.x &&
                    carRect.y < obstacles[i].rect.y + obstacles[i].rect.h &&
                    carRect.y + carRect.h > obstacles[i].rect.y) {
                    collision = true;
                    break;
                }
            }

            // Проверка выезда за рамку
            if (!collision) {
                if (carRect.x < BORDER_WIDTH) collision = true;
                else if (carRect.x + carRect.w > 1280 - BORDER_WIDTH) collision = true;
                else if (carRect.y < BORDER_WIDTH) collision = true;
                else if (carRect.y + carRect.h > 731 - BORDER_WIDTH) collision = true;
            }

            if (collision) {
                lose = true;
            }

            carRect = car.getRect();
            if (!lose && carRect.x >= parkingZone.x && carRect.x + carRect.w <= parkingZone.x + parkingZone.w &&
                carRect.y >= parkingZone.y && carRect.y + carRect.h <= parkingZone.y + parkingZone.h) {
                win = true;
            }

            if (!win && !lose && level == 3 && timeLimit > 0.0f) {
                float elapsed = (SDL_GetTicks() - startTime) / 1000.0f;
                if (elapsed >= timeLimit) {
                    lose = true;
                }
            }
        }

        // ОТРИСОВКА
        SDL_RenderClear(renderer);
        if (bgTexture != nullptr) SDL_RenderCopy(renderer, bgTexture, NULL, NULL);
        if (parkTexture != nullptr) SDL_RenderCopy(renderer, parkTexture, NULL, &parkingZone);
        for (size_t i = 0; i < obstacles.size(); i++) {
            SDL_RenderCopy(renderer, obstacles[i].texture, NULL, &obstacles[i].imageRect);
        }
        car.render(renderer);

        SDL_Color white;
        white.r = 255; white.g = 255; white.b = 255; white.a = 255;
        string levelText = "Level: " + to_string(level);
        Menu::drawText(renderer, levelText, 20, 20, white);

        if (level == 3 && !win && !lose) {
            float elapsed = (SDL_GetTicks() - startTime) / 1000.0f;
            float remaining = timeLimit - elapsed;
            if (remaining < 0) remaining = 0;
            int minutes = (int)remaining / 60;
            int seconds = (int)remaining % 60;
            string timeText = "Time: " + to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + to_string(seconds);
            if (remaining < 10) {
                SDL_Color red;
                red.r = 255; red.g = 50; red.b = 50; red.a = 255;
                Menu::drawText(renderer, timeText, 1000, 20, red);
            }
            else {
                Menu::drawText(renderer, timeText, 1000, 20, white);
            }
        }

        SDL_RenderPresent(renderer);

        // ========== ОБРАБОТКА ПОБЕДЫ ==========
        if (win) {
            SDL_Delay(500);
            Uint32 showStart = SDL_GetTicks();
            while (SDL_GetTicks() - showStart < 3000) {
                SDL_Event e;
                while (SDL_PollEvent(&e)) {
                    if (e.type == SDL_QUIT) return 2;
                }
                SDL_RenderClear(renderer);
                if (winTexture != nullptr) SDL_RenderCopy(renderer, winTexture, NULL, NULL);
                SDL_RenderPresent(renderer);
                SDL_Delay(16);
            }
            // Очищаем память препятствий
            for (size_t i = 0; i < obstacles.size(); i++) {
                SDL_DestroyTexture(obstacles[i].texture);
            }
            return 0; // Возврат в меню
        }

        // ========== ОБРАБОТКА ПОРАЖЕНИЯ ==========
        if (lose) {
            // Показываем lose.png 1 секунду
            SDL_Delay(500);
            Uint32 showStart = SDL_GetTicks();
            while (SDL_GetTicks() - showStart < 1000) {
                SDL_Event e;
                while (SDL_PollEvent(&e)) {
                    if (e.type == SDL_QUIT) return 2;
                }
                SDL_RenderClear(renderer);
                if (loseTexture != nullptr) SDL_RenderCopy(renderer, loseTexture, NULL, NULL);
                SDL_RenderPresent(renderer);
                SDL_Delay(16);
            }

            // Показываем menu_lose.png и ждём нажатия R или ESC
            bool waitingForInput = true;
            while (waitingForInput) {
                SDL_Event e;
                while (SDL_PollEvent(&e)) {
                    if (e.type == SDL_QUIT) return 2;
                    if (e.type == SDL_KEYDOWN) {
                        if (e.key.keysym.sym == SDLK_r) {
                            // Перезапуск уровня (R)
                            for (size_t i = 0; i < obstacles.size(); i++) {
                                SDL_DestroyTexture(obstacles[i].texture);
                            }
                            return 1; // 1 = перезапустить этот же уровень
                        }
                        else if (e.key.keysym.sym == SDLK_ESCAPE) {
                            waitingForInput = false;
                        }
                    }
                }

                SDL_RenderClear(renderer);
                if (menuLoseTexture != nullptr) {
                    SDL_RenderCopy(renderer, menuLoseTexture, NULL, NULL);
                }
                else {
                    SDL_SetRenderDrawColor(renderer, 50, 50, 80, 255);
                    SDL_RenderClear(renderer);
                    SDL_Color whiteText;
                    whiteText.r = 255; whiteText.g = 255; whiteText.b = 255; whiteText.a = 255;
                    Menu::drawText(renderer, "Press R to restart level", 500, 350, whiteText);
                    Menu::drawText(renderer, "Press ESC to main menu", 520, 420, whiteText);
                }
                SDL_RenderPresent(renderer);
                SDL_Delay(16);
            }
            // Очищаем память препятствий
            for (size_t i = 0; i < obstacles.size(); i++) {
                SDL_DestroyTexture(obstacles[i].texture);
            }
            return 0; // Возврат в меню
        }

        SDL_Delay(16);
    }

    return 0;
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cout << "Ошибка SDL: " << SDL_GetError() << endl;
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        cout << "Ошибка SDL_image: " << IMG_GetError() << endl;
        return 1;
    }

    if (!Menu::initFont("res/font.ttf", 36)) {
        cout << "Не удалось загрузить шрифт" << endl;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Симулятор Парковки - Курсовая",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280, 731,
        SDL_WINDOW_SHOWN
    );

    if (window == nullptr) {
        cout << "Ошибка окна: " << SDL_GetError() << endl;
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        cout << "Ошибка рендера: " << SDL_GetError() << endl;
        return 1;
    }

    // ========== ЗАГРУЗКА ФОНА ДЛЯ МЕНЮ ==========
    if (!Menu::loadMenuBackground(renderer, "res/menu.png")) {
        cout << "Не удалось загрузить фон меню" << endl;
    }
    if (!Menu::loadCarSelectionBackground(renderer, "res/menu_car.png")) {
        cout << "Не удалось загрузить фон выбора машины" << endl;
    }

    // Загружаем фон уровня
    SDL_Surface* bgSurface = IMG_Load("res/map.png");
    SDL_Texture* bgTexture = nullptr;
    if (bgSurface != nullptr) {
        bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
        SDL_FreeSurface(bgSurface);
        cout << "Фон загружен" << endl;
    }

    // Загружаем зону парковки
    SDL_Surface* parkSurface = IMG_Load("res/parking_zone.png");
    SDL_Texture* parkTexture = nullptr;
    if (parkSurface != nullptr) {
        parkTexture = SDL_CreateTextureFromSurface(renderer, parkSurface);
        SDL_FreeSurface(parkSurface);
        cout << "Зона парковки загружена" << endl;
    }

    // Зона парковки
    int parkingCenterX = 1080;
    int parkingCenterY = 530;
    int parkingW = 150;
    int parkingH = 150;
    SDL_Rect parkingZone;
    parkingZone.x = parkingCenterX - parkingW / 2;
    parkingZone.y = parkingCenterY - parkingH / 2;
    parkingZone.w = parkingW;
    parkingZone.h = parkingH;

    // Загружаем картинки победы и поражения
    SDL_Surface* winSurface = IMG_Load("res/win.png");
    SDL_Texture* winTexture = nullptr;
    if (winSurface != nullptr) {
        winTexture = SDL_CreateTextureFromSurface(renderer, winSurface);
        SDL_FreeSurface(winSurface);
    }

    SDL_Surface* loseSurface = IMG_Load("res/lose.png");
    SDL_Texture* loseTexture = nullptr;
    if (loseSurface != nullptr) {
        loseTexture = SDL_CreateTextureFromSurface(renderer, loseSurface);
        SDL_FreeSurface(loseSurface);
    }

    SDL_Surface* menuLoseSurface = IMG_Load("res/menu_lose.png");
    SDL_Texture* menuLoseTexture = nullptr;
    if (menuLoseSurface != nullptr) {
        menuLoseTexture = SDL_CreateTextureFromSurface(renderer, menuLoseSurface);
        SDL_FreeSurface(menuLoseSurface);
    }

    // Картинки для меню выбора машины
    SDL_Texture* carMenuTextures[4] = { nullptr, nullptr, nullptr, nullptr };
    string carMenuFiles[4] = { "res/car_red.png", "res/car_blue.png", "res/car_yellow.png", "res/car_pink.png" };
    for (int i = 0; i < 4; i++) {
        SDL_Surface* surf = IMG_Load(carMenuFiles[i].c_str());
        if (surf != nullptr) {
            carMenuTextures[i] = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_FreeSurface(surf);
        }
    }

    // ========== НАБОРЫ ПРЕПЯТСТВИЙ ==========
    vector<ObstacleData> obstaclesDataEasy = {
        {"res/stena_ob.png", 900, 100, 103, 119},
        {"res/stena_ob.png", 200, 520, 103, 119},
        {"res/tree_ob.png", 600, 300, 100, 100},
    };

    vector<ObstacleData> obstaclesDataMedium = {
        {"res/flower_ob.png", 920, 200, 30, 30},
        {"res/stena_ob.png", 550, 200, 100, 100},
        {"res/stena_ob.png", 650, 280, 100, 100},
        {"res/mushroom_ob.png", 370, 500, 80, 200},
        {"res/tree_ob.png", 1000, 100, 50, 150},
        {"res/tree_ob.png", 100, 290, 50, 150},
    };

    vector<ObstacleData> obstaclesDataHard = {
        {"res/flower_ob.png", 300, 400, 20, 20},
        {"res/flower_ob.png", 350, 420, 20, 20},
        {"res/flower_ob.png", 750, 220, 20, 20},
        {"res/mushroom_ob.png", 500, 200, 40, 80},
        {"res/stolb_ob.png", 800, 350, 80, 110},
        {"res/tree_ob.png", 950, 600, 50, 150},
        {"res/stena_ob.png",550, 300, 100, 100},
        {"res/tree_ob.png", 100, 290, 50, 150},
        {"res/stolb_ob.png", 920, 350, 80, 110},
        {"res/mushroom_ob.png", 1150, 200, 40, 200},
        {"res/tree_ob.png", 140, 590, 50, 150},
    };

    // ========== ОСНОВНОЙ ЦИКЛ ИГРЫ ==========
    bool gameRunning = true;
    int currentLevel = 0;
    int currentCarColor = 0;

    while (gameRunning) {
        // Если нет текущего уровня (первый запуск или ESC), показываем меню
        if (currentLevel == 0) {
            currentLevel = Menu::showMainMenu(renderer);
            if (currentLevel == 0) {
                gameRunning = false;
                break;
            }

            currentCarColor = Menu::showCarSelection(renderer, carMenuTextures);
            if (currentCarColor == -1) currentCarColor = 0;
        }

        // Запускаем уровень
        int levelResult = runLevel(renderer, currentLevel, currentCarColor,
            bgTexture, parkTexture, parkingZone,
            winTexture, loseTexture, menuLoseTexture,
            obstaclesDataEasy, obstaclesDataMedium, obstaclesDataHard);

        if (levelResult == 2) {
            // Выход из игры
            gameRunning = false;
            break;
        }
        else if (levelResult == 1) {
            // Перезапуск того же уровня (R) - просто продолжаем цикл с тем же currentLevel
            continue;
        }
        else {
            // Возврат в меню (0) - сбрасываем текущий уровень
            currentLevel = 0;
            continue;
        }
    }

    // ========== ОЧИСТКА ПАМЯТИ ==========
    if (bgTexture != nullptr) SDL_DestroyTexture(bgTexture);
    if (parkTexture != nullptr) SDL_DestroyTexture(parkTexture);
    if (winTexture != nullptr) SDL_DestroyTexture(winTexture);
    if (loseTexture != nullptr) SDL_DestroyTexture(loseTexture);
    if (menuLoseTexture != nullptr) SDL_DestroyTexture(menuLoseTexture);

    for (int i = 0; i < 4; i++) {
        if (carMenuTextures[i] != nullptr) SDL_DestroyTexture(carMenuTextures[i]);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    Menu::unloadCarSelectionBackground();
    Menu::unloadMenuBackground();
    Menu::closeFont();
    SDL_Quit();

    return 0;
}
