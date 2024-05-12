#include "entities.h"
#include "raylib.h"
#include <string>
#include <vector>
#include <time.h>

//  Creating player (starship) and manager (game) variables

Player starship;
Manager manager;
//Sound shot_sound;

void InitPlayer()
{ 

	//Beggining position of the starship

	starship.Position.y = 675;
	starship.Position.x = 350;
}

void LoadAssets()
{
	//Loading all the textures and sounds

	manager.rock_texture = LoadTexture("./assets/rocks.png"); //Absolute path
	manager.explosion_texture = LoadTexture("./assets/explz_cor.png");
	manager.starship_texture = LoadTexture("./assets/spaceship.png");
	manager.lasercharge_texture = LoadTexture("./assets/charge.png");
	manager.bullet_texture = LoadTexture("./assets/loooop.png");

	manager.shot_sound = LoadSound("./assets/shot.wav");
	manager.death_sound = LoadSound("./assets/lose.wav");
	manager.explosion_sound = LoadSound("./assets/meteor_explosion.wav");
	manager.gem_sound = LoadSound("./assets/gem_collect.wav");
}

void UnloadAssets()
{
	UnloadTexture(manager.rock_texture);
	UnloadTexture(manager.explosion_texture);
	UnloadTexture(manager.starship_texture);
	UnloadTexture(manager.lasercharge_texture);

	UnloadSound(manager.shot_sound);
	UnloadSound(manager.death_sound);
	UnloadSound(manager.explosion_sound);
	UnloadSound(manager.gem_sound);
}

void RenderPlayer()
{

	// Blinking of the starship's texture that indicates temporary invulnerability

	starship.blinking = (starship.timer >= 0 && starship.timer < 20 || starship.timer >= 40 && starship.timer < 60 || starship.timer >= 80 && starship.timer < 100 || 
		starship.timer >= 120 && starship.timer < 140 || starship.timer >= 160 && starship.timer < 180 || starship.timer >= 200) ? true : false;                    

	// DRAWING THE STARSHIP

	if (starship.start && starship.blinking)
	{
		DrawTexture(manager.starship_texture, starship.Position.x - 72, starship.Position.y - 50, WHITE);
	}
}

Entity bullet; // We need only one bullet entity as there can't be multiple bullet entities on the screen at the same time

void UpdatePlayer()
{

	// STARTING (NOT ONLY) SCREEN WITH HIGHSCORE

	if (starship.start == false)
	{
		DrawText("Shoot rock", 110, 100, 80, ORANGE);
		DrawText("Press x to start", 190, 545, 40, LIGHTGRAY);


		if (starship.score > 0)
		{
			std::string scr = std::to_string(starship.score);
			DrawText(scr.c_str(), 450, 260, 40, YELLOW);
			DrawText("Your score: ", 190, 260, 40, LIGHTGRAY);
		}

		DrawText("HighScore: ", 190, 320, 40, LIGHTGRAY);
		std::string highscr = std::to_string(manager.highscore);
		DrawText(highscr.c_str(), 450, 320, 40, YELLOW);
		if (manager.new_highscore) DrawText("NEW HIGHSCORE!", 190, 365, 30, YELLOW);


		
		InitPlayer();
		starship.timer = 0;
		starship.invisibility = true;
		
		if (IsKeyPressed(KEY_X))
		{
			manager.new_highscore = false;
			starship.start = true;
			starship.score = 0;
		}
			
	}
	else
	{
		
		starship.timer++;           
		
		if (starship.timer == 200)        // Starship is invulnerable for the first 200 frames it exists 
		{
			starship.invisibility = false;
		}
		// 
		//   ---------------CONTROLS--------------------
		
		//         MOVING

		if (IsKeyDown(KEY_LEFT) && starship.Position.x > 35)
		{
			starship.Position.x -= 4;
		}

		if (IsKeyDown(KEY_RIGHT) && starship.Position.x < 670)
		{
			starship.Position.x += 4;
		}

		//       CHARGING LASER

		if (IsKeyDown(KEY_Z))
		{
			
			// Animating the laser charge ------------------------------*
			
			if (starship.timer % 2 == 0)
			{
				if (starship.laser_x < 1200) starship.laser_x += 40;
				else starship.laser_x = 1120;
			}

			// ---------------------------------------------------------*


			if (starship.laser_charge < 76) starship.laser_charge++;
			if (starship.laser_charge == 75)
			{
				
				starship.ready_to_shoot = true;
			}// It takes 1,25 sec to charge a shot
			DrawTextureRec(manager.lasercharge_texture, { starship.laser_x,  0, 40, 60 }, { starship.Position.x - 20, starship.Position.y - 70 }, WHITE);
		}

		//       RELEASING

		else {
				
			starship.laser_charge = 0;
			starship.laser_x = 0;
			if (starship.ready_to_shoot)
			{
				//SHOOTING
				
				PlaySoundMulti(manager.shot_sound);
				
				bullet.Position.x = starship.Position.x;
				bullet.Position.y = starship.Position.y - 15;

			}
			starship.ready_to_shoot = false;
		}
	}
	

	// ----------------------------------------------

	// BULLET RENDER
	
	if (bullet.Position.y > -70) bullet.Position.y = bullet.Position.y - 25;  // bullet speed

	if (starship.start)
	{
		// Animating the bullet -----------------*
		if (starship.timer % 3 == 0)
		{
			if (starship.bullet_x < 360) starship.bullet_x + 60;
			else starship.bullet_x = 0;
		}
		// --------------------------------------*
		DrawTextureRec(manager.bullet_texture, {starship.bullet_x, 0, 60, 60}, { bullet.Position.x - 30, bullet.Position.y - 4 }, WHITE);
		

		// WRITING CURRENT PLAYER'S SCORE (Why exactly here, no idea)

		std::string scr = std::to_string(starship.score);

		DrawText("Score: ", 10, 700, 30, WHITE);
		DrawText(scr.c_str(), 130, 700, 30, YELLOW);
		}
			
}

std::vector<Rock> rocks;
std::vector<Gem> gems;
std::vector<Explosion> booms;

void New_explosion(int x, int y)         // Constructor function for explosions 
{
	Explosion boom;
	boom.Position.x = x;
	boom.Position.y = y;
	booms.push_back(boom);

	PlaySoundMulti(manager.explosion_sound);
}

void Death()                             // Ends the game and saves a new highscore (if there is one)
{
	New_explosion(starship.Position.x, starship.Position.y);

	starship.start = false;
	starship.ready_to_shoot = false;
	manager.difficulty = 0;
	manager.next_difficulty = 2000;
	if (starship.score > manager.highscore)
	{
		manager.highscore = starship.score;
		manager.new_highscore = true;
	}

	PlaySoundMulti(manager.death_sound);
}

void Difficulty()                         // With every 2000 score points the game becomes a little more difficult (There are more meteors and their trajectory becomes trickier)
{
	if (starship.score > manager.next_difficulty)
	{
		manager.difficulty++;
		manager.next_difficulty += 2000;
	}
}

void Explosion_animation()
{
	for (int a = 0; a < booms.size(); a++)
	{

		// Deleting the explosion after it was finished
		if (booms[a].texture_x >= 1518)
		{
			booms.erase(booms.begin() + a);
			break;
		}
	}

	for (Explosion& boom : booms)

	{
		boom.timer++;
		//boom.Position.y +=6;                   // EXPLOSION PHYSICS (I decided not to have it in the final version, but it can be turned on by uncommenting this line)

		DrawTextureRec(manager.explosion_texture, {boom.texture_x, 0, 123,130 }, { boom.Position.x - 60, boom.Position.y - 80}, WHITE);

		// ANIMATING THE EXPLOSION ----------------------------*

		if (boom.timer % 4 == 0)
		{
			boom.timer = 0;
			boom.texture_x += 123;
			
		}

		// ---------------------------------------------------*
		
	}
	
}

void RockUpdate() {
	int rock_trigger = GetRandomValue(0, 22 - manager.difficulty);  // In the beginning there is a 1/22 chance of spawning a new meteor every frame
	
	

	//SPAWNING ROCKS    

	if (rock_trigger == 0 && starship.start)
	{

		Rock new_rock;
		new_rock.Position.x = GetRandomValue(0, 700);
		new_rock.Position.y = -50;

		new_rock.texture = manager.rock_texture;
		
		new_rock.type = GetRandomValue(0, 3);       // There are 4 different textures for meteors that are being chosen on random when the meteor is created

		new_rock.x_speed = (new_rock.Position.x > 350) ? GetRandomValue(0, -4) : GetRandomValue(0, 4);      // I cheat here and made that meteors spawned on the right side always go a little left and vice versa. That way there won't be useless meteors that just go off the screen right after being spawned
		new_rock.y_speed = (new_rock.x_speed > 0) ? 9 - new_rock.x_speed / 10 : 9 + new_rock.x_speed / 10;


		rocks.push_back(new_rock);
	}

	//RENDERING ROCKS

	for (Rock& rock : rocks)
	{
		

		if (starship.timer % 70 == 0) rock.Random_movement = GetRandomValue(-10 - manager.difficulty * 3, 10 + manager.difficulty * 3);
		rock.Position.y += rock.y_speed;
		rock.Position.x += rock.x_speed + rock.Random_movement / 10;

		rock.rotation = rock.rotation + 1;
		
		switch (rock.type)
		{
		case 0:
			DrawTexturePro(rock.texture, { 120, 90, 70, 90 }, { rock.Position.x, rock.Position.y, 70, 90 }, { 35, 45 }, rock.rotation, WHITE);

			break;
		case 1:
			DrawTexturePro(rock.texture, { 120, 8, 70, 90 }, { rock.Position.x, rock.Position.y, 70, 90 }, { 35, 45 }, rock.rotation, WHITE);

			break;
		case 2:
			DrawTexturePro(rock.texture, { 205, 20, 50, 70 }, { rock.Position.x, rock.Position.y, 70, 90 }, { 35, 45 }, rock.rotation, WHITE);

			break;
		case 3:
			DrawTexturePro(rock.texture, { 205, 110, 50, 70 }, { rock.Position.x, rock.Position.y, 70, 90 }, { 35, 45 }, rock.rotation, WHITE);
			break;
		}
		
		//DrawCircle(rock.Position.x, rock.Position.y, 18, BROWN);  <---- debug thing for finding actul meteor position
	}

	// DELETING ROCKS

	for (int a = 0; a < rocks.size(); a++)
	{
		// CLEANING MISSED ROCKS

		if (rocks[a].Position.y > 780)
		{
			rocks.erase(rocks.begin() + a);
			break;
		}

		// COLLISION WITH BULLETS

		if (bullet.Position.x >= rocks[a].Position.x - 30 && bullet.Position.x < rocks[a].Position.x + 30
			&& bullet.Position.y + 8 >= rocks[a].Position.y - 25 && bullet.Position.y + 8 < rocks[a].Position.y + 25)
		{
			
			//UnloadTexture(rocks[a].texture);

			New_explosion(rocks[a].Position.x, rocks[a].Position.y);

			for (int b = GetRandomValue(3, 5); b > 0; b--)
			{
				Gem new_gem;
				new_gem.Position.x = rocks[a].Position.x + GetRandomValue(-40, 40);
				new_gem.Position.y = rocks[a].Position.y + GetRandomValue(-40, 40);
				new_gem.x_speed = GetRandomValue(-10, 10);
				new_gem.speed = GetRandomValue(5, 6);
				gems.push_back(new_gem);
			}
			
			rocks.erase(rocks.begin() + a);

			break;
		}

		// COLLISION WITH THE STARSHIP

		if (starship.Position.x >= rocks[a].Position.x - 25 && starship.Position.x < rocks[a].Position.x + 25
			&& starship.Position.y >= rocks[a].Position.y - 10 && starship.Position.y < rocks[a].Position.y + 10 && starship.invisibility == false)  // The hitbox of the starship is slightly smaller than it's texture. That way the game feels less frustrating at times
		{
			rocks.erase(rocks.begin() + a);
			
			Death();
			
			break;
		}
	}
}

void UpdateGems()
{
	
	for (int a = 0; a < gems.size(); a++)
	{
		if (gems[a].collected)
		{
			//Depicting how many points the player gets by collecting a gem
			if (gems[a].time > 0)
			{
				gems[a].time--;
				std::string gained_score = std::to_string(gems[a].score);

				DrawText(gained_score.c_str(), gems[a].Position.x, gems[a].Position.y - 30 - 25 * gems[a].collected_at_the_moment, 20, RED);
				
			}
			else {
				gems.erase(gems.begin() + a);
				break;
			}
			
		}
		else {
			

			gems[a].Position.y += gems[a].speed;
			gems[a].Position.x += gems[a].x_speed / 10;

			DrawTextureRec(manager.rock_texture, {300, 0, 60, 90}, { gems[a].Position.x - 10, gems[a].Position.y - 55 }, RED);
			//DrawCircle(gems[a].Position.x, gems[a].Position.y, 4, RED);

			if (starship.Position.x >= gems[a].Position.x - 35 && starship.Position.x < gems[a].Position.x + 35
				&& starship.Position.y >= gems[a].Position.y - 10 && starship.Position.y < gems[a].Position.y + 10 && starship.start == true)
			{
				
				PlaySoundMulti(manager.gem_sound);

				starship.score += manager.score_bonus;
				gems[a].score = manager.score_bonus;
				manager.score_bonus += 10;
				manager.combo_timer = 100;
				
				gems[a].time = 50;

				// Code below makes score points for collecting gems to appear above the previous score points if there are such, so it won't look messy 

				gems[a].collected = true;

				for (Gem& gem : gems)
				{
					if (gem.collected == true) gems[a].collected_at_the_moment++;
				}

				
			}
		}
		
	}

	// COMBO SCORE BOMUS

	if (manager.combo_timer > 0)
	{
		manager.combo_timer--;

		DrawText("Next gem: ", 550, 680, 20, WHITE);
		std::string combo = std::to_string(manager.score_bonus);

		DrawText(combo.c_str(), 660, 680, 20, RED);

		DrawRectangle(550, 705, manager.combo_timer, 20, RED);

	}
	else manager.score_bonus = 50;

}

