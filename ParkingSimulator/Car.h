#ifndef CAR_H
#define CAR_H

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <cmath>

class Car {
private:
    SDL_Rect rect;
    SDL_Texture* texture;
    double angle;      // угол поворота в градусах
    float speed;       // скорость движения
    float x;           // точная координата X (float для плавности)
    float y;           // точная координата Y
    int screenWidth;
    int screenHeight;

public:
    Car();
    ~Car();

    bool loadTexture(SDL_Renderer* renderer, const std::string& filename);
    void setPosition(float px, float py);
    void setSpeed(float sp);
    void setScreenSize(int w, int h);
    void moveForward();      // движение вперёд по направлению
    void moveBackward();     // движение назад
    void rotateLeft(float degrees);
    void rotateRight(float degrees);
    void update();           // обновление позиции и границ
    SDL_Rect getRect();
    void render(SDL_Renderer* renderer);
    float getX();
    float getY();
    double getAngle();

    // ========== НОВЫЕ МЕТОДЫ ДЛЯ ЦЕНТРИРОВАНИЯ (без изменения старых) ==========
    void setPositionCenter(float cx, float cy);  // установка по центру
    float getCenterX();                           // получить центр X
    float getCenterY();                           // получить центр Y
    void setSize(int w, int h);                   // изменить размер машины
    int getWidth();                               // получить ширину
    int getHeight();                              // получить высоту
};

#endif
