#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <SDL.h>
#include <vector>
#include <string>

class Obstacle {
private:
    SDL_Rect rect;
    SDL_Texture* texture;
    std::string name;

public:
    Obstacle();
    ~Obstacle();

    bool load(SDL_Renderer* renderer, const std::string& file, int x, int y, int w, int h);
    SDL_Rect getRect();
    void render(SDL_Renderer* renderer);
    bool checkCollision(const SDL_Rect& carRect);
};

class ObstacleManager {
private:
    std::vector<Obstacle> obstacles;

public:
    ~ObstacleManager();
    void addObstacle(Obstacle obs);
    bool loadFromLevel(SDL_Renderer* renderer, int level);
    void renderAll(SDL_Renderer* renderer);
    bool checkAnyCollision(const SDL_Rect& carRect);
    void clear();
};

#endif