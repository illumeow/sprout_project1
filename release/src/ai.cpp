#include "splendor.hpp"
#include <bits/stdc++.h>
using namespace std;

Gem my_gameboard[BOARD_HEIGHT][BOARD_WIDTH];

bool check_eliminate(Pos *pos) {
  // 01~98
  for(int i=0; i<BOARD_HEIGHT; i++) {
    for(int j=1; j<BOARD_WIDTH-1; j++) {
      if(my_gameboard[i][j-1].type==my_gameboard[i][j].type 
      && my_gameboard[i][j].type==my_gameboard[i][j+1].type 
      && my_gameboard[i][j].type!=GEM_NULL) {
        if(pos!=nullptr) {
          pos->x = i;
          pos->y = j;
        }
        return true;
      }
    }
  }
  // 10~89
  for(int i=1; i<BOARD_HEIGHT-1; i++) {
    for(int j=0; j<BOARD_WIDTH; j++) {
      if(my_gameboard[i-1][j].type==my_gameboard[i][j].type 
      && my_gameboard[i][j].type==my_gameboard[i+1][j].type 
      && my_gameboard[i][j].type!=GEM_NULL) {
        if(pos!=nullptr) {
            pos->x = i;
            pos->y = j;
        }
        return true;
      }
    }
  }
  return false;
}

bool check_swap(Pos a, Pos b) {
  // Q, z 不能替換
  if(my_gameboard[a.x][a.y].ability == ABI_BOMB 
  && my_gameboard[b.x][b.y].ability == ABI_KILLSAME 
  || my_gameboard[b.x][b.y].ability == ABI_BOMB 
  && my_gameboard[a.x][a.y].ability == ABI_KILLSAME) return false;
  // Q, z 不須連線
  if(my_gameboard[a.x][a.y].ability == ABI_BOMB 
  || my_gameboard[a.x][a.y].ability == ABI_KILLSAME 
  || my_gameboard[b.x][b.y].ability == ABI_BOMB 
  || my_gameboard[b.x][b.y].ability == ABI_KILLSAME) return true;
  // swap(a, b) in my_gameboard then just check_eliminate(nullptr) and swap it back
  swap(my_gameboard[a.x][a.y], my_gameboard[b.x][b.y]);
  bool result = check_eliminate(nullptr);
  swap(my_gameboard[a.x][a.y], my_gameboard[b.x][b.y]);
  return result;
}

void ai(Pos& pos1, Pos& pos2) {
  // TODO: Task 4.
  /*
  初步想法: 
  優先消除特殊寶石，掃過盤面知道每種寶石各有幾個
  之後用"能消除多少寶石"來決定要消除哪個特殊寶石
  若沒有特殊寶石則試著做出來，都做不出來就隨機選一個消除?
  不確定能不能預測drop以後的結果(不含新生成的)來決定
  4/25 edit:
  可以直接模擬盤面掉落的情形
  用dropping()的code
  注意: check_xxx()的執行對象是gameboard, 所以不一定能用
  check_inboard()可以。
  */
  // You should remove the code below and determine the four values by yourself.
  for(int i=0; i<BOARD_HEIGHT; i++) {
    for(int j=0; j<BOARD_WIDTH; j++) {
        my_gameboard[i][j] = get_gem({i, j});
    }
  }

  return;
}
