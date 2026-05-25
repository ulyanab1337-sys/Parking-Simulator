#include "Obstacle.h"
#include <SDL_image.h>
#include <iostream>

// ============ Одиночное препятствие ============

Obstacle::Obstacle() {
    rect = { 0, 0, 0, 0 };
    texture = nullptr;
}

Obstacle::~Obstacle() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

bool Obstacle::load(SDL_Renderer* renderer, const std::string& file, int x, int y, int w, int h) {
    SDL_Surface* surf = IMG_Load(file.c_str());
    if (!surf) {
        std::cout << "Не загружено: " << file << std::endl;
        return false;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    rect = { x, y, w, h };
    return texture != nullptr;
}

SDL_Rect Obstacle::getRect() {
    return rect;
}

void Obstacle::render(SDL_Renderer* renderer) {
    if (texture) {
        SDL_RenderCopy(renderer, texture, NULL, &rect);
    }
}

bool Obstacle::checkCollision(const SDL_Rect& carRect) {
    return (carRect.x < rect.x + rect.w &&
        carRect.x + carRect.w > rect.x &&
        carRect.y < rect.y + rect.h &&
        carRect.y + carRect.h > rect.y);
}

// ============ Менеджер препятствий ============

ObstacleManager::~ObstacleManager() {
    clear();
}

void ObstacleManager::addObstacle(Obstacle obs) {
    obstacles.push_back(obs);
}

bool ObstacleManager::loadFromLevel(SDL_Renderer* renderer, int level) {
    clear();

    // Здесь можно загружать разные наборы для разных уровней
    // Пока загружаем все препятствия

    struct ObData {
        std::string file;
        int x, y, w, h;
    };

    std::vector<ObData> allObstacles = {
        {"res/flower_ob.png", 300, 400, 40, 40},
        {"res/flower_ob.png", 350, 420, 40, 40},
        {"res/mushroom_ob.png", 500, 200, 64, 64},
        {"res/stolb_ob.png", 800, 350, 64, 96},
        {"res/tree_ob.png", 950, 600, 64, 64},
        {"res/tree_ob.png", 100, 300, 64, 64},
        {"res/mushroom_ob.png", 1150, 200, 64, 64},
    };

    for (auto& ob : allObstacles) {
        Obstacle obs;
        if (obs.load(renderer, ob.file, ob.x, ob.y, ob.w, ob.h)) {
            addObstacle(obs);
        }
    }

    return true;
}

void ObstacleManager::renderAll(SDL_Renderer* renderer) {
    for (auto& obs : obstacles) {
        obs.render(renderer);
    }
}

bool ObstacleManager::checkAnyCollision(const SDL_Rect& carRect) {
    for (auto& obs : obstacles) {
        if (obs.checkCollision(carRect)) {
            return true;
        }
    }
    return false;
}

void ObstacleManager::clear() {
    obstacles.clear();
}