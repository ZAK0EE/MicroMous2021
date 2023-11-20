/*
  ____     ___    _   _      _      ____     __  __
 / ___|   / _ \  | | | |    / \    |  _ \    \ \/ /
 \___ \  | | | | | | | |   / _ \   | | | |    \  /
  ___) | | |_| | | |_| |  / ___ \  | |_| |    /  \
 |____/   \__\_\  \___/  /_/   \_\ |____/    /_/\_\

                      P.M.3
*/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "stack.h"
#include "API.h"

#define DEBUG
#define MAZE_WIDTH 16
#define MAZE_HEIGHT 16
enum Directons
{
    Forward = 0, Right, Back, Left
};

//This is for different tests purpose.
#define c1_y 7
#define c1_x 7




//Position over the maze. (16x16) maze.
uint8_t pos[16][16] =
{
  {14, 13, 12, 11, 10,  9,  8,  7,  7,  8,  9, 10, 11, 12, 13, 14},
  {13, 12, 11, 10,  9,  8,  7,  6,  6,  7,  8,  9, 10, 11, 12, 13},
  {12, 11, 10,  9,  8,  7,  6,  5,  5,  6,  7,  8,  9, 10, 11, 12},
  {11, 10,  9,  8,  7,  6,  5,  4,  4,  5,  6,  7,  8,  9, 10, 11},
  {10,  9,  8,  7,  6,  5,  4,  3,  3,  4,  5,  6,  7,  8,  9, 10},
  { 9,  8,  7,  6,  5,  4,  3,  2,  2,  3,  4,  5,  6,  7,  8,  9},
  { 8,  7,  6,  5,  4,  3,  2,  1,  1,  2,  3,  4,  5,  6,  7,  8},
  { 7,  6,  5,  4,  3,  2,  1,  0,  0,  1,  2,  3,  4,  5,  6,  7},
  { 7,  6,  5,  4,  3,  2,  1,  0,  0,  1,  2,  3,  4,  5,  6,  7},
  { 8,  7,  6,  5,  4,  3,  2,  1,  1,  2,  3,  4,  5,  6,  7,  8},
  { 9,  8,  7,  6,  5,  4,  3,  2,  2,  3,  4,  5,  6,  7,  8,  9},
  {10,  9,  8,  7,  6,  5,  4,  3,  3,  4,  5,  6,  7,  8,  9, 10},
  {11, 10,  9,  8,  7,  6,  5,  4,  4,  5,  6,  7,  8,  9, 10, 11},
  {12, 11, 10,  9,  8,  7,  6,  5,  5,  6,  7,  8,  9, 10, 11, 12},
  {13, 12, 11, 10,  9,  8,  7,  6,  6,  7,  8,  9, 10, 11, 12, 13},
  {14, 13, 12, 11, 10,  9,  8,  7,  7,  8,  9, 10, 11, 12, 13, 14}
};


//uint8_t pos[MAZE_HEIGHT][MAZE_WIDTH] = {0};
//Walls map
uint8_t wall[MAZE_HEIGHT][MAZE_WIDTH] = {0};

int8_t x_pos = 0;
int8_t y_pos = 0;
Stack CellStack;
//calculate the robot steps and forks.
int8_t dir = 0; //[0, 1, 2, 3] robot direction, 0 for startup position, turning right dir++, turning left dir--.


void log(char* text)
{
    fprintf(stderr, "%s\n", text);
    fflush(stderr);
}
                        /*Functions prototypes*/
//Updates the surrounding walls for current cell only.
uint8_t update_wall(int8_t y, int8_t x);
//Returns the minimum distance for opening neighboring of the current cell.
uint8_t getmd(int8_t y, int8_t x);
//Returns true if not center.
int8_t is_center(int8_t y, int8_t x);
//Modified Fill flood algorithm code.
void flood(int8_t y, int8_t x);
//Move the robot to md
void move_robot();
//Updates the x,y coordination of the robot.
void update_pos();
//The required steps for movement
void move_step(uint8_t d);
//This function to show the cells values on the simulation.
void update_simulation()
{
    char str[3];
    for (uint8_t i = 0; i < MAZE_HEIGHT; i++)
    {
        for (uint8_t j = 0; j < MAZE_WIDTH; j++)
        {
            sprintf(str, "%d", pos[i][j]);
            API_setText(j, i, str);
        }
    }

}
//This function to show the cells' wall values on the simulation.
void show_walls()
{
    char str[3];
    for (uint8_t i = 0; i < MAZE_HEIGHT; i++)
    {
        for (uint8_t j = 0; j < MAZE_WIDTH; j++)
        {
            sprintf(str, "%d", wall[i][j]);
            //fprintf(stderr, " %d", wall[i][j]);
            API_setText(j, i, str);
        }
        printf("\n");
    }

}

uint8_t mode = 1;
int main()
{

    while(1)
    {


       // show_walls();
        if (is_center(y_pos, x_pos))
        {
			mode = !mode;
            //Start to center.
            if (mode == 0)
                pos[c1_y][c1_x] = pos[c1_y][c1_x + 1] = pos[c1_y + 1][c1_x] = pos[c1_y + 1][c1_x + 1] = 0;
            //Center to start.
            if (mode == 1)
                pos[0][0] = 0;
        }


        //Updates surrounding walls for the current cell.
        update_wall(y_pos, x_pos);
        //Applying the modified fill flood algorithm on the current cell.
        flood(y_pos, x_pos);
        //Update cells numbers to the simulation
        update_simulation();
        //Move function.
        move_robot();

    }
}

//Updates the surrounding walls for current cell only.
uint8_t update_wall(int8_t y, int8_t x)
{

    uint8_t count = 0;
    if (API_wallFront())
    {
        wall[y][x] |= (1 << (dir));
		wall[y + 1][x] |= (1 << (dir));
        count++;
    }
    if (API_wallRight())
    {
        wall[y][x] |= (1 << ((dir + 1) % 4));
        count++;
    }
    if (API_wallLeft())
    {
        wall[y][x] |= (1 << ((dir + 3) % 4));
        count++;
    }
    return count;
}

//Returns the minimum distance for opening neighboring of the current cell.
uint8_t getmd(int8_t y, int8_t x)
{
    uint8_t md = 0xFF;
    //If no walls on the right of the current cell nor on the left of the right cell, and we didn't reach the maximum x cell
    if (!(wall[y][x] & (1 << Right)) && x != MAZE_WIDTH - 1 && !(wall[y][x + 1] & (1 << Left)) )
    {
        if (pos[y][x + 1] < md)
            md = pos[y][x + 1];
    }
    //If no walls on the left of the current cell nor on the right of the left cell, and we didn't reach the minimum x cell
    if (!(wall[y][x] & (1 << Left)) && x != 0 && !(wall[y][x - 1] & (1 << Right)) )
    {
        if (pos[y][x - 1] < md)
            md = pos[y][x - 1];
    }
    //If no walls on the forward of the current cell nor on the back of the forward cell, and we didn't reach the maximum y cell
    if (!(wall[y][x] & (1 << Forward)) && y != MAZE_WIDTH - 1 && !(wall[y + 1][x] & (1 << Back)) )
    {
        if (pos[y + 1][x] < md)
            md = pos[y + 1][x];
    }
    //If no walls on the back of the current cell nor on the forward of the back cell, and we didn't reach the minimum y cell
    if (!(wall[y][x] & (1 << Back)) && y != 0 && !(wall[y - 1][x] & (1 << Forward)) )
    {
        if (pos[y - 1][x] < md)
            md = pos[y - 1][x];
    }
    return md;

}

int8_t is_open(int8_t y, int8_t x)
{
	
}
//Returns true if not center.
int8_t is_center(int8_t y, int8_t x)
{
    //From start to center.
    if (mode == 0)
    {
        if((y == c1_y && x == c1_x) ||(y == c1_y && x == c1_x + 1) ||(y == c1_y + 1 && x == c1_x) ||(y == c1_y + 1 && x == c1_x + 1))
            return 1;
        else
            return 0;
    }
    //From center to start.
    else
    {
        if(y == 0 && x ==0)
            return 1;
        else
            return 0;
    }

}
//Modified Fill flood algorithm code.
void flood(int8_t y, int8_t x)
{
	#ifdef DEBUG
	    char *size = malloc(200 * sizeof(char)); //simulation buffer
		int max = 0;
		int MAXCALL = 0;
		static int MAXOVERALL = 0;
	#endif

    CreateStack(&CellStack);
	uint8_t md;
    //Push initial x and y.
    Push(x, &CellStack);
    Push(y, &CellStack);


    while (!StackEmpty(&CellStack))
    {
        Pop(&y, &CellStack);
        Pop(&x, &CellStack);
        md = getmd(y, x);
        //Recursive break condition.
        if (md == pos[y][x] - 1)
            continue;
        pos[y][x] = md + 1;
        if(y != MAZE_HEIGHT - 1  && !is_center(y + 1, x) && !(wall[y][x] & (1 << Forward)))
        {
            Push(x, &CellStack);
            Push(y + 1, &CellStack);
        }
        if (x != 0 && !is_center(y, x - 1) && !(wall[y][x] & (1 << Left)))
        {
            Push(x - 1, &CellStack);
            Push(y, &CellStack);
        }
        if(y != 0  && !is_center(y - 1, x) && !(wall[y][x] & (1 << Back)))
        {
            Push(x, &CellStack);
            Push(y - 1, &CellStack);
        }
        if (x != MAZE_HEIGHT - 1 && !is_center(y, x + 1) && !(wall[y][x] & (1 << Right)))
        {
            Push(x + 1, &CellStack);
            Push(y, &CellStack);
        }
		#ifdef DEBUG
			MAXCALL ++;
			if (StackSize(&CellStack) > max)//DELETE THIS
				max = StackSize(&CellStack);//DELETE THIS
		#endif

    }
	#ifdef DEBUG
		if(max > MAXOVERALL)
			MAXOVERALL = max;
		sprintf(size, "***********************\nMaximum stack size is: %d\nTotal calls per move = %d\nMaximum overall is: %d\n******************\n", max, MAXCALL, MAXOVERALL);//DELETE THIS
		log(size);//DELETE THIS
	#endif



}
void move_robot()
{
    //Forward.
    if (!(wall[y_pos][x_pos] & (1 << dir)))
    {
        switch(dir)
        {
        case 0:
            if (pos[y_pos + 1][x_pos] + 1 == pos[y_pos][x_pos])
                {
                    move_step('F');
                    return;
                }
            break;
        case 1:
            if (pos[y_pos][x_pos + 1] + 1 == pos[y_pos][x_pos])
                {
                    move_step('F');
                    return;
                }
            break;
        case 2:
            if (pos[y_pos - 1][x_pos] + 1 == pos[y_pos][x_pos])
                {
                    move_step('F');
                    return;
                }
            break;
        case 3:
            if (pos[y_pos][x_pos - 1] + 1 == pos[y_pos][x_pos])
                {
                    move_step('F');
                    return;
                }
            break;
        }
    }
    //Right
    if (!(wall[y_pos][x_pos] & (1 << ((dir + 1) % 4))))
    {
        switch(dir)
        {
        case 0:
            if (pos[y_pos][x_pos + 1] + 1 == pos[y_pos][x_pos])
                {
                    move_step('R');
                    return;
                }
            break;
        case 1:
            if (pos[y_pos - 1][x_pos] + 1 == pos[y_pos][x_pos])
                {
                    move_step('R');
                    return;
                }
            break;
        case 2:
            if (pos[y_pos][x_pos - 1] + 1 == pos[y_pos][x_pos])
                {
                    move_step('R');
                    return;
                }
            break;
        case 3:
            if (pos[y_pos + 1][x_pos] + 1 == pos[y_pos][x_pos])
                {
                    move_step('R');
                    return;
                }
            break;
            }

    }
    //Left
    if (!(wall[y_pos][x_pos] & (1 << ((dir + 3) % 4))))
    {
        switch(dir)
        {
        case 0:
            if (pos[y_pos][x_pos - 1] + 1 == pos[y_pos][x_pos])
                {
                    move_step('L');
                    return;
                }
            break;
        case 1:
            if (pos[y_pos + 1][x_pos] + 1 == pos[y_pos][x_pos])
                {
                    move_step('L');
                    return;
                }
            break;
        case 2:
            if (pos[y_pos][x_pos + 1] + 1 == pos[y_pos][x_pos])
                {
                    move_step('L');
                    return;
                }
            break;
        case 3:
            if (pos[y_pos - 1][x_pos] + 1 == pos[y_pos][x_pos])
                {
                    move_step('L');
                    return;
                }
            break;
        }

    }
    //Back
    if (!(wall[y_pos][x_pos] & (1 << ((dir + 2) % 4))))
    {
        switch(dir)
        {
        case 0:
            if (pos[y_pos - 1][x_pos] + 1 == pos[y_pos][x_pos])
                {
                    move_step('B');
                    return;
                }
            break;
        case 1:
            if (pos[y_pos][x_pos - 1] + 1 == pos[y_pos][x_pos])
                {
                    move_step('B');
                    return;
                }
            break;
        case 2:
            if (pos[y_pos + 1][x_pos] + 1 == pos[y_pos][x_pos])
                {
                    move_step('B');
                    return;
                }
            break;
        case 3:
            if (pos[y_pos][x_pos + 1] + 1 == pos[y_pos][x_pos])
                {
                    move_step('B');
                    return;
                }
            break;
        }

    }
}
//Updates the x,y coordination of the robot.
void update_pos()
{
    switch(dir)
    {
    case 0:
        y_pos++;
        break;
    case 1:
        x_pos++;
        break;
    case 2:
        y_pos--;
        break;
    case 3:
        x_pos--;
        break;
    }

}
//The required steps for movement
void move_step(uint8_t d)
{
    switch(d)
    {
    case 'F':
        break;
    case 'L':
        API_turnLeft();
        dir--;
        if (dir < 0) dir += 4;
        break;
    case 'R':
        API_turnRight();
        dir++;
        dir %= 4;
        break;
    case 'B':
        API_turnRight();
        API_turnRight();
        dir +=2;
        dir %= 4;
        break;
    }
    API_moveForward();
    update_pos();
}

