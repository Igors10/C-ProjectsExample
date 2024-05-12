#pragma once
#include "raylib.h"
#include <string>
#include <vector>
#include <queue>

struct Node  // Structure used when building a path with A*
{
	Vector2 position;
	int f_value;
	int g_value;
	Vector2 parent_position;
	Node()
	{

	};
	Node(Vector2 pos)
	{
		position = pos;
		f_value = 0;
		g_value = 0;
		parent_position = {0,0};
	};
};

struct Tile  // Structure from which tile grid consists
{
	
	int position_x;
	int position_y;
	int coordinate_x;
	int coordinate_y;
	
	Color color;

	enum class Type {
		Path,
		Obstacle,
		Tradepost,
		Base,
	};

	Type type;

	Tile()
	{

	};
	Tile(int x_pos, int y_pos, int x_coord, int y_coord, Color col)
	{
		position_x = x_pos;
		position_y = y_pos;
		coordinate_x = x_coord;
		coordinate_y = y_coord;
		color = col;
		type = Type::Path;
		
	};
};

struct Agent  // Starcatcher
{
	float position_x;
	float position_y;
	int coordinate_x;
	float size = 10.f;
	int coordinate_y;
	bool holding_star;
	Vector2 speed;
	int stamina;
	int max_stamina;
	int stars_collected;


	enum class State {
		FindingStar, 
		FindingBase,
		FindingTrade,
		Wander // or go towards tile clicked by player
	};

	State state;

	Agent()
	{

	};
	Agent(float x_pos, float y_pos, int x_coord, int y_coord)
	{
		holding_star = false;
		position_x = x_pos;
		position_y = y_pos;
		coordinate_x = x_coord;
		coordinate_y = y_coord;
		state = State::FindingStar;
		speed = { 0.f, 0.f };
		max_stamina = 2000;
		stamina = max_stamina;
		stars_collected = 0;
	};

};

struct Level
{
	Tile tiles[10][10];

	// ---- A* Variables ----

	std::vector<Vector2> waypoint{};
	std::vector<Tile> waytiles{};
	std::vector<Tile> checked_tiles{};
	bool star_found = false;

	// ----------------------

	std::vector<Node> all_nodes{};
	std::vector<int> open_nodes{};

	// ----------------------

	int tile_count_vertical = 1;
	int tile_count_horizontal = 1;
	int map_size = 600;
	int tile_size = 1;
	int starting_obstacle_count = 15;
	Agent player;
	bool pause = false;
	bool swap_base = false;
	bool swap_tradepost = false;
	bool show_starting_text = true;

	
	Tile* base_tile;
	Tile* tradepost_tile;
	Vector2 fallen_star; // theres no struct for the star, its enough for it to be only a vector2
	Vector2 target_position; // Position towards which A* is finding closest path

	Level()
	{

	};
};

void DrawText();
void PlayerLogic();
void Input();
void InitLevel();
void DrawTiles();
void DrawPlayer();