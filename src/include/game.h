struct Position {
  int x;
  int y;
};

struct GameState {
  int isRunning;
  int isAttacking;
  int attackFrames;
  int attackFrameFinish;
  struct Position attackPosition;
  struct Position position;
  int lookDirection;
  int weapon;
  char map[32][32];
  struct Position enemies[32];
  int enemyCount;
  int maxEnemies;
  int health;
  int spawnChance;
  int points;
};

void initGame();
void deinitGame();
int loadMap(struct GameState *, char *);
void pollEvents(struct GameState *);
void reposition(struct GameState *, int);
void attack(struct GameState *);
void basicAttack(struct GameState *);
void aoeAttack(struct GameState *);
void finishAttack(struct GameState *);
void drawPlayer(struct GameState *);
void drawUI(struct GameState *);
void updateWeaponUI(int);
void dropWeapon(struct GameState *, int, int);
void updateEnemies(struct GameState *, int);
void killEnemy(struct GameState *, int);
void takeDamage(struct GameState *);
void spawnEnemy(struct GameState *);
int enemyIndex(struct GameState *, int, int);
void scorePoint(struct GameState *);