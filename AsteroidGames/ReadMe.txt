Rock shoot 
Done by Igor Sorogovecs

The code was split between 5 files (the same layout me and my team used in our previous project)
1) main.cpp
2) level.h
3) level.cpp    //Basically creates and renders moving stars in the background
4) entities.cpp  //Almost everything is done here
5) entities.h    //Contains all the classes of gameobjects

---< level.cpp >---
void InitStars()- Creates a number of stars with random positions evenly allocated across the window.

void Renderlevel()- Moves the stars down. Whenever a star reaches bottom it reappears at the top og the window.
-------------------

---< entities.cpp >---
void InitPlayer()- Sets the location of the starship to starting position.

void LoadAssets()- Loads all the textuers and sounds into the GPU

void UnloadAsstes()- Unloads all the textures and sounds from GPU

void RenderPlayer()- Draws the texture of the starship on the screen and makes it blink for the first 200 frames. 
Blinking indicates that the starship is temporary invincible. This wasn't present in the original, but I decided to add that, because it seems like a reasonable thing to have.


void UpdatePlayer()- The first part of it (if(starship.start == false)) is actually drawing the title screen. starship.start basically indicates if the x was pressed and the game was started.

The second part of it is starship behavior.
starship.timer counts for how many frames the starship was alive.
The condtrols are also there, they are the same as in the original game, except starship can't go up nor down.

-Animation-
All the animation is done by "travelling right through the spritesheet". When a certain amout of frame pass the sprite is changed to the next one, so the illusion of animation appears.

Whenever a player is holding "Z" key the starship.laser_charge is being upscaled while it is not equal to 76.
if starship.laser_charge is equal to 75 then a shoot can be made

While the player is not holding the "Z" key starship.laser_charge is always being set to 0, but in case if the shoot was previously charged executes the piece of code that correspondes to shooting 

-Shooting-
The is only one bullet entity in the whole game. There is no need to spawn more and have an array of bullets, because there can't be a situation when two bullets are on the screen at the same time.
The bullet is always travelling up the screen crushing every rock on its way and stops when reaches position just above the window.
Shooting basically teleports the bullet to starship's location. Since it is no longer above the screen it rushes there rapidly and we get an illusion of it just being fired.

Closer to the end of this function there is code for bullet rendering and animation.

At the very bottom of the function there is a code that shows the player how my score points they have.


(Between void UpdatePlayer() and void New_explosion() there are some arrays for various gameobject

void New_explosion()- This function is used to initialise new explosions where and when nessecary 

void Death()- This code executes whenever player is hit by rock. It finished current run, resets player position and difficulty (more on that later) as well as rewrites highscore if it was beaten.

void Difficutly()- I decided to add dynamic difficulty that raises every time a player scores 2000, 4000, 6000 and so on. 
Higher difficulty makes more asteroids appear on the screen, as well as making their trajectory trickier (more and more similar to how snake moves)

void Explosion_animation()- Does exactly what it says as well as deletes explosion entity after it finishes.


void RockUpdate()- This is one of the heaviest functions. Here is what it does(mainly rock behavior):

1) If the was started spawns new rocks with a 1/22 chance every frame.
2) It makes the asteroids move down and a little to the side as well as draws their texture.
3) Deletes every rock that moves below the window level.
4) It checks for a collision with the bullet. In case of detected collision spawns a bunch of gems and deletes the asteroid.
5) Checks for a collison with the ship and executes Death() if detected.


void UpdateGems()- mainly gem behavior and score management

1) "if(gems[a].collected)" part is responsible for depicting how many points gets player when picking up a gem.
It deletes the gem entity only after the score number disappears, because the red number and the actual gem is like two forms of the same game object
2) "else" is responsible for the gem while it's flying down. It moves the gem down, draws it's texture, checks for the collison with starship and adds score points if collision is detected. It also resets the combo timer.
3) After for loop there is a part responsible for combo score bonus. It also draws that little combo indicator/bar at the bottom right of the screen when the combo is active.
If combo timer expires it returns given score point value back to default 50.

----------------------