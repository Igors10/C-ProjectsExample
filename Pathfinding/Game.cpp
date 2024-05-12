#pragma once
#include "./Header/Game.h"
#include "raylib.h"
#include <vector>
#include <iostream>
#include <queue>
#include <stack>
#include <cmath>
#include <string.h>
using namespace std;

Level level;

void NewStar()
{
	// ***** Creating Fallen Star ***** (technically just moving it)

	int star_random_x = GetRandomValue(0, level.tile_count_horizontal - 1);
	int star_random_y = GetRandomValue(0, level.tile_count_vertical - 1);

	while (level.tiles[star_random_x][star_random_y].type != Tile::Type::Path)
	{
		star_random_x = GetRandomValue(0, level.tile_count_horizontal - 1);
		star_random_y = GetRandomValue(0, level.tile_count_vertical - 1);
	}

	int star_position_x = level.tiles[star_random_x][star_random_y].position_x + level.tile_size / 2;
	int star_position_y = level.tiles[star_random_x][star_random_y].position_y + level.tile_size / 2;

	level.fallen_star = { float(star_position_x), float(star_position_y) };

	// ---------------------------
}

void InitLevel()
{
	// Calcualting the number of tiles horizontally and vertically from 2d array size

	level.tile_count_horizontal = sizeof(level.tiles) / sizeof(level.tiles[0]);
	level.tile_count_vertical = sizeof(level.tiles) / (sizeof(Tile) * level.tile_count_horizontal); 

	// Setting tile size (the more tiles the smaller they'll be to fit the same space)
	level.tile_size = (GetScreenHeight() - 200) / level.tile_count_vertical; 

	
	// Creating tiles
	for (int a = 0; a < level.tile_count_horizontal; a++)
	{
		for (int b = 0; b < level.tile_count_vertical; b++)
		{
			int x_position = 200 + level.tile_size * a;
			int y_position = 100 + level.tile_size * b;
			Color new_tile_color = ((a + b) % 2 == 0) ? LIGHTGRAY : WHITE;
			Tile new_tile(x_position, y_position, a, b, new_tile_color);
			level.tiles[a][b] = new_tile;
		}
	}

	// Replacing some of the empty tiles with obstacle tiles
	for (int a = 0; a < level.starting_obstacle_count; a++)
	{
		
		while (true) {
			int tile_random_x = GetRandomValue(0, level.tile_count_horizontal - 1);
			int tile_random_y = GetRandomValue(0, level.tile_count_vertical - 1);
			
			if (level.tiles[tile_random_x][tile_random_y].type == Tile::Type::Path)
			{
				level.tiles[tile_random_x][tile_random_y].type = Tile::Type::Obstacle;
				level.tiles[tile_random_x][tile_random_y].color = DARKGRAY;
				std::cout << level.tiles[tile_random_x][tile_random_y].position_x << " " << level.tiles[tile_random_x][tile_random_y].position_y << std::endl;
				break;
				
			}
			
		}
		

	}
	

	// Placing tradepost tile somewhere on the map
	while (true)
	{
		int tile_random_x = GetRandomValue(0, level.tile_count_horizontal - 1);
		int tile_random_y = GetRandomValue(0, level.tile_count_vertical - 1);

		if (level.tiles[tile_random_x][tile_random_y].type == Tile::Type::Path)
		{
			level.tiles[tile_random_x][tile_random_y].type = Tile::Type::Tradepost;
			level.tiles[tile_random_x][tile_random_y].color = BROWN;
			level.tradepost_tile = &level.tiles[tile_random_x][tile_random_y];
			break;
		}
	}

	// Placing starcatcher base tile somewhere on the map
	while (true)
	{
		int tile_random_x = GetRandomValue(0, level.tile_count_horizontal);
		int tile_random_y = GetRandomValue(0, level.tile_count_vertical);

		if (level.tiles[tile_random_x][tile_random_y].type == Tile::Type::Path)
		{
			level.tiles[tile_random_x][tile_random_y].type = Tile::Type::Base;
			level.tiles[tile_random_x][tile_random_y].color = DARKBLUE;
			level.base_tile = &level.tiles[tile_random_x][tile_random_y];
			break;
		}
	}

	// ***** Initiate Player *****

	int agent_random_x = GetRandomValue(0, level.tile_count_horizontal - 1);
	int agent_random_y = GetRandomValue(0, level.tile_count_vertical - 1);
	
	while (level.tiles[agent_random_x][agent_random_y].type != Tile::Type::Path)
	{
		agent_random_x = GetRandomValue(0, level.tile_count_horizontal - 1);
		agent_random_y = GetRandomValue(0, level.tile_count_vertical - 1);
	}

	int agent_position_x = level.tiles[agent_random_x][agent_random_y].position_x + level.tile_size / 2;
	int agent_position_y = level.tiles[agent_random_x][agent_random_y].position_y + level.tile_size / 2;


	Agent Agent(float(agent_position_x), float(agent_position_y), agent_random_x, agent_random_y);

	level.player = Agent;
	 
	level.player.size = level.tile_size * 0.35f;

	// ---------------------------

	NewStar();

}


void AddNode(Vector2 position)  // Adding new node, used for pathfinding
{
	Node node(position);
	level.all_nodes.push_back(node);
	level.open_nodes.push_back(int(level.all_nodes.size()) - 1);
}

int GetX(float x)  // Getting X coordinate from a position on the screen
{
	for (int a = 1; a <= level.tile_count_vertical; a++)
	{
		if (x - 200 - a * level.tile_size < 0) return a - 1;
	}
	return -1;
}

int GetY(float y)  // Getting Y coordinate from a position on the screen
{
	for (int a = 1; a <= level.tile_count_horizontal; a++)
	{
		if (y - 100 - a * level.tile_size < 0) return a - 1;
	}
	return -1;
}

int GetVectorLength(int x, int y)
{
	return int(sqrt(x * x + y * y));
}


Tile& GetTile(int x, int y)
{
	return level.tiles[x][y];
}


void SetNode(Node& node, int parent_g_value, Vector2 parent_pos)  // Calculating node's f (total) and g (step) values 
{
	if (node.position.x == level.target_position.x && node.position.y == level.target_position.y) level.star_found = true;

	int total_value = 0;
	int step_value = 0;
	int distance_value = 0;
	
	step_value = GetVectorLength(int(node.position.x) - int(parent_pos.x), int(node.position.y) - int(parent_pos.y)) + parent_g_value; // + parent_tile->step_distance
	
	node.g_value = step_value;

	distance_value = abs(GetVectorLength(int(node.position.x) - int(level.fallen_star.x), int(node.position.y) - int(level.fallen_star.y)));

	total_value = step_value + distance_value;
	//total_value = step_value;
	//total_value = distance_value;

	node.f_value = total_value;

	return;

}

void CheckAroundNode(Node& node_copy) // checks every tile around given nodes and sets nodes on empty yet not checked tiles
{
	Node node = node_copy;
	
	for (int a = -1; a < 2; a++)
	{
		for (int b = -1; b < 2; b++)
		{
			//  *** Checking if a node can be placed on given coordinates ***

			bool skip_tile = false;

			if (level.tile_count_horizontal <= GetX(node.position.x) + a || level.tile_count_vertical <= GetY(node.position.y) + b 
				|| GetY(node.position.y) + b < 0 || GetX(node.position.x) + a < 0 || (a == 0 && b == 0)) continue;
			
			if (GetTile(GetX(node.position.x) + a, GetY(node.position.y) + b).type == Tile::Type::Obstacle) continue;

			for (int c = 0; c < level.all_nodes.size(); c++)
			{
				if (GetX(level.all_nodes[c].position.x) == GetX(node.position.x) + a
					&& GetY(level.all_nodes[c].position.y) == GetY(node.position.y) + b)
				{
					skip_tile = true;
					continue;
				}
			}
			
			if (skip_tile) continue;

			// --------------------------------------------------
			// If it gets to this place in code, it places a node

			AddNode({ float(GetTile(GetX(node.position.x) + a, GetY(node.position.y) + b).position_x + level.tile_size / 2),
				      float(GetTile(GetX(node.position.x) + a, GetY(node.position.y) + b).position_y + level.tile_size / 2) });

			level.all_nodes[level.open_nodes[level.open_nodes.size() - 1]].parent_position = node.position;

			SetNode(level.all_nodes[level.open_nodes[level.open_nodes.size() - 1]], node.g_value, 
				level.all_nodes[level.open_nodes[level.open_nodes.size() - 1]].parent_position); 

			if (level.star_found) return; // if the goal is reached node placing part of the pathfinding ends
			
		}
	}
	
	return;


}


Node& GetNextNodeToCheck() // finds next node with the lowest f value (closest to the point of destination)
{

	int min_f_value_node_id = 0;
	int open_node_id = level.open_nodes[0];

	for (int a = 1; a < level.open_nodes.size(); a++)
	{
		if (level.all_nodes[level.open_nodes[a]].f_value < level.all_nodes[open_node_id].f_value && level.all_nodes[level.open_nodes[a]].f_value != 0)
		{
			open_node_id = level.open_nodes[a];
			min_f_value_node_id = a;
		}
	}

	level.open_nodes.erase(level.open_nodes.begin() + min_f_value_node_id);

	return level.all_nodes[open_node_id];
	
}


Vector2 NormalizeVector(Vector2 vector)
{
	double length = sqrt(pow(vector.x, 2) + pow(vector.y, 2));
	vector.x = vector.x / static_cast <float>(length);
	vector.y = vector.y / static_cast <float>(length);

	return vector;
}

Vector2 PlayerGoTo(Vector2 target_position) // Makes starcatcher move towards chosen position
{
	float new_x_speed = target_position.x - level.player.position_x;
	float new_y_speed = target_position.y - level.player.position_y;


	Vector2 new_speed = NormalizeVector({ new_x_speed, new_y_speed });

	std::cout << "new player speed is " << new_speed.x << ", " << new_speed.y << std::endl;

	return new_speed;
}

Node* GetNode(Vector2 pos)  // Returns a node with input position
{
	for (int a = 0; a < level.all_nodes.size(); a++)
	{
		if (GetX(level.all_nodes[a].position.x) == GetX(pos.x) && GetY(level.all_nodes[a].position.y) == GetY(pos.y)) return &level.all_nodes[a];
	}

	return &level.all_nodes[0];
}

void BuildingThePath()  // When A* found the destination point this function creates a path of waypoints for the starcatcher to move between
{
	std::stack<Node*> path{ };
	Node* tile_to_go = &level.all_nodes[int(level.all_nodes.size()) - 1];
	if (tile_to_go->parent_position.x != 0 && tile_to_go->parent_position.y != 0) std::cout << "target position is " << GetX(level.target_position.x) << ", " << GetY(level.target_position.y) << endl;

	std::cout << "Final path is: " << std::endl;
	while (tile_to_go->parent_position.x != 0 && tile_to_go->parent_position.y != 0)
	{
		path.push(tile_to_go);
		tile_to_go = GetNode(tile_to_go->parent_position);
		
	}

	while (path.size() > 0)
	{
		Vector2 new_waypoint = { float(path.top()->position.x), float(path.top()->position.y) };
		level.waypoint.push_back(new_waypoint);
		path.pop();
		
	}

	level.star_found = false;

	level.player.speed = PlayerGoTo(level.waypoint.front());

	level.all_nodes.clear();
	level.open_nodes.clear();

}


void ClearPath()  // Cancels current path that starcather is following
{
	// Centralising starcatcher in a tile
	level.player.position_x = float(level.tiles[GetX(level.player.position_x)][GetY(level.player.position_y)].position_x) + level.tile_size / 2;
	level.player.position_y = float(level.tiles[GetX(level.player.position_x)][GetY(level.player.position_y)].position_y) + level.tile_size / 2;

	level.waypoint.clear();
	level.all_nodes.clear();
	level.open_nodes.clear();
}

void TargetReached();

void Pathfinding(Vector2 target)  // Begins A* algorithm to find closest path to input position
{
	bool reachable = true;

	ClearPath();
	
	level.target_position = target;
	
	// Checks if the position is reached
	if (target.x == level.player.position_x && target.y == level.player.position_y) TargetReached();

	// Creates first node on a tile with starcatcher
	AddNode({ level.player.position_x, level.player.position_y});

	// While goal is not reached checks open node with minimal f value
	while (!level.star_found)
	{
		// if there are no open nodes left and goal is still not reached, A* stops
		if (level.open_nodes.size() < 1)
		{
			level.all_nodes.clear();
			level.open_nodes.clear();
			level.player.speed = { 0, 0 };
			reachable = false;
			break;
		}

		// Checking next node
		CheckAroundNode(GetNextNodeToCheck());
	}

	std::cout << "All the checked tiles are: " << std::endl;
	std::cout << "========================" << std::endl;
	for (int a = 0; a < level.all_nodes.size(); a++)
	{
		std::cout << "[" << GetX(level.all_nodes[a].position.x) << "] [" << GetY(level.all_nodes[a].position.y) << "]" << std::endl;
		if (level.all_nodes[a].parent_position.x == 0 && level.all_nodes[a].parent_position.y == 0) std::cout << "I dont have a parent" << std::endl;
	}
	std::cout << "========================" << std::endl;

	// Calls a function to create a waypoint path between starcatcher and destiantion tile (if there is an uninterrupted path)
	if (reachable) BuildingThePath();
}


void TargetReached() // Switches states upon reaching target destination
{
	ClearPath();

	switch (level.player.state)
	{
	case Agent::State::FindingStar:  // Upon collecting the star, switches towards finding tradepost to sell it
	{
		Pathfinding({ float(level.tradepost_tile->position_x) + level.tile_size / 2, float(level.tradepost_tile->position_y) + level.tile_size / 2 });
		level.player.state = Agent::State::FindingTrade;
		break;
	}
	case Agent::State::FindingBase:  // Upon reaching base, resets stamina and switches towards getting the star
	{
		level.player.stamina = level.player.max_stamina;

		Pathfinding(level.fallen_star);

		level.player.state = Agent::State::FindingStar;
		break;
	}
	case Agent::State::FindingTrade: // Upon reaching base, resets stamina and switches towards getting the star
	{
		level.player.stars_collected++;

		NewStar();

		Pathfinding(level.fallen_star);
		level.player.state = Agent::State::FindingStar;
		
		break;
	}
	case Agent::State::Wander:  // Upon reaching the place where player commanded starcatcher to go, swiches to find star again
	{
		Pathfinding(level.fallen_star);
		level.player.state = Agent::State::FindingStar;
		break;
	}

	}

}

void RethinkPath()  // Recalculates path to the same destination (used when player modifies the environment)
{
	ClearPath();

	switch (level.player.state)
	{
	case Agent::State::FindingBase:
		Pathfinding({ float(level.base_tile->position_x) + level.tile_size / 2, float(level.base_tile->position_y) + level.tile_size / 2 });
		break;

	case Agent::State::FindingTrade:
		Pathfinding({ float(level.tradepost_tile->position_x) + level.tile_size / 2, float(level.tradepost_tile->position_y) + level.tile_size / 2 });
		break;

	case Agent::State::FindingStar:
		Pathfinding(level.fallen_star);
		break;
	case Agent::State::Wander:
		Pathfinding(level.target_position);
		break;
	}
}

void SwitchTile(Tile* tile)  // Allows player to modify tiles on the map
{
	// If a base tile was selected, this will put a base tile with the following click
	if (level.swap_base && tile->type != Tile::Type::Tradepost)
	{
		tile->color = BLUE;
		tile->type = Tile::Type::Base;
		level.base_tile->color = ((level.base_tile->coordinate_x + level.base_tile->coordinate_y) % 2 == 0) ? LIGHTGRAY : WHITE;
		level.base_tile->type = Tile::Type::Path;
		level.base_tile = tile;
		level.swap_base = false;

		if (level.player.state == Agent::State::FindingBase) RethinkPath();
		
	}

	// If a tradepost tile was selected, this will put a tradepost tile with the following click
	else if (level.swap_tradepost && tile->type != Tile::Type::Base)
	{
		tile->color = BROWN;
		tile->type = Tile::Type::Tradepost;
		level.tradepost_tile->color = ((level.tradepost_tile->coordinate_x + level.tradepost_tile->coordinate_y) % 2 == 0) ? LIGHTGRAY : WHITE;
		level.tradepost_tile->type = Tile::Type::Path;
		level.tradepost_tile = tile;
		level.swap_tradepost = false;

		if (level.player.state == Agent::State::FindingTrade) RethinkPath();
		
	}

	// If no base nor tradepost were selected switches empty tiles for obstacles and vise versa 
	else if (tile->type == Tile::Type::Path)
	{
		tile->color = DARKGRAY;
		tile->type = Tile::Type::Obstacle;

		RethinkPath();
	}
	else if (tile->type == Tile::Type::Obstacle)
	{
		tile->color = ((tile->coordinate_x + tile->coordinate_y) % 2 == 0) ? LIGHTGRAY : WHITE;
		tile->type = Tile::Type::Path;

		RethinkPath();
	}
}


void Input()  // Manages input from the player
{
	if (IsMouseButtonPressed(0)) // Left mouse click
	{
		if (GetMouseX() < 200 || GetMouseX() > 200 + level.tile_size * level.tile_count_horizontal
			|| GetMouseY() < 100 || GetMouseY() > 100 + level.tile_size * level.tile_count_vertical) return;

		level.show_starting_text = false;
		if (IsKeyDown(KEY_LEFT_CONTROL)) // If left ctr is held moves the star to another position
		{
			if (level.tradepost_tile->coordinate_x == GetX(float(GetMouseX())) && level.tradepost_tile->coordinate_y == GetY(float(GetMouseY()))) return;
			level.fallen_star.x = float(GetMouseX());
			level.fallen_star.y = float(GetMouseY());
			level.fallen_star = { float(level.tiles[GetX(level.fallen_star.x)][GetY(level.fallen_star.y)].position_x) + level.tile_size / 2,
				float(level.tiles[GetX(level.fallen_star.x)][GetY(level.fallen_star.y)].position_y) + level.tile_size / 2 };
			if (level.player.state == Agent::State::FindingStar)
			{
				Pathfinding(level.fallen_star);
			}
		}
		else                           // If ctr isnt held commands the starcatcher to move towards clicked tile
		{
			if (level.player.state == Agent::State::FindingTrade)
			{
				level.fallen_star = { float(level.tiles[GetX(level.fallen_star.x)][GetY(level.fallen_star.y)].position_x) + level.tile_size / 2,
				float(level.tiles[GetX(level.fallen_star.x)][GetY(level.fallen_star.y)].position_y) + level.tile_size / 2 };
			}
			level.player.state = Agent::State::Wander;

			float target_position_x = float(GetTile(GetX(float(GetMouseX())), GetY(float(GetMouseY()))).position_x) + level.tile_size / 2;
			float target_position_y = float(GetTile(GetX(float(GetMouseX())), GetY(float(GetMouseY()))).position_y) + level.tile_size / 2;

			level.target_position = { target_position_x , target_position_y };

			Pathfinding(level.target_position);
		}
	}

	if (IsKeyPressed(KEY_P)) // Commands starcatcher to begin looking for stars (basically used to start the game)
	{
		level.show_starting_text = false;
		level.player.state = Agent::State::FindingStar;
		Pathfinding(level.fallen_star);
	}
	

	if (IsKeyPressed(KEY_O)) // Debug thing of getting coordinates of the starcatcher in real time
	{
		std::cout << "Player Coords are [" << GetX(level.player.position_x) << "] [" << GetY(level.player.position_y) << "] " << std::endl;
	}

	if (IsKeyPressed(KEY_S))  // Debug thing of getting coordinates of the star in real time
	{
		std::cout << "Star Coords are [" << GetX(level.player.position_x) << "] [" << GetY(level.player.position_y) << "] " << std::endl;
	}

	if (IsKeyPressed(KEY_SPACE)) level.pause = (level.pause) ? false : true;  // Used to pause the game

	if (IsMouseButtonPressed(1))  // Right click used to switch and move tiles on the map
	{
		for (int a = 0; a < level.tile_count_horizontal; a++)
		{
			for (int b = 0; b < level.tile_count_vertical; b++)
			{
				if (GetMouseX() > level.tiles[b][a].position_x && GetMouseX() < level.tiles[b][a].position_x + level.tile_size 
					&& GetMouseY() > level.tiles[b][a].position_y && GetMouseY() < level.tiles[b][a].position_y + level.tile_size)
				{

					if (level.tiles[b][a].type == Tile::Type::Base)
					{
						level.swap_base = (level.swap_base) ? false : true;
					}
					else if (level.tiles[b][a].type == Tile::Type::Tradepost)
					{
						level.swap_tradepost = (level.swap_tradepost) ? false : true;
					}
					else SwitchTile(&level.tiles[b][a]);
					return;
				}
			}
		}
	}
}

void PlayerLogic()  // Starcatcher functionality
{
	if (level.pause) return; // nothing happens if the game is paused

	// Moving the starcatcher
	level.player.position_x += level.player.speed.x;
	level.player.position_y += level.player.speed.y;

	
	// Guiding the starcatcher towards the next waypoint
	if (level.waypoint.size() > 0)
	{
		if (level.waypoint.front().x <= float(level.player.position_x) + 1.f && level.waypoint.front().x >= float(level.player.position_x) - 1.f && level.waypoint.front().y <= float(level.player.position_y) + 1.f && level.waypoint.front().y >= float(level.player.position_y) - 1.f)
		{
			level.waypoint.erase(level.waypoint.begin());
			if (level.waypoint.size() > 0)               // changes direction towards next waypoint if there are others on the map
			{
				level.player.speed = PlayerGoTo(level.waypoint.front());			
			}
			else                                        //  if upon reaching a waypoint there're no more, stops the starcatcher and reports that target location is reached
			{
				level.player.speed = { 0.f, 0.f };
				TargetReached();
			}
		}
	}
	
	if (level.player.speed.x == 0 && level.player.speed.y == 0) return;


	if (level.player.state == Agent::State::FindingTrade)  // Whenever starcatcher is holding a star
	{
		// Carries the star around
		level.fallen_star.x = level.player.position_x;
		level.fallen_star.y = level.player.position_y - 15;

		// Depletes stamina twice as much when carrying a star
		if (level.player.stamina > 1) level.player.stamina -= 2;
		else
		{
			// Switches to finding a base when stamina is out
			if (level.player.state == Agent::State::FindingTrade)
			{
				level.fallen_star = { float(level.tiles[GetX(level.fallen_star.x)][GetY(level.fallen_star.y)].position_x) + level.tile_size / 2,
				float(level.tiles[GetX(level.fallen_star.x)][GetY(level.fallen_star.y)].position_y) + level.tile_size / 2 };
			}

			Pathfinding({ float(level.base_tile->position_x) + level.tile_size / 2, float(level.base_tile->position_y) + level.tile_size / 2 });

			level.player.state = Agent::State::FindingBase;

		}
	}
	else if (level.player.stamina > 1)
	{
		level.player.stamina--;      // Depletes stamina
	}
	else if (level.player.state != Agent::State::FindingBase)  // Switches to find a base if there are no stamina left
	{
		// drops the star if holding one
		if (level.player.state == Agent::State::FindingTrade)
		{
			level.fallen_star = { float(level.tiles[GetX(level.fallen_star.x)][GetY(level.fallen_star.y)].position_x) + level.tile_size / 2,
			float(level.tiles[GetX(level.fallen_star.x)][GetY(level.fallen_star.y)].position_y) + level.tile_size / 2 };
		}

		Pathfinding({ float(level.base_tile->position_x) + level.tile_size / 2, float(level.base_tile->position_y) + level.tile_size / 2 });

		level.player.state = Agent::State::FindingBase;

	}

}

void DrawPlayer()
{
	// Drawing path connecting all the starcatcher and all the existing waypoints
	if (level.waypoint.size() > 0)
	{
		DrawLineEx({ level.player.position_x, level.player.position_y }, { level.waypoint.front().x, level.waypoint.front().y }, 7.f, GREEN);
		for (int a = 1; a < level.waypoint.size(); a++)
		{
			DrawLineEx(level.waypoint[a], level.waypoint[a - 1], 7.f, GREEN);
		}
	}

	// Drawing starcatcher itself
	DrawCircle(int(level.player.position_x), int(level.player.position_y), level.player.size, SKYBLUE);
}

void DrawText()
{
	// Draws the text above the map (usually something important about the state of the game)
	if (level.show_starting_text) DrawText("Press P to start pathfinding", 85, 50, 30, WHITE);
	else if (level.waypoint.size() < 1) DrawText("PATH IS BLOCKED", 85, 50, 30, WHITE);
	else if (level.pause) DrawText("GAME IS PAUSED, PRESS SPACE TO UNPAUSE", 85, 50, 30, WHITE);
	else if (level.swap_base) DrawText("Right click on tile to move base there", 85, 50, 30, WHITE);
	else if (level.swap_tradepost) DrawText("Right click on tile to move tradepost there", 85, 50, 30, WHITE);

	// Stamina bar
	DrawText("Stamina: ", 850, 50, 30, WHITE);
	DrawRectangle(750, 90, level.player.stamina / 6, 40, DARKGREEN);

	// Number of collected stars
	DrawText("Stars collected: ", 760, 150, 30, WHITE);
	string stars_collected_to_show = to_string(level.player.stars_collected);
	DrawText(stars_collected_to_show.c_str(), 1030, 150, 30, ORANGE);

	// Tutorial text on the right
	DrawText("Click on the map to move starcatcher", 760, 230, 20, WHITE);
	DrawText("Hold Ctr + click to move star around", 760, 260, 20, WHITE);
	DrawText("Right click to change tiles", 760, 290, 20, WHITE);
	DrawText("Press Space to pause the game", 760, 320, 20, WHITE);

	// Draws f value on nodes for debugging (its not visible because in current state this is being deleted in the same frame it is created)
	for (int a = 0; a < level.all_nodes.size(); a++)
	{
		string f_value_to_show = to_string(level.all_nodes[a].f_value);
		DrawText(f_value_to_show.c_str(), (int)level.all_nodes[a].position.x, (int)level.all_nodes[a].position.y, 10, BLACK);
	}
}

void DrawTiles()  // Draw grid with tiles
{
	for (int a = 0; a < level.tile_count_horizontal; a++)
	{
		for (int b = 0; b < level.tile_count_vertical; b++)
		{
			DrawRectangle(level.tiles[a][b].position_x, level.tiles[a][b].position_y, level.tile_size, level.tile_size, level.tiles[a][b].color);
		}
	}

	// Draw Fallen Star

	DrawCircle(int(level.fallen_star.x), int(level.fallen_star.y), 15.f, ORANGE);

	// ---------------
}