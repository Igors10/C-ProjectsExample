#pragma once
#include "raylib.h"
#include <string>
#include <vector>

struct Wolf;

struct Level {
    int new_grass_growing_frequency = 100;  // How fast grass will grow
    int timer = 0;
    bool show_sight_radiuses = true;        // put to false, to hide radiuses in which sheep and wolves can "see"
    bool show_hp = true;                    // put to false, to hide hp indicators on animals
    int spawn_border = 20;                  // Nothing can spawn that many points close to a border
    int starting_sheep_count = 10;
    int starting_wolf_count = 3;

    std::vector<std::string> names{"Sofia", "Dona", "Igor", "Franek", "Rajat", "Erik", "Filippo", "Jacob", "Hakan", "Emily", "Sarah", "Jen", "Micheal", "Ben", "Tom", "Maris",
    "Janis", "Vitolds", "Liga", "Mara", "Lasma", "Laura", "Anna", "Tanya", "Vika", "Tema", "Vlad", "Edgar", "Stacy", "Ovca", "Queen", "John", "Jack", "Walter", "Natalie",
    "Matt", "Lance", "Hans", "Ilga", "Renate", "Ivan", "Stan", "Bjork", "Kurt", "Regina", "Lenin", "Lidia", "Jessy", "Jimmy", "Overlord", "Muhammed", "C*nt", "Lucy", "Alkam"};

    Level() {
       
    }
};

struct Entity //parent class
{
    float size = 0.f;
    Vector2 position;
    Color color;


};

struct Grass : Entity
{
    
    enum class Grass_State {
        Growing,
        Spreading,
        Idle,
        Decaying,
        Trampled,
        Dying

    };

    bool highlight;                    // Grass will highlight with red whenever it is being targeted by sheep
    Grass_State state;
    int hp;
    int lifetime;                      // How much frames grass was alive
    int time_to_die;                   // If lifetime reaches this number grass will start to decay
    float growing_speed = 0;           
    int max_hp;                        // if max_hp is reached grass will start to spread around
    Color color;
    int id;

    Grass(float xpos, float ypos, int id_in_the_vector) {
        highlight = false;
        id = id_in_the_vector;
        position.x = xpos;
        position.y = ypos;
        size = 2.0f;
        hp = 5;
        max_hp = 100;
        time_to_die = 400;
        lifetime = GetRandomValue(0, 100);               // lifetime is randomized for every grass, so that their Sense, Decide and growing/decaying trigger times are different
        state = Grass_State::Growing;
        color = Color{ 0, 200, 60, 255 };
        
    }

    void GrassSense();
    void GrassDecide();
    void GrassAct();
    void GrassDie();
};


struct Sheep : Entity
{
    int hp = 0;
    int lifetime = 0;
    int max_hp = 0;                           // After reaching this value sheep will breed
    float sight_radius = 0.f;                 // How far sheep can see wolves

    Vector2 speed;
    int speed_change_time = 0;                // How freaquent sheep will change its way while wandering
    int age_rate = 0;                         // How fast sheep ages
    int no_breeding_time = 0;                 // After being born / giving birth sheep have time to do nothing
    Vector2 avoid_point;                      // This is where sheep will run to when avoiding wolves
    float speed_modifier = 0.f;

    std::string name;

    enum class Sheep_State {
        Wandering,
        Finding,
        Eating,
        Reproducing,
        Dying,
        Avoiding
    };
    Sheep_State state;
    int closest_grass_id;

    std::vector<Grass> grass_in_sight{};
    


    Sheep(float xpos, float ypos, std::string random_name)
    {
        speed_modifier = 1.f;
        name = random_name;
        position.x = xpos;
        position.y = ypos;
        size = 20.0f;
        sight_radius = 140.0f;
        hp = 30;
        max_hp = 130;
        lifetime = GetRandomValue(0, 100);
        speed.x = 0;
        speed.y = 0;
        speed_change_time = 100;
        no_breeding_time = 300;
        age_rate = 100;
    }

    void SheepSense();
    void SheepDecide();
    void SheepAct();
    void SheepDie();
    
};


struct Wolf : Entity
{                                                      // Many many variables are explained in sheep struct (I know many of those could be put under one class, but it was too late when I realised that)
    int hp = 0;
    int lifetime = 0;
    int max_hp = 0;
    float sight_radius = 0.f;
    float speed_modifier = 0.f;

    Vector2 speed;
    int speed_change_time = 0;
    int age_rate = 0;
    int no_breeding_time = 0;

    std::string name;


    enum class Wolf_State {
        Wandering,
        Finding,
        Eating,
        Reproducing,
        Dying,

    };
    Wolf_State state;
    
    Sheep* close_sheep;                                    // Pointer to a sheep to chase

    Wolf(float xpos, float ypos, std::string random_name)
    {
        speed_modifier = 1.f;
        close_sheep = NULL;
        name = random_name;
        position.x = xpos;
        position.y = ypos;
        size = 25.0f;
        sight_radius = 160.0f;
        hp = 30;
        max_hp = 130;
        lifetime = GetRandomValue(0, 100);
        speed.x = 0;
        speed.y = 0;
        speed_change_time = 100;
        no_breeding_time = 300;
        age_rate = 50;
    }

    void WolfSense();
    void WolfDecide();
    void WolfAct();
    void WolfDie();
    

};

void AddToVector();
void AnimalLogic();
void DrawSheep();
void DrawWolves();

void GrassLogic();
void DrawGrass();
void Game();
void InitLevel();