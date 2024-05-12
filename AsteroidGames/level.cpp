#include "raylib.h"
#include <string>
#include <iostream>
#include <vector>
#include <time.h>



struct star {
    Vector2 Position;
    float radius;
};

std::vector<star> stars{};

// Generating beautiful space background

void InitStars() {

    SetRandomSeed(time(NULL));

    for (int x = 0; x < 700; x = x + 100)
    {
        for (int y = 0; y < 750; y = y + 60)
        {
            star new_star;
            new_star.Position.x = GetRandomValue(x, x + 100);
            new_star.Position.y = GetRandomValue(y, y + 60);
            new_star.radius = GetRandomValue(1, 2);
            stars.push_back(new_star);
        }
    }
}

// Making our beautiful background dynamic as well

void Renderlevel()
{
    ClearBackground(BLACK);
   

    for (int i = 0; i < stars.size(); i++)
    {
        stars[i].Position.y = stars[i].Position.y + 4;     // Flying speed effect
        if (stars[i].Position.y > 750)
        {
            stars[i].Position.y = 0;                       // Whenever a star reaches bottom it reapears at the top
        }
        
        DrawCircle(stars[i].Position.x, stars[i].Position.y, stars[i].radius, WHITE);

    }

 
        
}