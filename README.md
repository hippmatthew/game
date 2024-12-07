You are the cursor. Use WASD to move and E to attack.
The frame advances each time you take a step or attack with your weapon
An enemy has a chance to spawn at a random position on the map each time you advance the frame

Weapons:

Step on an A or a B to pick up the respective weapon. Each weapon has a cetain amount of frames you must wait before being able to attack again called a frame cooldown (FCD)

- B: Basic Attack
  
Attacks the first space in front of the cursor in the direction you were last moving in. Elden Ring mode if you choose this weapon. FCD: 2

- A: AOE Attack

Attacks 1 space in each direction around the user. FCD: 6

Goal:

Survive as long as possible and get the highest score. You can only take 4 hits

Map Creator:

Create maps by adding a .map file to the data folder, changing which map is loaded in src/main.c file, and then recompiling.

Components for Map Building:
- *: wall
- B: basic attack
- A: aoe attack
- S: player spawn
- Y: initial enemies (not recommended as the greedy pathfinding algorithm is difficult enough to beat)

The map must be 32x32 and be made up of the above components
