#include "nm.h"
#include <stdlib.h> 
#include <string.h> 
// 队列 (list) 的最大容量
#define MAX_BOARDS 100000


//  棋盘队列。
static board board_list[MAX_BOARDS];

//  辅助函数：检查一个棋盘是否已完全清空 (全部为 0)。
static bool is_board_solved(board* p) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (p->cells[y][x] != 0) {
                return false; // 发现一个非 0 数字
            }
        }
    }
    return true; // 所有格子都是 0
}


// 辅助函数：比较两个棋盘 b1 和 b2 是否完全相同。 true 如果相同，否则 false。
static bool are_boards_equal(board* b1, board* b2) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (b1->cells[y][x] != b2->cells[y][x]) {
                return false;
            }
        }
    }
    return true;
}


//   辅助函数：检查棋盘 'p' 是否已经存在于 'board_list' 中。
static bool is_board_in_list(board* p, int count) {
    for (int i = 0; i < count; i++) {
        if (are_boards_equal(p, &board_list[i])) {
            return true; // 找到了一个匹配项
        }
    }
    return false; // 这是一个新棋盘
}


bool solve(int seed) {
    int f = 0; // 'front' 索引：我们要处理的下一个棋盘
    int b = 0; // 'back' 索引：我们要在哪里插入新棋盘

    // 步骤 2: 将初始棋盘放入列表
    // 检查队列是否至少有 1 个空间
    if (b >= MAX_BOARDS) {
        return false; // 队列太小
    }
    board_list[b] = randfill(seed);
    b++; // 队列中现在有 1 个棋盘

    // 循环，直到队列为空 (f == b)
    while (f < b) {
        // 步骤 3: 从队列前端获取当前要处理的棋盘
        board current_board = board_list[f];
        f++; // 将 'front' 索引前移

        // 步骤 4: 为 'current_board' 查找所有可能的 "子棋盘"
        // (即尝试所有可能的 'take' 操作)
        
        // 尝试坐标的每一种组合
        for (int y1 = 0; y1 < HEIGHT; y1++) {
        for (int x1 = 0; x1 < WIDTH; x1++) {
            for (int y2 = 0; y2 < HEIGHT; y2++) {
            for (int x2 = 0; x2 < WIDTH; x2++) {
                
                // 创建一个副本，以便 'take' 可以修改它
                board next_board = current_board;
                pair z = {x1, y1, x2, y2};

                // 尝试 'take' 操作
                if (take(&next_board, z)) {
                    // 成功！'next_board' 是一个有效的 "子棋盘"

                    // 步骤 5 (a): 检查这个子棋盘是否是最终解？
                    if (is_board_solved(&next_board)) {
                        return true; // 找到了！
                    }

                    // 步骤 5 (b): 检查这个子棋盘是否是 "唯一" 的
                    if (!is_board_in_list(&next_board, b)) {
                        
                        // 这是一个新棋盘，将它添加到队列末尾
                        if (b >= MAX_BOARDS) {
                            // 错误：队列已满，但还未找到解
                            return false;
                        }
                        board_list[b] = next_board;
                        b++;
                    }
                    // else: (如果 'is_board_in_list' 为 true)
                    // 这是一个重复的棋盘，按要求 "忽略它"。
                }
            }
            }
        }
        }
    } 

    // 循环结束，意味着队列已空 (f == b)
    // 我们已经探索了所有可能的棋盘，但没有找到解。
    return false;
}


//   辅助函数：检查两个点 (x1, y1) 和 (x2, y2) 是否 "连通"。
//   连通的定义：
//   1. 它们是相邻的（8 连通）。
//   2. 它们在同一条直线上（水平、垂直或对角线），
//   并且它们之间的所有格子都是空的 (值为 0)。
 
static bool is_connected(board* p, int x1, int y1, int x2, int y2) {
    int dx = abs(x1 - x2);
    int dy = abs(y1 - y2);

    // --- 1. 检查是否相邻 (8-连通) ---
    // (我们已经知道它们不是同一点)
    if (dx <= 1 && dy <= 1) {
        return true;
    }

    // --- 2. 检查是否为清晰的直线路径 ---
    // (如果它们不相邻)

    bool is_horizontal = (dy == 0);
    bool is_vertical = (dx == 0);
    bool is_diagonal = (dx == dy);

    // 如果不是水平、垂直或对角线，则它们无法连通
    if (!is_horizontal && !is_vertical && !is_diagonal) {
        return false;
    }

    // 确定从(x1, y1)到(x2, y2)检查路径的“步进”方向
    // 
    int step_x = 0, step_y = 0;//x1 == x2时，x轴方向不用移动
    if(x2 > x1) step_x = 1;
    else if(x2 < x1) step_x = -1;
    if(y2 > y1) step_y = 1;
    else if(y2 < y1) step_y = -1;

    // 从 (x1, y1) 的下一个格子开始检查
    int cx = x1 + step_x;
    int cy = y1 + step_y;

    // 沿着路径迭代，直到到达 (x2, y2)
    while (cx != x2 || cy != y2) {
        // 检查路径上是否有障碍物 (非 0 的数字)
        if (p->cells[cy][cx] != 0) {
            return false; // 路径被阻挡
        }
        // 移动到路径上的下一个格子
        cx += step_x;
        cy += step_y;
    }

    // 循环完成，没有发现障碍物
    return true;
}


bool take(board* p, pair z) {
    int x1 = z.x1, y1 = z.y1;
    int x2 = z.x2, y2 = z.y2;

    // 1. 检查坐标是否在边界内
    if (x1 < 0 || x1 >= WIDTH || y1 < 0 || y1 >= HEIGHT ||
        x2 < 0 || x2 >= WIDTH || y2 < 0 || y2 >= HEIGHT) {
        return false;
    }

    // 2. 检查是否匹配同一点
    if (x1 == x2 && y1 == y2) {
        return false;
    }

    // 3. 获取两个格子的值
    int val1 = p->cells[y1][x1];
    int val2 = p->cells[y2][x2];

    // 4. 检查是否匹配了空格子 (值为 0)
    if (val1 == 0 || val2 == 0) {
        return false;
    }

    // --- 规则 1：检查值的匹配 ---
    bool value_match = (val1 == val2) || (val1 + val2 == 10);
    if (!value_match) {
        return false; // 值不匹配
    }

    // --- 规则 2：检查位置的匹配 (连通性) ---
    if (!is_connected(p, x1, y1, x2, y2)) {
        return false; // 位置不连通
    }

    // --- 所有检查通过 ---
    // 这是一个有效的匹配，移除这两个数字
    p->cells[y1][x1] = 0;
    p->cells[y2][x2] = 0;
    
    return true; // 成功匹配
}

board randfill(int n)
{
    srand(n); // 使用种子 n 初始化
    board b;
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            // 填充 1 到 9 的随机数
            b.cells[y][x] = rand() % 9 + 1;
        }
    }
    return b;
}

void test(void)
{
    board b;
    pair p;
    bool result;

    // 一个辅助宏，用于在每次测试前清空棋盘 (将所有格子设为 0)
    #define CLEAR_BOARD() memset(&b, 0, sizeof(board))

    // --- Test 1: 简单相邻 (水平, 相同值) ---
    CLEAR_BOARD();
    b.cells[0][0] = 5;
    b.cells[0][1] = 5;
    p = (pair){0,0, 1,0};
    result = take(&b, p);
    assert(result == true); // 应该成功
    assert(b.cells[0][0] == 0); // 应该被移除
    assert(b.cells[0][1] == 0); // 应该被移除

    // --- Test 2: 简单相邻 (垂直, 和为 10) ---
    CLEAR_BOARD();
    b.cells[1][1] = 3;
    b.cells[2][1] = 7;
    p = (pair){1,1, 1,2};
    result = take(&b, p);
    assert(result == true);
    assert(b.cells[1][1] == 0);
    assert(b.cells[2][1] == 0);

    // --- Test 3: 简单相邻 (对角线) ---
    CLEAR_BOARD();
    b.cells[1][1] = 8;
    b.cells[2][2] = 8;
    p = (pair){1,1, 2,2};
    result = take(&b, p);
    assert(result == true);
    assert(b.cells[1][1] == 0);
    assert(b.cells[2][2] == 0);

    // --- Test 4: "值不匹配" ---
    CLEAR_BOARD();
    b.cells[0][0] = 1;
    b.cells[0][1] = 2; // 1 和 2 既不相等，和也不为 10
    p = (pair){0,0, 1,0};
    result = take(&b, p);
    assert(result == false); // 应该失败
    assert(b.cells[0][0] == 1); // 应该保持不变
    assert(b.cells[0][1] == 2); // 应该保持不变

    // --- Test 5: "长距离" (水平, 路径清晰) ---
    CLEAR_BOARD();
    b.cells[0][0] = 4;
    b.cells[0][1] = 0; // 路径是 0 (清晰)
    b.cells[0][2] = 6;
    p = (pair){0,0, 2,0};
    result = take(&b, p);
    assert(result == true); // 应该成功
    assert(b.cells[0][0] == 0);
    assert(b.cells[0][2] == 0);

    // --- Test 6: "长距离" (对角线, 路径清晰) ---
    CLEAR_BOARD();
    b.cells[0][0] = 9;
    b.cells[1][1] = 0; // 清晰
    b.cells[2][2] = 0; // 清晰
    b.cells[3][3] = 9;
    p = (pair){0,0, 3,3};
    result = take(&b, p);
    assert(result == true);
    assert(b.cells[0][0] == 0);
    assert(b.cells[3][3] == 0);

    // --- Test 7: "路径被阻挡" (水平) ---
    CLEAR_BOARD();
    b.cells[0][0] = 4;
    b.cells[0][1] = 5; // 5 是一个阻挡物!
    b.cells[0][2] = 6;
    p = (pair){0,0, 2,0};
    result = take(&b, p);
    assert(result == false); // 应该失败
    assert(b.cells[0][0] == 4); // 应该保持不变

    // --- Test 8: "路径被阻挡" (对角线) ---
    CLEAR_BOARD();
    b.cells[0][0] = 9;
    b.cells[1][1] = 0; // 清晰
    b.cells[2][2] = 1; // 1 是一个阻挡物!
    b.cells[3][3] = 9;
    p = (pair){0,0, 3,3};
    result = take(&b, p);
    assert(result == false);

    // --- Test 9: "无效路径" (非直线) ---
    CLEAR_BOARD();
    b.cells[0][0] = 7;
    b.cells[1][2] = 3; // (0,0) 和 (1,2) 不是一条直线
    p = (pair){0,0, 2,1}; // 坐标错误，(0,0) 和 (2,1)
    result = take(&b, p);
    assert(result == false);

    // --- Test 10: "坐标越界" ---
    CLEAR_BOARD();
    b.cells[0][0] = 5;
    p = (pair){0,0, 0, -1}; // y2 是 -1 (越界)
    result = take(&b, p);
    assert(result == false);
    
    p = (pair){0,0, WIDTH, 0}; // x2 是 WIDTH (5) (越界)
    result = take(&b, p);
    assert(result == false);

    // --- Test 11: "坐标相同" ---
    CLEAR_BOARD();
    b.cells[0][0] = 5;
    p = (pair){0,0, 0,0}; // 两个点是同一个
    result = take(&b, p);
    assert(result == false);

    // --- Test 12: "匹配空格子" ---
    CLEAR_BOARD();
    b.cells[0][0] = 5;
    b.cells[0][1] = 0; // 这是一个空格子
    p = (pair){0,0, 1,0}; // 尝试匹配 5 和 0
    result = take(&b, p);
    assert(result == false);

}
