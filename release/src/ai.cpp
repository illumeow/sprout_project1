#include "splendor.hpp"
#include <bits/stdc++.h>
using namespace std;


Gem my_gameboard[BOARD_HEIGHT][BOARD_WIDTH];
int type_list[GEM_CNT];
bool have_special = false;
GemData special[BOARD_HEIGHT*BOARD_WIDTH];
Pos direc[4] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
int index;
ElimData eli_data[BOARD_HEIGHT * BOARD_WIDTH];
GemData current_gem;
int abi;
Pos pos;
Pos tar, backup_pos1, cur_pos;
int max_eli, second_eli, cur_cnt;
int cur_type;
int tmp_cnt;


struct ElimData
{
  Pos pos;
  Pos tar;
  int cnt;
};


void output_my_gameboard() {
  cout << " ";
  for (int i = 0; i < BOARD_WIDTH; ++i) {
    cout << " " << i;
  }
  cout << '\n';
  for (int i = 0; i < BOARD_HEIGHT; ++i) {
    /* Chi-chun edit: Display line numbers for the user's convenience. */
    cout << "\033[0m" << i << " ";
    for (int j = 0; j < BOARD_WIDTH; ++j) {
      string color = get_color(my_gameboard[i][j].type);
      string ansi = "\x1b[" + color + "m";
      cout << ansi << "x ";
    }
    cout << "\x1b[0m";
    cout << '\n';
  }
  cout << "\n";
}

void init_typelist() {
  for(int i=0; i<6; i++) {
    type_list[i] = 0;
  }
}

void check_special(Pos, Pos, int&);

void remove_gem(Pos pos) {
  my_gameboard[pos.x][pos.y].type = GEM_NULL;
  my_gameboard[pos.x][pos.y].ability = ABI_NULL;
}

void recover_gem(Pos pos, int type, int ability) {
  my_gameboard[pos.x][pos.y].ability = type;
  my_gameboard[pos.x][pos.y].ability = ability;
}

void remove_gem_special(Pos pos, Pos elim_by, int &elim_cnt) {
  my_gameboard[pos.x][pos.y].type = GEM_NULL;
  if(my_gameboard[pos.x][pos.y].ability >= ABI_CROSS) {
    check_special(pos, elim_by, elim_cnt);
  }
  my_gameboard[pos.x][pos.y].ability = ABI_NULL;
}

void check_cross(Pos pos, int &elim_cnt) {
  remove_gem(pos);
  for(int i=0; i<BOARD_WIDTH; i++) {
    if(my_gameboard[pos.x][i].type > GEM_NULL) elim_cnt++;
    remove_gem_special({pos.x, i}, pos, elim_cnt);
  }
  for(int i=0; i<BOARD_HEIGHT; i++) {
    if(my_gameboard[i][pos.y].type > GEM_NULL) elim_cnt++;
    remove_gem_special({i, pos.y}, pos, elim_cnt);
  }
  recover_gem(pos, my_gameboard[pos.x][pos.y].type, ABI_CROSS);
}

void check_bomb(Pos pos, int &elim_cnt) {
  remove_gem(pos);
  for(int i=pos.x-2; i<=pos.x+2; i++) {
    for(int j=pos.y-2; j<=pos.y+2; j++) {
      if(check_inboard({i, j})) {
        if(my_gameboard[i][j].type > GEM_NULL) elim_cnt++;
        remove_gem_special({i, j}, pos, elim_cnt);
      }
    }
  }
  recover_gem(pos, GEM_NULL, ABI_BOMB);
}

void check_killsame(Pos pos, Pos elim_by, int &elim_cnt) {
  /*elim by z*/
  if(my_gameboard[elim_by.x][elim_by.y].ability == ABI_KILLSAME) {
    elim_cnt = -10;
    return;
  }

  /*elim by Q*/
  if(my_gameboard[elim_by.x][elim_by.y].ability == ABI_BOMB) {
    elim_cnt += type_list[gen_rand_type()];
    return;
  }

  /*elim by +, normal*/
  for(int i=0; i<BOARD_HEIGHT; i++) {
    for(int j=0; j<BOARD_WIDTH; j++) {
      if(my_gameboard[i][j].type == my_gameboard[elim_by.x][elim_by.y].type) {
        elim_cnt++;
        remove_gem_special({i, j}, pos, elim_cnt);
      }
    }
  }
  return;
}

void check_special(Pos pos, Pos elim_by, int &elim_cnt) {
  if(my_gameboard[pos.x][pos.y].ability == ABI_BOMB) check_bomb(pos, elim_cnt);
  else if(my_gameboard[pos.x][pos.y].ability == ABI_KILLSAME) check_killsame(pos, elim_by, elim_cnt);
  else if(my_gameboard[pos.x][pos.y].ability == ABI_CROSS) check_cross(pos, elim_cnt);
}

bool check_my_eliminate() {
  //cout << "elim" << '\n';
  for(int i=0; i<BOARD_HEIGHT; i++) {
    for(int j=1; j<BOARD_WIDTH-1; j++) {
      if(my_gameboard[i][j-1].type==my_gameboard[i][j].type 
      && my_gameboard[i][j].type==my_gameboard[i][j+1].type 
      && my_gameboard[i][j].type!=GEM_NULL) {
        //cout << i << " " << j << "\nup================\n";
        return true;
      }
    } 
  }

  for(int i=1; i<BOARD_HEIGHT-1; i++) {
    for(int j=0; j<BOARD_WIDTH; j++) {
      if(my_gameboard[i-1][j].type==my_gameboard[i][j].type 
      && my_gameboard[i][j].type==my_gameboard[i+1][j].type 
      && my_gameboard[i][j].type!=GEM_NULL) {
        //cout << i << " " << j << "\ndown================\n";
        return true;
      }
    }
  }

  return false;
}


void ai(Pos& pos1, Pos& pos2) {
  cout << "ai" << endl;
  // TODO: Task 4.
  /*
  初步想法: 
  優先消除特殊寶石，掃過盤面知道每種寶石各有幾個
  之後用"能消除多少寶石"來決定要消除哪個特殊寶石 --4/29 1:40 finished
  4/28: 遞迴check elim_cnt --NO!!!
  若沒有特殊寶石則試著做出來，都做不出來就隨機選一個消除?
  不確定能不能預測drop以後的結果(不含新生成的)來決定 -> 不能
  4/25 edit:
  可以直接模擬盤面掉落的情形--4/28: NO!!!!
  用dropping()的code
  注意: check_xxx()的執行對象是gameboard, 所以不一定能用
  check_inboard()可以。
  4/28 edit:
  [不能]暴力遞迴模擬。
  */

  // You should remove the code below and determine the four values by yourself.
  index = 0;
  init_typelist();
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

  for(int i=0; i<6; i++) {
    cout << type_list[i] << " ";
  }
  cout << endl;

  // output_my_gameboard();

  /* special comes first */
  if(have_special) {
    cout << "special!" << '\n';
    // for each special(GemData)
    for(int i=0; i<index; i++) {
      current_gem = special[i];
      abi = current_gem.gem.ability;
      pos = current_gem.pos;

      eli_data[i].pos = pos;
      if(abi == ABI_CROSS) check_cross(pos, eli_data[i].cnt);
      else if(abi == ABI_BOMB) check_bomb(pos, eli_data[i].cnt);
      else if(abi == ABI_KILLSAME) check_killsame(pos, );

      max_eli = 0;
      second_eli = 0;
      for(int i=0; i<index; i++) {
        cur_cnt = eli_data[i].cnt;
        cur_pos = eli_data[i].pos;
        if(cur_cnt > max_eli) {
          max_eli = cur_cnt;
          pos1 = cur_pos;
        }
        if(second_eli < cur_cnt && cur_cnt < max_eli && my_gameboard[cur_pos.x][cur_pos.y].ability > ABI_CROSS) {
          second_eli = cur_cnt;
          backup_pos1 = cur_pos;
        }
      }
    }
      if(my_gameboard[pos1.x][pos1.y].ability == ABI_CROSS) {
        cur_type = my_gameboard[pos1.x][pos1.y].type;
        for(int i=0; i<4; i++) {
          if(cur_type == my_gameboard[pos1.x+direc[i].x][pos1.y+direc[i].y].type) {
            // 別人過來
          }
          else {
            // 我過去
          }
        }
      }
      if(my_gameboard[pos1.x][pos1.y].ability == ABI_BOMB) {
        for(int i=0; i<4; i++) {
          tar = {pos1.x+direc[i].x, pos1.y+direc[i].y};
          if(check_inboard(tar)) break;
        }
      }
      else { // killsame
        tar = eli_data[i].tar;
      }
      pos2 = tar;
      cout << pos1.x << " " << pos1.y << '\n';
      cout << pos2.x << " " << pos2.y << '\n';
    }
    return;
  }
    
  /*try to gen special*/
  
  /* rand one */
  for (int i = 0; i < BOARD_HEIGHT; i++) {
    for (int j = 0; j < BOARD_WIDTH; j++) {
      for(int k=0; k<2; k++) {
        tar = {i+direc[k].x, j+direc[k].y};
        if(!check_inboard(tar)) continue;
        swap(my_gameboard[i][j], my_gameboard[tar.x][tar.y]);
        bool result = check_my_eliminate();
        swap(my_gameboard[i][j], my_gameboard[tar.x][tar.y]);
        if(result) {
          output_my_gameboard();
          pos1 = {i, j};
          pos2 = tar;
          //cout << pos1.x << " " << pos1.y << '\n';
          //cout << pos2.x << " " << pos2.y << '\n';
          return;
        }
      } 
    }
  }
  return;
}

