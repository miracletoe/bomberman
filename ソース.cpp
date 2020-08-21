#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<time.h>

#define MAP_WIDTH	 (31)
#define MAP_HEIGHT	 (13)

#define SCREEN_WIDTH  (16)

#define MONSTER_MAX    (9)

#define BOMB_HOLD_MAX  (3)
#define BOMB_COUNT_MAX (11) 

enum {
	DIRECTION_NORTH,
	DIRECTION_WEST,
	DIRECTION_SOUTH,
	DIRECTION_EAST,
	DIRECTION_MAX
};

int directions[][2] = {
	{0,-1},//DIRECTION_NORTH
	{-1,0},//DIRECTION_WEST
	{0,1},//DIRECTION_SOUTH
	{1,0}//DIRECTION_EAST
};

enum {
	CELL_TYPE_NONE,
	CELL_TYPE_HARD,
	CELL_TYPE_SOFT,
	CELL_TYPE_EXPROSION,
	CELL_TYPE_EXIT,
	CELL_TYPE_MAX
};

int cells[MAP_HEIGHT][MAP_WIDTH];

char cellAA[][2 + 1]{
	"　",//CELL_TYPE_NONE
	"■",//CELL_TYPE_HARD
	"□",//CELL_TYPE_SOFT
	"爆",//CELL_TYPE_EXPROSION
	"扉",//CELL_TYPE_EXIT

};

typedef struct {                  //キャラクターの定義
	int x, y;
	int direction;
	bool isDead;
}MONSTER;

MONSTER monsters[MONSTER_MAX];

typedef struct {
	int x, y;
	int count;
}BOMB;


BOMB bombs[BOMB_HOLD_MAX];       //ボムの定義

int exitX, exitY;

int getMonster(int _x, int _y) {
	for (int i = 0; i < MONSTER_MAX;i++)
		if ((!monsters[i].isDead) && (monsters[i].x == _x) && (monsters[i].y == _y))
			return i;
	return -1;
}
void setFreePosition(int* _pX, int* _pY) {
	while (1) {
		int x = rand() % MAP_WIDTH;
		int y = rand() % MAP_HEIGHT;
		switch (cells[y][x]) {
		case CELL_TYPE_HARD:
		case CELL_TYPE_SOFT:

			break;
		default:
		{
			if (x == 2 || y == 2 || x == 3 || y == 3)
				break;
			int monster = getMonster(x, y);
			if (monster < 0) {
				*_pX = x;
				*_pY = y;
				return;
			}
		}
		break;
		}
	}
}

void setRandomSoftPosition(int* _px, int* _py) {
	while (1) {
		int x = rand() % MAP_WIDTH;
		int y = rand() % MAP_HEIGHT;
		if (cells[y][x] == CELL_TYPE_SOFT) {
			*_px = x;
			*_py = y;
			return;
		}
	}
}
int getFreeBomb(void) {
	for (int i = 0;i < BOMB_HOLD_MAX;i++)
		if (bombs[i].count <= 0)
			return i;
	return -1;
}

int getExistBomb(int _x, int _y) {
	for (int i = 0;i < BOMB_HOLD_MAX;i++)
		if ((bombs[i].count > 0) && (bombs[i].x == _x) && (bombs[i].y == _y))
			return i;
	return -1;
}
bool checkCanMove(int _x, int _y) {
	switch (cells[_y][_x]) {
	case CELL_TYPE_HARD:
	case CELL_TYPE_SOFT:
		return false;
	default:
		if ((getMonster(_x, _y) < 0)
			&& (getExistBomb(_x, _y) < 0))
			return true;
	}
	return false;
}
void display(void) {

	system("cls");
	int left = monsters[0].x - SCREEN_WIDTH / 2;
	if (left < 0)
		left = 0;
	if (left > MAP_WIDTH - SCREEN_WIDTH)
		left = MAP_WIDTH - SCREEN_WIDTH;
	for (int y = 0;y < MAP_HEIGHT;y++) {
		for (int x = left;x < left + SCREEN_WIDTH;x++) {
			int monster = getMonster(x, y);
			int bomb = getExistBomb(x, y);

			if (monster > 0)
				printf("敵");
			else if (monster == 0)
				printf("自");
			else if (bomb >= 0) {
				char aa[] = "０";
				aa[1] += bombs[bomb].count;
				printf(aa);
			}
			else
				printf(cellAA[cells[y][x]]);
		}
		printf("\n");
	}
}
void gameOver(void) {
	monsters[0].isDead = true;
	display();
	printf("GAME OVER\a");
	_getch();
	exit(0);
}
void explosion(int _x, int _y) {
	cells[_y][_x] = CELL_TYPE_EXPROSION;
	for (int j = 0; j < DIRECTION_MAX;j++) {
		int x = _x;
		int y = _y;

		for (int i = 0;i < 2;i++) {
			x += directions[j][0];
			y += directions[j][1];
			if (cells[y][x] == CELL_TYPE_HARD)
				break;
			else if (cells[y][x] == CELL_TYPE_SOFT) {
				cells[y][x] = CELL_TYPE_EXPROSION;
				break;
			}
			else {
				int monster = getMonster(x, y);
				if (monster > 0)
					monsters[monster].isDead = true;
				else if (monster == 0) {
					cells[y][x] = CELL_TYPE_EXPROSION;
					gameOver();
				}


			}
			int bomb = getExistBomb(x, y);
			if (bomb >= 0) {
				bombs[bomb].count = 0;
				explosion(x, y);
			}
			cells[y][x] = CELL_TYPE_EXPROSION;

		}
	}
}


int main(void) {
	srand((unsigned int)time(NULL));
	for (int y = 0;y < MAP_HEIGHT;y += MAP_HEIGHT - 1)
		for (int x = 0; x < MAP_WIDTH; x++)
			cells[y][x] = CELL_TYPE_HARD;

	for (int y = 0;y < MAP_HEIGHT;y++)
		for (int x = 0;x < MAP_WIDTH;x += MAP_WIDTH - 1)
			cells[y][x] = CELL_TYPE_HARD;
	for (int y = 1;y < MAP_HEIGHT - 1;y++)
		for (int x = 1; x < MAP_WIDTH - 1; x++) {
			if ((x % 2 == 0) && (y % 2 == 0))
				cells[y][x] = CELL_TYPE_HARD;
			else if (rand() % 2)
				cells[y][x] = CELL_TYPE_SOFT;
		}
	cells[1][1] =
		cells[2][1] =
		cells[1][2] = CELL_TYPE_NONE;

	setRandomSoftPosition(&exitX, &exitY);
	monsters[0].x =
		monsters[0].y = 1;
	for (int i = 1;i < MONSTER_MAX;i++)
		setFreePosition(&monsters[i].x, &monsters[i].y);
	while (1) {
		display();
		for (int y = 1;y < MAP_HEIGHT - 1;y++)
			for (int x = 1; x < MAP_WIDTH - 1; x++)
				if (cells[y][x] == CELL_TYPE_EXPROSION)
					cells[y][x] = ((x == exitX) && (y == exitY))
					 ?CELL_TYPE_EXIT
				     :CELL_TYPE_NONE;

		int x = monsters[0].x;
		int y = monsters[0].y;


		switch (_getch()) {
		case'w':y--; break;
		case's':y++; break;
		case'a':x--; break;
		case'd':x++; break;
		case' ':
		int bomb = getFreeBomb();

		if (bomb >= 0&& (getExistBomb(x, y) < 0)) {
			bombs[bomb].x = monsters[0].x; //ボムの配置
			bombs[bomb].y = monsters[0].y;
			bombs[bomb].count = BOMB_COUNT_MAX;
				
		}

		break;
		
	}

		if (getMonster(x, y) > 0)
			gameOver();

		if (checkCanMove(x, y)) {
			monsters[0].x = x;
			monsters[0].y = y;
			if (cells[y][x] == CELL_TYPE_EXIT) {
				int enemyCount = 0;
				for (int i = 1; i < MONSTER_MAX;i++)
					if (!monsters[i].isDead) {
						enemyCount++;
						break;
					}if (enemyCount <= 0) {
						printf("CLEAR!\a");
						_getch();
						exit(0);
					}

			}
		}


		for (int i = 0;i < BOMB_HOLD_MAX;i++) {
			if (bombs[i].count <= 0)
				continue;

			bombs[i].count--;
			if (bombs[i].count <= 0)
				explosion(bombs[i].x, bombs[i].y);

		}
		for (int i = 1;i < MONSTER_MAX;i++) {
			if (monsters[i].isDead)
				continue;

			int x = monsters[i].x + directions[monsters[i].direction][0];
			int y = monsters[i].y + directions[monsters[i].direction][1];
			if (getMonster(x, y) == 0) {
				monsters[i].x = x;
				monsters[i].y = y;
				gameOver();
			}
			else if (checkCanMove(x, y)) {
				monsters[i].x = x;
				monsters[i].y = y;
				if (cells[monsters[i].y][monsters[i].x] == CELL_TYPE_EXPROSION)
					monsters[i].isDead = true;
			}
			else
				monsters[i].direction = rand() % DIRECTION_MAX;

		}
	}
}
