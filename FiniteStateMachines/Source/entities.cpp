#pragma once
#include "../Header/entities.h"
#include "raylib.h"
#include <vector>
#include <iostream>
#include <queue>
#include <cmath>
#include <string.h>
using namespace std;

// Game elements are stored here

std::vector<Grass> grass{};
std::vector<Sheep> sheep{};
std::vector<Wolf> wolf{};

std::queue<Sheep> sheeps_to_add{};       // Those are queue for sheep and wolves to be added at the end of the frame
std::queue<Wolf> wolves_to_add{};

Level level;


void CreateNewGrass()
{
	float x_position = static_cast <float>(GetRandomValue(level.spawn_border, GetScreenWidth() - level.spawn_border));
	float y_position = static_cast <float>(GetRandomValue(level.spawn_border, GetScreenHeight() - level.spawn_border));

	Grass new_grass(x_position, y_position, static_cast <int>(grass.size()));
	grass.push_back(new_grass);
}

void InitLevel()
{
	//            Creating starting sheep

	for (int a = 0; a < level.starting_sheep_count; a++)
	{
		float x_position = static_cast <float>(GetRandomValue(level.spawn_border, GetScreenWidth() - level.spawn_border));
		float y_position = static_cast <float>(GetRandomValue(level.spawn_border, GetScreenHeight() - level.spawn_border));

		Sheep new_sheep(x_position, y_position, level.names[GetRandomValue(0, static_cast <int>(level.names.size() - 1))]);
		sheep.push_back(new_sheep);
	}

	//            Creating starting wolves

	for (int a = 0; a < level.starting_wolf_count; a++)
	{
		float x_position = static_cast <float>(GetRandomValue(level.spawn_border +10, GetScreenWidth() - level.spawn_border -10));
		float y_position = static_cast <float>(GetRandomValue(level.spawn_border +10, GetScreenHeight() - level.spawn_border -10));

		Wolf new_wolf(x_position, y_position, level.names[GetRandomValue(0, static_cast <int>(level.names.size() - 1))]);
		wolf.push_back(new_wolf);
	}
}

void AddToVector()                             // Here wolves and sheep that are born in the middle of the game are added to vectors
{
	while (sheeps_to_add.size() > 0)
	{
		sheep.push_back(sheeps_to_add.front());
		
		sheeps_to_add.pop();
		
	}
	while (wolves_to_add.size() > 0)
	{
		wolf.push_back(wolves_to_add.front());

		wolves_to_add.pop();
	}
}

void Game()                // This could have being named timer, but whatever
{
	level.timer++;
}

void Grass::GrassSense()      // Only thing that grass senses is it sees if it is being trampled or not
{
	for (int a = 0; a < sheep.size(); a++)
	{
		if (CheckCollisionCircles(position, size, sheep[a].position, sheep[a].size)) state = Grass_State::Trampled;
	}
	for (int a = 0; a < wolf.size(); a++)
	{
		if (CheckCollisionCircles(position, size, wolf[a].position, wolf[a].size)) state = Grass_State::Trampled;
	}
	
}

void Grass::GrassDecide()
{
	if (hp <= 0) state = Grass_State::Dying;                                         // If grass has hp below 0 it dies
	else if (state == Grass_State::Trampled                                          // If it is trampled/decaying it doesnt change its state
	|| state == Grass_State::Dying || state == Grass_State::Decaying) return;
	else if (lifetime >= time_to_die) state = Grass_State::Decaying;                 // If the grass is too old it will start to decay
	else if (hp >= max_hp) state = Grass_State::Spreading;                           // Grass can also spread if it reaches max hp
	else state = Grass_State::Growing;                                               // default state of grass is growing
}

void Grass::GrassAct()
{
	switch (state)
	{
	case Grass_State::Growing:

		hp++;
		if (hp % 10 == 0) size++;
		color = Color{ 0, 200, 60, 255 };

		break;

	case Grass_State::Spreading:

		color = GREEN;

		if (lifetime % 100 == 0)                 // Once every 100 frames fully grown grass spreads around
		{
			float x_position_offset;
			// commbnet
			float y_position_offset;

			
			for (int a = 0; a < 10; a++)
			{
				bool location_is_fine = true;
				x_position_offset = static_cast <float>(GetRandomValue(-50, 50));                // New grass is created next to its parent
				y_position_offset = static_cast <float>(GetRandomValue(-50, 50));
				for (int b = 0; b < grass.size(); b++)
				{
					if (CheckCollisionPointCircle({ position.x + x_position_offset, position.y + y_position_offset }, grass[b].position, grass[b].size) ||   // Here it checks if randomly chosen position is in
						position.y + y_position_offset > GetScreenHeight() - level.spawn_border || position.x + x_position_offset >                          // borders of the world and not overlaping with another grass
						GetScreenWidth() - level.spawn_border || position.y + y_position_offset < level.spawn_border 
						|| position.x + x_position_offset < level.spawn_border)
					{
						location_is_fine = false;
						break;
					}
				}
				if (location_is_fine && grass.size() < 100)  // Add a max amount of grass that can be on the screen so it won't lag
				{
					Grass new_grass(position.x + x_position_offset, position.y + y_position_offset, static_cast <int>(grass.size()));
					grass.push_back(new_grass);
					break;
				}
			}
		}

		break;

	case Grass_State::Decaying:

		hp--;
		if (hp % 10 == 0) size--;
		color = BROWN;

		break;

	case Grass_State::Dying:

		GrassDie();

		break;
	}
}

void Grass::GrassDie()                               //Here the grass is being deleted from the simulation
{
	for (int a = 0; a < grass.size(); a++)
	{
		if (position.x == grass[a].position.x)
		{
			grass.erase(grass.begin() + a);
			
		}
	}
}

void GrassLogic()
{
	// New Grass Grows

	if (level.timer % level.new_grass_growing_frequency == 0)
	{
		CreateNewGrass();
	}

	// Grasses sense/decide/act cycle

	for (int a = 0; a < grass.size(); a++)
	{
		grass[a].lifetime++;
		
		if (grass[a].lifetime % 5 == 0)
		{
			grass[a].GrassSense();
			grass[a].GrassDecide();
		}

		grass[a].GrassAct();
		
	}

}

void DrawGrass()
{
	for (int a = 0; a < grass.size(); a++)
	{
		DrawCircle(static_cast <int>(grass[a].position.x), static_cast <int>(grass[a].position.y), grass[a].size, grass[a].color);
	}
}


Vector2 NormalizeVector(Vector2 vector)
{
	double length = sqrt(pow(vector.x, 2) + pow(vector.y, 2));
	vector.x = vector.x / static_cast <float>(length);
	vector.y = vector.y / static_cast <float>(length);

	return vector;
}



Vector2 GoTo(float x, float y, Vector2 position)  // Function that directs sheep's or wolves movement to a certain point
{

	Vector2 difference;
	difference.x = x - position.x;
	difference.y = y - position.y;

	return NormalizeVector(difference);

}

void Wolf::WolfDie()                               // Function that deletes wolves from the simulation
{
	
	for (int a = 0; a < wolf.size(); a++)
	{
		if (position.x == wolf[a].position.x)
		{
			wolf.erase(wolf.begin() + a);
			return;
		}
		
	}
}

void Wolf::WolfSense()
{
	if (hp < max_hp && no_breeding_time < lifetime)
	{
		for (int a = 0; a < sheep.size(); a++)

		{

			if (CheckCollisionPointCircle(sheep[a].position, position, sight_radius))                                  // Here it checks if any sheep are inside the sight radius
			{
				if (close_sheep != NULL)
				{
					if (sqrt(pow(position.y - sheep[a].position.y, 2) + pow(position.x - sheep[a].position.x, 2)) <            // Here it finds the closest one that is inside the radius
						sqrt(pow(position.y - close_sheep->position.y, 2) + pow(position.x - close_sheep->position.x, 2)))
						close_sheep = &sheep[a];
				}
				else close_sheep = &sheep[a];
				
				
			}

		}
		
	}
	else close_sheep = NULL;
}

void Wolf::WolfDecide()
{
	if (hp < 0)                                                                                        // If hp is less then 0 then wolf dies
	{
		state = Wolf_State::Dying;
	}
	else if (hp > max_hp && no_breeding_time < lifetime)                                               // If the wolf's hp has reached maximum and it is not in no breed time it will magically create a new wolf
	{
		state = Wolf_State::Reproducing;
	}
	else if (close_sheep != NULL && CheckCollisionPointCircle(close_sheep->position, position, size))  // Wolf eats the sheep if collides with it
	{
		state = Wolf_State::Eating;
	}
	else if (close_sheep != NULL)                                                                      // If there is a sheep in the sight radius wolf will go towards it
	{
		state = Wolf_State::Finding;
	}
	else                                                                                               // If no sheep is inside the radius or if wolf is in no_breeding_time wolf will wander aimlessly
	{
		state = Wolf_State::Wandering;
	}
	
}


void Wolf::WolfAct()
{
	// here the movement occurs. No matter the state wolf is alsways on the move
	position.x += speed.x * speed_modifier;
	position.y += speed.y * speed_modifier;

	switch (state)
	{
	case Wolf_State::Wandering:
		close_sheep = NULL;                 // Setting close_sheep to NULL so that wolf doesnt pick a new target outside of range immideately after eating a sheep
		speed_modifier = 1.f;
		if (lifetime % speed_change_time == 0)               
		{
			speed = GoTo(static_cast <float>(GetRandomValue(level.spawn_border, GetScreenWidth() - level.spawn_border)), static_cast <float>(GetRandomValue(level.spawn_border, GetScreenHeight() - level.spawn_border)), position);
		}
		
		break;
	case Wolf::Wolf_State::Finding:
		speed_modifier = 2.f;                       // Wolf will speed up when chasing its prey

		DrawCircle(static_cast <int>(close_sheep->position.x), static_cast <int>(close_sheep->position.y), close_sheep->size + 5, RED);  // This draws a red circle around chased sheep

		speed = GoTo(close_sheep->position.x, close_sheep->position.y, position);

		break;
	case Wolf::Wolf_State::Eating:
		close_sheep->SheepDie();            // Makes sheep die
		hp += 40; 
		
		for (int a = 0; a < wolf.size(); a++)       // This reset of all wolfs is needed so that pointers won't get broken after array of sheep changes (not the best solution, but one that works nontheless)
		{
			wolf[a].state = Wolf_State::Wandering;
			wolf[a].WolfSense();
		}
		state = Wolf_State::Wandering;

		break;
	case Wolf::Wolf_State::Reproducing:

	{
		int x_position = (GetRandomValue(static_cast <int>(position.x - level.spawn_border), static_cast <int>(position.x + level.spawn_border)));
		// Border control
		if (position.x - level.spawn_border < level.spawn_border) x_position = level.spawn_border;
		else if (position.x + level.spawn_border > GetScreenWidth()) x_position = GetScreenWidth() - level.spawn_border;

		int y_position = (GetRandomValue(static_cast <int>(position.y - level.spawn_border), static_cast <int>(position.y + level.spawn_border)));
		// Border control
		if (position.y - level.spawn_border < level.spawn_border) y_position = level.spawn_border;
		else if (position.y + level.spawn_border > GetScreenHeight()) y_position = GetScreenHeight() - level.spawn_border;

		// Creating new wolf
		Wolf newborn_wolf(static_cast <float>(x_position), static_cast <float>(y_position), level.names[GetRandomValue(0, static_cast <int>(level.names.size() - 1))]);  // There are too many of those being born. Fix!!
		wolves_to_add.push(newborn_wolf);

		// Debug line
		std::cout << name << " gave birth to " << newborn_wolf.name << " at " << level.timer << std::endl;

		// Reducing HP and making it less horny for some time
		hp = 50;
		no_breeding_time = lifetime + 600;


		state = Wolf_State::Wandering;

		break;
	}
		
	case Wolf::Wolf_State::Dying:
		WolfDie();
		break;
	
	}
}

void Sheep::SheepDie()                             // Function that removes a sheep from the simulation
{
	for (int a = 0; a < sheep.size(); a++)
	{
		if (position.x == sheep[a].position.x)
		{
			sheep.erase(sheep.begin() + a);
			return;
		}
	}
}

void Sheep :: SheepSense()
{
	grass_in_sight.clear();
	
	avoid_point = { position.x, position.y };               

	if (hp < max_hp && no_breeding_time < lifetime)
	{
		for (int a = 0; a < grass.size(); a++)                                // Here sheep looks for all the grass in range and puts it into an array
		{
			if (CheckCollisionCircles(position, sight_radius, grass[a].position, grass[a].size))     
			{
				grass_in_sight.push_back(grass[a]);
			}

		}

		// Finding closest grass to eat
		closest_grass_id = 0;
		if (grass_in_sight.size() > 0)
		{
			for (int a = 1; a < grass_in_sight.size(); a++)   
			{
				if (sqrt(pow(position.y - grass_in_sight[a].position.y, 2) + pow(position.x - grass_in_sight[a].position.x, 2)) <
					sqrt(pow(position.y - grass_in_sight[closest_grass_id].position.y, 2) + pow(position.x - grass_in_sight[closest_grass_id].position.x, 2)))
					closest_grass_id = a;

			}
		}
		
		// ------------------------------
	}
	
	
	for (int a = 0; a < wolf.size(); a++)              // This function should find every wolf around and calculate a point to which sheep will run to escape them
	{
		if (CheckCollisionCircles(position, sight_radius, wolf[a].position, wolf[a].size)) 
		{
			avoid_point.x += (avoid_point.x - wolf[a].position.x) * 10;
			avoid_point.y += (avoid_point.y - wolf[a].position.y) * 10;
		}
		
	}



	
}

void Sheep :: SheepDecide()
{
	if (hp < 0 || state == Sheep_State::Dying)                                           // If below 0 hp sheep dies
	{
		state = Sheep_State::Dying;
	}
	else if (avoid_point.x != position.x)                                                // if there are any wolves around (thus avoid point was modified) sheep will try to avoid them
	{
		state = Sheep_State::Avoiding;
	}
	else if (hp >= max_hp && lifetime > no_breeding_time)                                // If sheep reaches max hp it will create another sheep
	{
		state = Sheep_State::Reproducing;
	}
	else if (grass_in_sight.size() > 0 &&                                                // Upon contact with grass sheep will eat it
		CheckCollisionPointCircle({ grass_in_sight[closest_grass_id].position.x, grass_in_sight[closest_grass_id].position.y }, position, size))
	{
		state = Sheep_State::Eating;
		
	}
	else if (grass_in_sight.size() > 0)                                                  // If there are some juicy grass in range sheep will go towards it to eat it
	{
		state = Sheep_State::Finding;
	}
	else                                                                                 // If there are no wolves and no grass around, sheep will wander aimlessly
	{
		state = Sheep_State::Wandering;
	}
	
}


void Sheep :: SheepAct()
{

	// Movement
	position.x += speed.x * speed_modifier;
	position.y += speed.y * speed_modifier;

	

	switch (state)
	{
	case Sheep_State::Reproducing:
	{

			int x_position = (GetRandomValue(static_cast <int>(position.x - level.spawn_border), static_cast <int>(position.x + level.spawn_border)));
			// Border control
			if (position.x - level.spawn_border < level.spawn_border) x_position = level.spawn_border;
			else if (position.x + level.spawn_border > GetScreenWidth()) x_position = GetScreenWidth() - level.spawn_border;

			int y_position = (GetRandomValue(static_cast <int>(position.y - level.spawn_border), static_cast <int>(position.y + level.spawn_border)));
			// Border control
			if (position.y - level.spawn_border < level.spawn_border) y_position = level.spawn_border;
			else if (position.y + level.spawn_border > GetScreenHeight()) y_position = GetScreenHeight() - level.spawn_border;

			// Creating new sheep
			Sheep newborn_sheep(static_cast <float>(x_position), static_cast <float>(y_position), level.names[GetRandomValue(0, static_cast <int>(level.names.size() - 1))]); 
			sheeps_to_add.push(newborn_sheep);

			std::cout << name << " gave birth to " << newborn_sheep.name << " at " << level.timer << std::endl;

			// Reducing HP and making it less horny for some time
			hp = 50;
			no_breeding_time = lifetime + 600;

		
		state = Sheep_State::Wandering;
		break;
	}
	case Sheep_State::Eating:
	{
		grass_in_sight[closest_grass_id].GrassDie();
		hp += 5;
		break;
	}
	case Sheep_State::Avoiding:
	{
		speed_modifier = 1.5f;                                        // Sheep speeds up when avoiding predators

		speed = GoTo(avoid_point.x, avoid_point.y, position);
		
		break;
	}
		
	case Sheep_State::Finding:
		
	{
		speed_modifier = 1.f;

		// Marking red grass that sheeps wants to eat
		DrawCircle(static_cast <int>(grass_in_sight[closest_grass_id].position.x), static_cast <int>(grass_in_sight[closest_grass_id].position.y), grass_in_sight[closest_grass_id].size + 5, RED);

		speed = GoTo(grass_in_sight[closest_grass_id].position.x, grass_in_sight[closest_grass_id].position.y, position);

		break;
	}
		

	case Sheep_State::Wandering:
		
		speed_modifier = 1.f;
		if (lifetime % speed_change_time == 0)  // changing its wandering trajectory
		{
			speed = GoTo(static_cast <float>(GetRandomValue(level.spawn_border, GetScreenWidth() - level.spawn_border)), static_cast <float>(GetRandomValue(level.spawn_border, GetScreenHeight() - level.spawn_border)), position);
		}

		break;
	case Sheep_State::Dying:

		SheepDie();

		break;
	}
}


void AnimalLogic()
{
	// Here is general logic for sheep and wolves, their sense/decide and act cycles

	for (int a = 0; a < wolf.size(); a++)
	{
		wolf[a].lifetime++;

		if (wolf[a].lifetime % 5 == 0)
		{
			wolf[a].WolfSense();
			wolf[a].WolfDecide();
			if (wolf[a].lifetime % wolf[a].age_rate == 0) wolf[a].hp--;
		}

		if (wolf[a].position.x <= 0 + wolf[a].size || wolf[a].position.x >= GetScreenWidth() - wolf[a].size) wolf[a].speed.x *= -1;
		if (wolf[a].position.y <= 0 + wolf[a].size || wolf[a].position.y >= GetScreenHeight() - wolf[a].size) wolf[a].speed.y *= -1;

		wolf[a].WolfAct();

	}
	for (int a = 0; a < sheep.size(); a++)
	{
		sheep[a].lifetime++;

		if (sheep[a].lifetime % 5 == 0)
		{
			sheep[a].SheepSense();
			sheep[a].SheepDecide(); 
			if (sheep[a].lifetime % sheep[a].age_rate == 0) sheep[a].hp--;  // Aging
		}		

		// collision with borders
		if (sheep[a].position.x <= 0 + sheep[a].size || sheep[a].position.x >= GetScreenWidth() - sheep[a].size) sheep[a].speed.x *= -1;
		if (sheep[a].position.y <= 0 + sheep[a].size || sheep[a].position.y >= GetScreenHeight() - sheep[a].size) sheep[a].speed.y *= -1;

		sheep[a].SheepAct();
	}

}

void DrawSheep()
{
	for (int a = 0; a < sheep.size(); a++)
	{
		// Draws the actual sheep
		DrawCircle(static_cast <int>(sheep[a].position.x), static_cast <int>(sheep[a].position.y), sheep[a].size, { 255, 255, 255, 255 });

		// Draws hp
		if (level.show_hp)
		{
			string hp_bar = to_string(sheep[a].hp); //hp
			DrawText(hp_bar.c_str(), static_cast<int> (sheep[a].position.x) - 8, static_cast<int>(sheep[a].position.y) - 8, 20, BLACK);

			DrawText(sheep[a].name.c_str(), static_cast<int> (sheep[a].position.x) - 25, static_cast<int>(sheep[a].position.y) - 40, 20, BLACK);
		}
	}

	// Draws the sight radius
	if (level.show_sight_radiuses)
	{
		for (int a = 0; a < sheep.size(); a++)
		{
			DrawCircle(static_cast <int>(sheep[a].position.x), static_cast <int>(sheep[a].position.y), sheep[a].sight_radius, {255, 255, 255, 40});
		}
	}
}


void DrawWolves()
{
	// Draws the sight radius
	if (level.show_sight_radiuses)
	{
		for (int a = 0; a < wolf.size(); a++)
		{
			DrawCircle(static_cast <int>(wolf[a].position.x), static_cast <int>(wolf[a].position.y), wolf[a].sight_radius, { 255, 0, 0, 40 });
		}
	}
	
	for (int a = 0; a < wolf.size(); a++)
	{
		// Draws the wolf
		DrawCircle(static_cast <int>(wolf[a].position.x), static_cast <int>(wolf[a].position.y), wolf[a].size, { 40, 40, 40, 255 });

		//Hp
		if (level.show_hp)
		{
			string hp_bar = to_string(wolf[a].hp); //hp
			DrawText(hp_bar.c_str(), static_cast<int> (wolf[a].position.x) - 8, static_cast<int>(wolf[a].position.y) - 8, 20, WHITE);

			DrawText(wolf[a].name.c_str(), static_cast<int> (wolf[a].position.x) - 25, static_cast<int>(wolf[a].position.y) - 40, 20, BLACK);
		}
	}

	
}