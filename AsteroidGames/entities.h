#pragma once
#include "raylib.h"
#include <string>



class Entity
{
public:
    
    Vector2 Position;
    
};

class Player
{
public:

    Vector2 Position;
    bool ready_to_shoot;
    int laser_charge;
   float laser_x = 0;
   float bullet_x = 0;
    int timer;
    bool start = false;
    bool invisibility = true;
    bool blinking = true;
    int score = 0;
};

class Explosion
{
public:
    Vector2 Position;
    
    int timer = 0;
    float texture_x = 100;
    
};

class Manager
{
public:
    int score_bonus = 50;
    int combo_timer = 0;
    int highscore = 0;
    bool new_highscore = false;

    int difficulty = 0;
    int next_difficulty = 2000; // to track difficulty

    Texture2D rock_texture;
    Texture2D explosion_texture;
    Texture2D starship_texture;
    Texture2D bullet_texture;
    Texture2D lasercharge_texture;

    Sound shot_sound;
    Sound death_sound;
    Sound explosion_sound;
    Sound gem_sound;
};

class Rock
{
public:

    Vector2 Position;
    double y_speed;
    double x_speed;
    int type;
    float rotation = 0;
    float Random_movement = 0;

    
    Texture2D texture;
};

class Gem
{
public:
    Vector2 Position;
    int time = 20;
    int speed;
    float x_speed;
    int score;
    bool collected = false;
    int collected_at_the_moment = 0;
};



void InitPlayer();
void RenderPlayer();
void UpdatePlayer();
void RockUpdate();
void UpdateGems();
void LoadAssets();
void Explosion_animation();
void Difficulty();
void UnloadAssets();