#include "include/game.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
  srand(time(0));

  struct GameState state;
  state.isRunning = 1;
  state.isAttacking = 0;
  state.enemyCount = 0;
  state.maxEnemies = 32;
  state.spawnChance = 1;
  state.health = 4;
  state.points = 0;

  initGame();

  if (!loadMap(&state, "data/map.map")) {
    (void)printf("failed to load map");
    return 1;
  }

  drawUI(&state);
  drawPlayer(&state);

  while (state.isRunning) {
    pollEvents(&state);

    if (state.isAttacking && ++state.attackFrames == state.attackFrameFinish)
      finishAttack(&state);

    updateEnemies(&state, 0);

    if (rand() % 35 + 1 <= state.spawnChance) {
      spawnEnemy(&state);
      state.spawnChance = state.spawnChance - 10 < 1 ? 1 : state.spawnChance - 10;
    }
    else
      ++state.spawnChance;

    drawPlayer(&state);
  }

  deinitGame();

  return 0;
}