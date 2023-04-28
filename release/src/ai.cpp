#include "splendor.hpp"
#include <bits/stdc++.h>
using namespace std;

#define STOP 100


Gem my_gameboard[BOARD_HEIGHT][BOARD_WIDTH];
int type_list[GEM_CNT];
bool have_special = false;
GemData special[BOARD_HEIGHT*BOARD_WIDTH];
Pos dir[4] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};

struct ElimData
{
  Pos pos;
  Pos tar;
  int cnt;
};


void remove_gem_ability(Pos pos) {
  my_gameboard[pos.x][pos.y].ability = ABI_NULL;
}

void recover_gem_ability(Pos pos, int ability) {
  my_gameboard[pos.x][pos.y].ability = ability;
}

void remove_gem_ability_special(Pos pos, int &elim_cnt) {
  if(my_gameboard[pos.x][pos.y].ability >= ABI_CROSS) check_special(pos, pos, elim_cnt);
  my_gameboard[pos.x][pos.y].ability = ABI_NULL;
}

void check_cross(Pos pos, int &elim_cnt) {
  remove_gem_ability(pos);
  elim_cnt += BOARD_HEIGHT+BOARD_WIDTH-1;
  for(int i=0; i<BOARD_WIDTH; i++) remove_gem_ability_special({pos.x, i}, elim_cnt);
  for(int i=0; i<BOARD_HEIGHT; i++) remove_gem_ability_special({i, pos.y}, elim_cnt);
  recover_gem_ability(pos, ABI_CROSS);
}

void check_bomb(Pos pos, int &elim_cnt) {
  remove_gem_ability(pos);
  for(int i=pos.x-2; i<=pos.x+2; i++) {
    if(i < 0) i = 0;
    else if(i>=BOARD_HEIGHT) i = BOARD_HEIGHT;
    for(int j=pos.y-2; j<=pos.y+2; j++) {
      if(j < 0) j = 0;
      else if(j>=BOARD_WIDTH) j = BOARD_WIDTH;
      elim_cnt++;
      remove_gem_ability_special({i, j}, elim_cnt);
    }
  }
  recover_gem_ability(pos, ABI_BOMB);
}

void check_killsame(Pos pos, Pos &elim_tar, int &elim_cnt) {
  int tmp_cnt[4]; 
  
  /*elim by Q, z*/
  if(my_gameboard[elim_tar.x][elim_tar.y].type==GEM_NULL) {
    elim_cnt = type_list[gen_rand_type()-1];
    elim_tar = pos;
    return;
  }

  /*elim by +*/
  if(my_gameboard[elim_tar.x][elim_tar.y].ability==ABI_CROSS) {
    for(int i=0; i<BOARD_HEIGHT; i++) {
      for(int j=0; j<BOARD_WIDTH; j++) {
          if(my_gameboard[i][j].type == my_gameboard[elim_tar.x][elim_tar.y].type) tmp_cnt[0]++;
      }
    }
    elim_cnt = tmp_cnt[0];
    elim_tar = pos;
    return;
  }

  /*elim by normal gem*/
  for(int i=0; i<4; i++) {
    Pos tar = {pos.x + dir[i].x, pos.y + dir[i].y};
    if(!check_inboard(tar)) continue;
    remove_gem_ability(pos);
    for(int i=0; i<BOARD_HEIGHT; i++) {
      for(int j=0; j<BOARD_WIDTH; j++) {
          if(my_gameboard[i][j].type == my_gameboard[tar.x][tar.y].type){
            tmp_cnt[i]++;
            remove_gem_ability_special({i, j}, elim_cnt);
          }
      }
    }
    recover_gem_ability(pos, ABI_KILLSAME);
  }
  int ret_cnt = 0;
  Pos ret_tar;
  for(int i=0; i<4; i++) {
    if(tmp_cnt[i] > ret_cnt) {
      ret_cnt = tmp_cnt[i];
      ret_tar = {pos.x + dir[i].x, pos.y + dir[i].y};
    }
  }
  elim_cnt = ret_cnt;
  elim_tar = ret_tar;
}

void check_special(Pos pos, Pos tar, int &elim_cnt) {
  if(my_gameboard[pos.x][pos.y].ability == ABI_BOMB) check_bomb(pos, elim_cnt);
  else if(my_gameboard[pos.x][pos.y].ability == ABI_KILLSAME) check_killsame(pos, tar, elim_cnt);
  else if(my_gameboard[pos.x][pos.y].ability == ABI_CROSS) check_cross(pos, elim_cnt);
}

void ai(Pos& pos1, Pos& pos2) {
  // TODO: Task 4.
  /*
  初步想法: 
  優先消除特殊寶石，掃過盤面知道每種寶石各有幾個
  之後用"能消除多少寶石"來決定要消除哪個特殊寶石
  4/28: 遞迴check elim_cnt
  若沒有特殊寶石則試著做出來，都做不出來就隨機選一個消除?
  不確定能不能預測drop以後的結果(不含新生成的)來決定
  4/25 edit:
  可以直接模擬盤面掉落的情形: NO!!!!
  用dropping()的code
  注意: check_xxx()的執行對象是gameboard, 所以不一定能用
  check_inboard()可以。
  4/28 edit:
  [不能]暴力遞迴模擬。
  */

  // You should remove the code below and determine the four values by yourself.
  int index = 0;
  ElimData eli_data[BOARD_HEIGHT * BOARD_WIDTH];
  // sweep over the gameboard and record special(also bool) atst
  for(int i=0; i<BOARD_HEIGHT; i++) {
    for(int j=0; j<BOARD_WIDTH; j++) {
        Gem tmp = get_gem({i, j});
        my_gameboard[i][j] = tmp;
        type_list[tmp.type]++;
        if(tmp.ability >= ABI_CROSS) {
          have_special = true;
          special[index++] = {{i, j}, {tmp.type, tmp.ability}}; 
        }
    }
  }

  /* special comes first */
  if(have_special) {
    // for each special(GemData)
    for(int i=0; i<index; i++) {
      GemData sgem = special[i];
      int abi = sgem.gem.ability;
      Pos pos = sgem.pos;
      eli_data[i].pos = pos;
      if(abi == ABI_CROSS) check_cross(pos, eli_data[i].cnt);
      else if(abi == ABI_BOMB) check_bomb(pos, eli_data[i].cnt);
      else if(abi == ABI_KILLSAME) check_killsame(pos, eli_data[i].tar, eli_data[i].cnt);
        
      int max_eli = 0;
      for(int i=0; i<index; i++) {
        if(eli_data[i].cnt > max_eli) {
          max_eli = eli_data[i].cnt;
          pos1 = eli_data[i].pos;
          pos2 = eli_data[i].tar;
        }
      }
    }
    return;
  }
  return;
}

