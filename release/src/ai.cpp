#include "splendor.hpp"
#include <bits/stdc++.h>
using namespace std;


Gem my_gameboard[BOARD_HEIGHT][BOARD_WIDTH];
int type_list[GEM_CNT];
Pos direc[4] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}}; // down right up left


struct tarData{
  Pos target;
  bool available;
};

struct ElimData{
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

tarData tar_available[4];
bool one, two, three;
Pos tmp_tar;
int max_cnt, cur_type, tmp_cnt;
void check_cross(Pos pos, Pos &tar, int &elim_cnt) {
  cur_type = my_gameboard[pos.x][pos.y].type;
  for(int i=0; i<4; i++) tar_available[i].available = false;
  one = true;
  two = true;
  three = true;
  remove_gem(pos);
  // + 動
  if((pos.x != tar.x) || (pos.y!=tar.y)) {
    for(int i=0; i<4; i++) {
      if(i == 0) {
        if(check_inboard({pos.x+1, pos.y})) {
          for(int j=pos.y-2; j<pos.y; j++) {
            if(check_inboard({i, j})) {
              if(cur_type != my_gameboard[pos.x+1][j].type) {
                one = false;
              }
            }
          }
          for(int j=pos.y+1; j<pos.y+3; j++) {
            if(check_inboard({pos.x+1, j})) {
              if(cur_type != my_gameboard[pos.x+1][j].type) {
                two = false;
              }
            }
          }
        }
        if(check_inboard({pos.x+3, pos.y})) {
          for(int j=pos.x+2; j<pos.x+4; j++) {
            if(cur_type != my_gameboard[j][pos.y].type) {
              three = false;
            }
          }
        }
      }
      else if(i == 1){
        if(check_inboard({pos.x, pos.y+1})) {
          for(int j=pos.x-2; j<pos.x; j++) {
            if(check_inboard({j, pos.y+1})) {
              if(cur_type != my_gameboard[j][pos.y+1].type) {
                one = false;
              }
            }
          }
          for(int j=pos.x+1; j<pos.x+3; j++) {
            if(check_inboard({j, pos.y+1})) {
              if(cur_type != my_gameboard[j][pos.y+1].type) {
                two = false;
              }
            }
          }
        }
        if(check_inboard({pos.x, pos.y+3})) {
          for(int j=pos.y+2; j<pos.y+4; j++) {
            if(cur_type != my_gameboard[pos.x][j].type) {
              three = false;
            }
          }
        }
      }
      else if(i == 2){
        if(check_inboard({pos.x-1, pos.y})) {
          for(int j=pos.y-2; j<pos.y; j++) {
            if(check_inboard({i, j})) {
              if(cur_type != my_gameboard[pos.x-1][j].type) {
                one = false;
              }
            }
          }
          for(int j=pos.y+1; j<pos.y+3; j++) {
            if(check_inboard({i, j})) {
              if(cur_type != my_gameboard[pos.x-1][j].type) {
                two = false;
              }
            }
          }
        }
        if(check_inboard({pos.x-3, pos.y})) {
          for(int j=pos.x-2; j<pos.x-4; j--) {
            if(cur_type != my_gameboard[j][pos.y].type) {
              three = false;
            }
          }
        }
      }
      else {
        if(check_inboard({pos.x, pos.y-1})) {
          for(int j=pos.x-2; j<pos.x; j++) {
            if(check_inboard({j, pos.y-1})) {
              if(cur_type != my_gameboard[j][pos.y-1].type) {
                one = false;
              }
            }
          }
          for(int j=pos.x+1; j<pos.x+3; j++) {
            if(check_inboard({j, pos.y-1})) {
              if(cur_type != my_gameboard[j][pos.y-1].type) {
                two = false;
              }
            }
          }
        }
        if(check_inboard({pos.x, pos.y-3})) {
          for(int j=pos.y-2; j<pos.y-4; j--) {
            if(cur_type != my_gameboard[pos.x][j].type) {
              three = false;
            }
          }
        }
      }
      if(one || two || three) {
        tar_available[i].target = {pos.x+direc[i].x, pos.y+direc[i].y};
        tar_available[i].available = true;
      }    
    }
    // + 動了沒用
    if ( !(tar_available[0].available) 
      && !(tar_available[1].available)
      && !(tar_available[2].available)
      && !(tar_available[3].available)) 
      tar = {BOARD_HEIGHT, BOARD_WIDTH};
    else {
      max_cnt = 0;
      for(int i=0; i<4; i++) {
        if(tar_available[i].available) {
          tmp_tar = tar_available[i].target;
          tmp_cnt = 0;
          for(int i=0; i<BOARD_WIDTH; i++) {
            if(my_gameboard[tmp_tar.x][i].type > GEM_NULL) tmp_cnt++;
          }
          for(int i=0; i<BOARD_HEIGHT; i++) {
            if(my_gameboard[i][tmp_tar.y].type > GEM_NULL) tmp_cnt++;
          }
        }
        if(tmp_cnt > max_cnt) {
          max_cnt = tmp_cnt;
          tar = tmp_tar;
        }
      }
      for(int i=0; i<BOARD_WIDTH; i++) {
        if(my_gameboard[tar.x][i].type > GEM_NULL){
          elim_cnt++;
          remove_gem_special({tar.x, i}, pos, elim_cnt);
        } 
      }
      for(int i=0; i<BOARD_HEIGHT; i++) {
        if(my_gameboard[i][tar.y].type > GEM_NULL) {
          elim_cnt++;
          remove_gem_special({i, tar.y}, pos, elim_cnt);
        }
      }
    }
  }
  // 原地開消
  else {
    for(int i=0; i<BOARD_WIDTH; i++) {
      if(my_gameboard[tar.x][i].type > GEM_NULL){
        elim_cnt++;
        remove_gem_special({tar.x, i}, pos, elim_cnt);
      } 
    }
    for(int i=0; i<BOARD_HEIGHT; i++) {
      if(my_gameboard[i][tar.y].type > GEM_NULL) {
        elim_cnt++;
        remove_gem_special({i, tar.y}, pos, elim_cnt);
      }
    }
  }
  recover_gem(pos, cur_type, ABI_CROSS);
}

void check_bomb(Pos pos, Pos tar, int &elim_cnt) {
  remove_gem(pos);
  for(int i=0; i<4; i++) {
    tar = {pos.x+direc[i].x, pos.y+direc[i].y};
    if(check_inboard(tar)) break;
  }
  for(int i=pos.x-2; i<=pos.x+2; i++) {
    for(int j=pos.y-2; j<=pos.y+2; j++) {
      if(check_inboard({i, j})) {
        if(my_gameboard[i][j].type > GEM_NULL) {
          elim_cnt++;
          remove_gem_special({i, j}, pos, elim_cnt);
        }
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
  else if(my_gameboard[elim_by.x][elim_by.y].ability == ABI_BOMB) {
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
  if(my_gameboard[pos.x][pos.y].ability == ABI_BOMB) check_bomb(pos, elim_by, elim_cnt);
  else if(my_gameboard[pos.x][pos.y].ability == ABI_KILLSAME) check_killsame(pos, elim_by, elim_cnt);
  else if(my_gameboard[pos.x][pos.y].ability == ABI_CROSS) check_cross(pos, elim_by, elim_cnt);
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

bool have_special = false;
GemData special[BOARD_HEIGHT*BOARD_WIDTH];
int index;
ElimData eli_data[BOARD_HEIGHT * BOARD_WIDTH];
GemData current_gem;
int abi;
int max_eli, cur_eli;
Pos cur_pos, cur_tar;
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
      cur_pos = current_gem.pos;

      eli_data[i].pos = current_gem.pos;
      if(abi == ABI_CROSS) {
        check_cross(cur_pos, eli_data[i].tar, eli_data[i].cnt);
      }
      else if(abi == ABI_BOMB) {
        check_bomb(cur_pos, eli_data[i].tar, eli_data[i].cnt);
      }
      else if(abi == ABI_KILLSAME) {
        check_killsame(cur_pos, eli_data[i].tar, eli_data[i].cnt);
      }

      max_eli = 0;
      pos1 = {BOARD_HEIGHT, BOARD_WIDTH};
      for(int i=0; i<index; i++) {
        if(!check_inboard(eli_data[i].tar)) continue;
        cur_eli = eli_data[i].cnt;
        cur_pos = eli_data[i].pos;
        cur_tar = eli_data[i].tar;
        if(cur_eli > max_eli) {
          max_eli = cur_eli;
          pos1 = cur_pos;
          pos2 = cur_tar;
        }
      }
    }
    if(check_inboard(pos1)) return;
  }
    
  /*try to gen special*/
  
  /* rand one */
  for (int i = 0; i < BOARD_HEIGHT; i++) {
    for (int j = 0; j < BOARD_WIDTH; j++) {
      for(int k=0; k<2; k++) {
        // down then right
        cur_tar = {i+direc[k].x, j+direc[k].y};
        if(!check_inboard(cur_tar)) continue;
        swap(my_gameboard[i][j], my_gameboard[cur_tar.x][cur_tar.y]);
        bool result = check_my_eliminate();
        swap(my_gameboard[i][j], my_gameboard[cur_tar.x][cur_tar.y]);
        if(result) {
          output_my_gameboard();
          pos1 = {i, j};
          pos2 = cur_tar;
          return;
        }
      } 
    }
  }
  return;
}

