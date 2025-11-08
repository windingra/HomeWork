#pragma once

/* Put other #includes here,
   your struct board, helper function
   prototypes etc.
*/
#define HEIGHT 4
#define WIDTH 5

typedef struct {
    //0表示被移除
    int cells[HEIGHT][WIDTH];
} board;