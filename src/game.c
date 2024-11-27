#include "include/game.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

void initGame() {
  struct termios terminal;

  (void)tcgetattr(STDIN_FILENO, &terminal);
  terminal.c_lflag &= ~(ICANON | ECHO);
  (void)tcsetattr(STDIN_FILENO, TCSANOW, &terminal);

  (void)printf("\033[2J\033[H");
  (void)fflush(stdout);
}

void deinitGame() {
  struct termios terminal;

  (void)tcgetattr(STDIN_FILENO, &terminal);
  terminal.c_lflag  |= (ICANON | ECHO);
  (void)tcsetattr(STDIN_FILENO, TCSANOW, &terminal);

  (void)printf("\033[15;10HGame Over!\033[38;1H\033[2K\n");
}

int loadMap(struct GameState * state, char * path) {
  FILE * mapFile = fopen(path, "r");
  if (mapFile == NULL) return 0;

  for (int i = 0; i < 32; ++i) {
    for (int j = 0; j < 32; ++j) {
      char c = fgetc(mapFile);

      if (c == EOF) return 0;
      if (c == '\n') continue;
      if (c == 'S') {
        state->position.x = j;
        state->position.y = i;
        c = ' ';
      }
      if (c == 'Y') {
        state->enemies[state->enemyCount].x = j;
        state->enemies[state->enemyCount].y = i;
        ++state->enemyCount;
      }

      state->map[j][i] = c;
      (void)printf("%c", c);
    }
    (void)fgetc(mapFile);
    (void)printf("\n");
  }

  (void)fclose(mapFile);

  return 1;
}

void pollEvents(struct GameState * state) {
  char key = getchar();

  if      (key == 'w')  reposition(state, 0);
  else if (key == 'a')  reposition(state, 1);
  else if (key == 's')  reposition(state, 2);
  else if (key == 'd')  reposition(state, 3);
  else if (key == 'e')  attack(state);
  else if (key == 'q')  state->isRunning = 0;
}

void reposition(struct GameState * state, int direction) {
  int x = state->position.x;
  int y = state->position.y;

  if      (direction == 0) y - 1 < 0  ? y = 0  : --y;
  else if (direction == 1) x - 1 < 0  ? x = 0  : --x;
  else if (direction == 2) y + 1 > 31 ? y = 31 : ++y;
  else if (direction == 3) x + 1 > 31 ? x = 31 : ++x;

  if (state->map[x][y] == '*') return;
  else if (state->map[x][y] == 'B') {
    (void)printf("\033[%d;%dH", y + 1, x + 1);

    dropWeapon(state, x, y);
    updateWeaponUI(1);

    state->weapon = 1;
  }
  else if (state->map[x][y] == 'A') {
    (void)printf("\033[%d;%dH", y + 1, x + 1);

    dropWeapon(state, x, y);
    updateWeaponUI(2);

    state->weapon = 2;
  }

  state->position.x = x;
  state->position.y = y;
  state->lookDirection = direction;
}

void attack(struct GameState * state) {
  if (!state->weapon || state->isAttacking ) return;

  state->attackPosition = state->position;
  state->isAttacking = 1;

  if (state->weapon == 1) basicAttack(state);
  else if (state->weapon == 2) aoeAttack(state);
}

void basicAttack(struct GameState * state) {
  int x = state->position.x;
  int y = state->position.y;

  if      (state->lookDirection == 0) y - 1 < 0  ? y = 0  : --y;
  else if (state->lookDirection == 1) x - 1 < 0  ? x = 0  : --x;
  else if (state->lookDirection == 2) y + 1 > 31 ? y = 31 : ++y;
  else if (state->lookDirection == 3) x + 1 > 31 ? x = 31 : ++x;

  if (state->map[x][y] == '*') {
    state->isAttacking = 0;
    return;
  }
  if (state->map[x][y] == 'Y') {
    int index = enemyIndex(state, x, y);
    if (index == -1) {
      state->isAttacking = 0;
      return;
    }

    killEnemy(state, index);
    scorePoint(state);
  }

  (void)printf("\033[%d;%dH/", y + 1, x + 1);

  state->attackFrames = 0;
  state->attackFrameFinish = 2;
}

void aoeAttack(struct GameState * state) {
  int x = state->position.x;
  int y = state->position.y;

  for (int i = -1; i <= 1; ++i) {
    for (int j = -1; j <= 1; ++j) {
      if (i == 0 && j == 0) continue;

      (void)printf("\033[%d;%dH", y + i + 1, x + j + 1);

      if (state->map[x + j][y + i] == '*') continue;
      if (state->map[x + j][y + i] == 'Y') {
        int index = enemyIndex(state, x + j, y + i);
        if (index == -1) continue;

        killEnemy(state, index);
        scorePoint(state);
      }

      (void)printf("\033[%d;%dH@", y + i + 1, x + j + 1);
    }
  }

  state->attackFrames = 0;
  state->attackFrameFinish = 6;
}

void finishAttack(struct GameState * state) {
  for (int i = -1; i <= 1; ++i) {
    for (int j = -1; j <= 1; ++j) {
      int x = state->attackPosition.x + j;
      int y = state->attackPosition.y + i;
      (void)printf("\033[%d;%dH%c", y + 1, x + 1, state->map[x][y]);
    }
  }

  state->isAttacking = 0;
}

void drawPlayer(struct GameState * state) {
  (void)printf("\033[%d;%dH", state->position.y + 1, state->position.x + 1);
}

void drawUI(struct GameState * state) {
  (void)printf("\033[34;1H\033[2KHP |");
  for (int i = 0; i < state->health; ++i)
    (void)printf("======");
  (void)printf("|");

  (void)printf("\033[35;1H\033[2KWeapon: None");

  (void)printf("\033[36;1H\033[2KScore: 0");
}

void updateWeaponUI(int weapon) {
  (void)printf("\033[35;1H\033[2KWeapon: ");

  if (!weapon) (void)printf("None");
  else if (weapon == 1) (void)printf("Basic");
  else if (weapon == 2) (void)printf("AOE");
}

void dropWeapon(struct GameState * state, int x, int y) {
  if (!state->weapon) {
    (void)printf(" ");
    state->map[x][y] = ' ';
  }
  else if (state->weapon == 1) {
    (void)printf("B");
    state->map[x][y] = 'B';
  }
  else if (state->weapon == 2) {
    (void)printf("A");
    state->map[x][y] = 'A';
  }
}

void updateEnemies(struct GameState * state, int index) {
  if (index >= state->enemyCount) return;

  struct Position pos = state->enemies[index];

  state->map[pos.x][pos.y] = ' ';
  (void)printf("\033[%d;%dH ", pos.y + 1, pos.x + 1);

  int dx = state->position.x - pos.x;
  int dy = state->position.y - pos.y;
  int triedX = 1;

  int goX = abs(dx) >= abs(dy);

  pos.x += goX * ((dx > 0) - (dx < 0));
  pos.y += !goX * ((dy > 0) - (dy < 0));

  if (state->map[pos.x][pos.y] == '*' || state->map[pos.x][pos.y] == 'Y') {
    pos.x -= goX * ((dx > 0) - (dx < 0));
    pos.y -= !goX * ((dy > 0) - (dy < 0));

    goX = !goX;

    pos.x += goX * ((dx > 0) - (dx < 0));
    pos.y += !goX * ((dy > 0) - (dy < 0));

    if (state->map[pos.x][pos.y] == '*' || state->map[pos.x][pos.y] == 'Y') {
      pos.x -= goX * ((dx > 0) - (dx < 0));
      pos.y -= !goX * ((dy > 0) - (dy < 0));
    }
  }

  if (pos.x == state->position.x && pos.y == state->position.y) {
    killEnemy(state, index);
    takeDamage(state);
    return updateEnemies(state, index + 1);
  }

  state->enemies[index] = pos;
  state->map[pos.x][pos.y] = 'Y';

  (void)printf("\033[%d;%dHY", pos.y + 1, pos.x + 1);

  updateEnemies(state, index + 1);
}

void killEnemy(struct GameState * state, int index) {
  if (index >= state->enemyCount) return;

  --state->enemyCount;
  if (state->enemyCount <= 0) return;

  struct Position pos = state->enemies[index];

  state->map[pos.x][pos.y] = ' ';
  (void)printf("\033[%d;%dH ", pos.y + 1, pos.x + 1);

  for (int i = index; i < state->enemyCount; ++i)
    state->enemies[i] = state->enemies[i + 1];
}

void takeDamage(struct GameState * state) {
  --state->health;

  (void)printf("\033[34;1H\033[2KHP |");
  for (int i = 0; i < state->health; ++i)
    (void)printf("======");
  (void)printf("\033[34;29H|");

  if (state->health == 0) state->isRunning = 0;
}

void spawnEnemy(struct GameState * state) {
  if (state->enemyCount == state->maxEnemies) return;

  int x = rand() % 31;
  int y = rand() % 31;

  while ((x != state->position.x && y != state->position.y) || state->map[x][y] == '*' || state->map[x][y] == 'Y') {
    x = rand() % 31;
    y = rand() % 31;
  }

  state->enemies[state->enemyCount].x = x;
  state->enemies[state->enemyCount].y = y;
  ++state->enemyCount;

  (void)printf("\033[%d;%dHY", y + 1, x + 1);
}

int enemyIndex(struct GameState * state, int x, int y) {
  for (int i = 0; i < state->enemyCount; ++i) {
    if (state->enemies[i].x == x && state->enemies[i].y == y)
      return i;
  }

  return -1;
}

void scorePoint(struct GameState * state) {
  ++state->points;
  (void)printf("\033[36;1H\033[2KScore: %d", state->points);
}