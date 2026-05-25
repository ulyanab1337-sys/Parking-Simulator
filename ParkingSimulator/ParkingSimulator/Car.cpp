#include "Car.h"
#include <iostream>

#define PI 3.14159265358979323846

Car::Car() {
    rect.x = 100;
    rect.y = 600;
    rect.w = 100;
    rect.h = 60;
    x = 100.0f;
    y = 600.0f;
    angle = 0.0;
    speed = 3.0f;
    texture = nullptr;
    screenWidth = 1280;
    screenHeight = 731;
}

Car::~Car() {
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

bool Car::loadTexture(SDL_Renderer* renderer, const std::string& filename) {
    SDL_Surface* surf = IMG_Load(filename.c_str());
    if (surf == nullptr) {
        std::cout << "Ошибка загрузки: " << filename << " - " << IMG_GetError() << std::endl;
        return false;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    return (texture != nullptr);
}

void Car::setPosition(float px, float py) {
    x = px;
    y = py;
    update();
}

void Car::setPositionCenter(float cx, float cy) {
    // cx, cy - центр машины
    x = cx - rect.w / 2.0f;
    y = cy - rect.h / 2.0f;
    update();
}

float Car::getCenterX() {
    return x + rect.w / 2.0f;
}

float Car::getCenterY() {
    return y + rect.h / 2.0f;
}

void Car::setSize(int w, int h) {
    // Сохраняем центр, меняем размер, восстанавливаем центр
    float centerX = getCenterX();
    float centerY = getCenterY();
    rect.w = w;
    rect.h = h;
    setPositionCenter(centerX, centerY);
}

int Car::getWidth() {
    return rect.w;
}

int Car::getHeight() {
    return rect.h;
}

void Car::setSpeed(float sp) {
    speed = sp;
}

void Car::setScreenSize(int w, int h) {
    screenWidth = w;
    screenHeight = h;
}

void Car::moveForward() {
    // Для движения вперёд используем угол + 90 градусов
    // (поправка, если картинка смотрит не вверх)
    double rad = (angle + 90.0) * PI / 180.0;
    x = x + speed * (float)cos(rad);
    y = y + speed * (float)sin(rad);
    update();
}

void Car::moveBackward() {
    double rad = (angle + 90.0) * PI / 180.0;
    x = x - speed * (float)cos(rad);
    y = y - speed * (float)sin(rad);
    update();
}

void Car::rotateLeft(float degrees) {
    angle = angle - degrees;
    if (angle < 0) {
        angle = angle + 360.0;
    }
}

void Car::rotateRight(float degrees) {
    angle = angle + degrees;
    if (angle >= 360.0) {
        angle = angle - 360.0;
    }
}

void Car::update() {
    rect.x = (int)x;
    rect.y = (int)y;

    if (rect.x < 0) {
        rect.x = 0;
        x = 0;
    }
    if (rect.y < 0) {
        rect.y = 0;
        y = 0;
    }
    if (rect.x + rect.w > screenWidth) {
        rect.x = screenWidth - rect.w;
        x = (float)rect.x;
    }
    if (rect.y + rect.h > screenHeight) {
        rect.y = screenHeight - rect.h;
        y = (float)rect.y;
    }
}

SDL_Rect Car::getRect() {
    return rect;
}

void Car::render(SDL_Renderer* renderer) {
    if (texture != nullptr) {
        SDL_Point center;
        center.x = rect.w / 2;
        center.y = rect.h / 2;
        // При отрисовке тоже добавляем поправку 90 градусов
        SDL_RenderCopyEx(renderer, texture, NULL, &rect, angle + 90.0, &center, SDL_FLIP_NONE);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
}

float Car::getX() {
    return x;
}

float Car::getY() {
    return y;
}

double Car::getAngle() {
    return angle;
}
