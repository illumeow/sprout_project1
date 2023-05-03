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
  int abi_type;
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

bool check_indeque(Pos pos) {
  if(0 <= pos.x && pos.x < BOARD_HEIGHT-5 
  && 0 <= pos.y && pos.y < BOARD_WIDTH-5) 
    return true;
  return false;
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

bool my_is_line(Pos p) {
  int curr_gem = my_gameboard[p.x][p.y].type;
  bool ret = false;
  if (curr_gem == GEM_NULL) return ret;
  if (p.x > 0 && p.x < BOARD_HEIGHT-1 && 
      curr_gem == my_gameboard[p.x-1][p.y].type && 
      curr_gem == my_gameboard[p.x+1][p.y].type)
    ret = true;
  if (p.y > 0 && p.y < BOARD_WIDTH-1 && 
      curr_gem == my_gameboard[p.x][p.y-1].type && 
      curr_gem == my_gameboard[p.x][p.y+1].type)
    ret = true;
  return ret;
}

bool check_my_eliminate(Pos left_up) {
  for(int i=left_up.x; i<left_up.x+5; i++) {
    for(int j=left_up.y; j<left_up.y+5; j++) {
      if(my_is_line({i, j}))
        return true;
    }
  }
  return false;
}


deque<deque<int>> five_times_five_board(5);

int my_check_line(Pos p) {
  int curr_gem = five_times_five_board[p.x][p.y];
  int ret = 0;
  if (curr_gem == GEM_NULL) return ret;
  if (p.x != 0 && p.x != BOARD_HEIGHT-6 && 
      curr_gem == five_times_five_board[p.x - 1][p.y] && 
      curr_gem == five_times_five_board[p.x + 1][p.y])
    ret |= 1;
  if (p.y != 0 && p.y != BOARD_WIDTH-6 && 
      curr_gem == five_times_five_board[p.x][p.y - 1] && 
      curr_gem == five_times_five_board[p.x][p.y + 1])
    ret |= 2;
  return ret;
}

bool my_visited[BOARD_HEIGHT-5][BOARD_WIDTH-5];
bool my_elimi_tags[BOARD_HEIGHT-5][BOARD_WIDTH-5];
int my_success_line[BOARD_HEIGHT-5][BOARD_WIDTH-5] = {};

void my_special_dfs(Pos pos, ElimiData *data) {
  // cout << "start from: " << pos.x << ", " << pos.y << '\n';
  my_visited[pos.x][pos.y] = 1;
  for (int i = 0; i < 4; ++i) {
    Pos tar = {pos.x + direc[i].x, pos.y + direc[i].y};
    if ( check_indeque(tar) && !my_visited[tar.x][tar.y]
      && five_times_five_board[pos.x][pos.y] == five_times_five_board[tar.x][tar.y]
      && my_elimi_tags[tar.x][tar.y])
      my_special_dfs(tar, data);
  }
  data->total_elimi++;
  data->mid_elimi += my_success_line[pos.x][pos.y];
  my_success_line[pos.x][pos.y] = 0;
}


int check_gen_special(Pos pos) {
  ElimiData data = {0, 0, 0};
  cout << "start dfs" << '\n';
  my_special_dfs(pos, &data);
  // cout << "end dfs" << '\n';
  int ret = ABI_NULL;
  if (data.total_elimi == 4) {
    ret = ABI_CROSS;
  }
  else if (data.total_elimi > 4) {
    if (data.total_elimi - data.mid_elimi == 2)
      ret = ABI_KILLSAME;
    else
      ret = ABI_BOMB;
  }
  
  return ret;
}

int my_elimi_special() {
  // reset visited array for eli_dfs
  for (int i = 0; i < BOARD_HEIGHT-5; ++i) {
    for (int j = 0; j < BOARD_WIDTH-5; ++j) {
      my_visited[i][j] = 0;
    }
  }
  // cout << "reseted visited" << '\n';
  // tag the gems should be eliminate
  for (int i = 0; i < BOARD_HEIGHT-5; ++i) {
    for (int j = 0; j < BOARD_WIDTH-5; ++j) {
      int check_ret = my_check_line({i, j});

      if (!check_ret) {
        continue;
      }

      if (check_ret & 1) {
        my_elimi_tags[i - 1][j] = 1;
        my_elimi_tags[i + 1][j] = 1;
      }
      if (check_ret & 2) {
        my_elimi_tags[i][j - 1] = 1;
        my_elimi_tags[i][j + 1] = 1;
      }
      my_success_line[i][j] += (check_ret + 1) >> 1;
      my_elimi_tags[i][j] = 1;
    }
  }
  // cout << "gem taged" << '\n';
  // check if five_times_five_board can gen any special gem
  for (int i = 0; i < BOARD_HEIGHT-5; ++i) {
    for (int j = 0; j < BOARD_WIDTH-5; ++j) {
      if(my_success_line[i][j]) {
        int abi_type = check_gen_special({i, j});
        return abi_type;
      }
    }
  }
  return ABI_NULL;
}

void copy_board_to_deque(int sj, int sk) {
  int i = 0;
  for(int j=sj; j<sj+5; j++) {
    for(int k=sk; k<sk+5; k++) {
      five_times_five_board[i].push_back(my_gameboard[j][k].type);
    }
    i++;
  }
}

void output_deque() {
  cout << '\n';
  for(auto i: five_times_five_board) {
    for(auto j: i) {
      cout << j << ' ';
    }
    cout << '\n';
  }
}

bool check_adjacent(Pos a, Pos b) {
  return dist_sq(a, b) == 1?true:false;
}


bool have_special = false;
GemData special[BOARD_HEIGHT*BOARD_WIDTH];
int indx;
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
  indx = 0;
  init_typelist();
  // sweep over the gameboard and record special(also bool) at the same time
  for(int i=0; i<BOARD_HEIGHT; i++) {
    for(int j=0; j<BOARD_WIDTH; j++) {
        Gem tmp = get_gem({i, j});
        my_gameboard[i][j] = tmp;
        type_list[tmp.type]++;
        if(tmp.ability >= ABI_CROSS) {
          have_special = true;
          special[indx++] = {{i, j}, {tmp.type, tmp.ability}}; 
        }
    }
  }

  // output_my_gameboard();

  /* special comes first */
  if(have_special) {
    cout << "special!" << '\n';
    // for each special(GemData)
    for(int i=0; i<indx; i++) {
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
      for(int i=0; i<indx; i++) {
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
    if(check_inboard(pos1) && check_adjacent(pos1, pos2))
      return;
    else
      cout << "\nfrom ai: " << pos1.x << ' ' << pos1.y << '\n' << pos2.x << ' ' << pos2.y << '\n';
  }
    
  /*try to gen special: Q > Z > + */
  cout << "\nsearching special" << '\n';
  // init deque, indx, map
  indx = 0;
  copy_board_to_deque(0, 0);
  output_deque();
  map<int, int> ability_cnt = {
    {ABI_CROSS, 0},
    {ABI_BOMB, 0},
    {ABI_KILLSAME, 0}
  };

  int shift_right = 0, shift_down = 0;
  ElimData special_and_poses[BOARD_HEIGHT*BOARD_WIDTH*2] = {};
  // check deque
  bool brk = false;
  for(int c=0; c<(BOARD_HEIGHT-4)*(BOARD_WIDTH-4); c++) {
    if(brk) break;
    for(int i=0; i<BOARD_HEIGHT-5; i++) {
      if(brk) break;
      for(int j=0; j<BOARD_WIDTH-5; j++) {
        if(brk) break;
        Pos pos = {i, j};
        for(int k=0; k<4; k++) {
          Pos tar = {pos.x+direc[k].x, pos.y+direc[k].y};
          if(check_indeque(tar)) {
            // cout << "\ncheck deque" << '\n';
            swap(five_times_five_board[pos.x][pos.y], five_times_five_board[tar.x][tar.y]);
            int special_type = my_elimi_special();
            // cout << "alive" << '\n';
            swap(five_times_five_board[pos.x][pos.y], five_times_five_board[tar.x][tar.y]);
            
            if(special_type >= ABI_CROSS) {
              special_and_poses[indx].abi_type = special_type;
              special_and_poses[indx].pos = {pos.x+shift_down, pos.y+shift_right};
              special_and_poses[indx].tar = {tar.x+shift_down, tar.y+shift_right};
              ability_cnt[special_type]++;
              if(special_type == ABI_BOMB) {
                brk = true;
                if(2 <= shift_down && shift_down <= 3
                && 2 <= shift_right && shift_right <= 3)
                  special_and_poses[indx].cnt = 1;
                break;
              }
              indx++;
            }
          }
        }
      } 
    }
    // move deque
    if(c%6 < 5) {
      // shift right
      shift_right++;
      for(int i=0; i<BOARD_HEIGHT-5; i++) {
        five_times_five_board[i].pop_front();
      }
      for(int i=0; i<BOARD_HEIGHT-5; i++) {
        five_times_five_board[i].push_back(my_gameboard[i+shift_down][BOARD_WIDTH-6+shift_right].type);
      }
    }
    else {
      // shift down
      shift_right = 0;
      for(int i=0; i<BOARD_HEIGHT-5; i++) {
        five_times_five_board[i].clear();
      }
      shift_down++;
      copy_board_to_deque(shift_down, 0);
    }
  }

  if(indx > 0) {
    // can gen special
    ElimData special_pos;
    if(ability_cnt[ABI_BOMB] > 0) {
      for(int i=0; i<indx; i++) {
        special_pos = special_and_poses[indx];
        if(special_pos.abi_type == ABI_BOMB) {
          pos1 = special_pos.pos;
          pos2 = special_pos.tar;
          if(special_pos.cnt == 1)
            break;
        }
      }
    }
    else if(ability_cnt[ABI_KILLSAME] > 0) {
      for(int i=0; i<indx; i++) {
        special_pos = special_and_poses[indx];
        if(special_pos.abi_type == ABI_KILLSAME) {
          pos1 = special_pos.pos;
          pos2 = special_pos.tar;
          break;
        }
      }
    }
    else {
      for(int i=0; i<indx; i++) {
        special_pos = special_and_poses[indx];
        if(special_pos.abi_type == ABI_CROSS) {
          pos1 = special_pos.pos;
          pos2 = special_pos.tar;
          break;
        }
      }
    }

    if(check_adjacent(pos1, pos2))
      return;
    else
      cout << "\nfrom ai: " << pos1.x << ' ' << pos1.y << '\n' << pos2.x << ' ' << pos2.y << '\n';
  }

  /* best one */
  cout << "\nfind the best one" << '\n';
  ElimData elims[BOARD_HEIGHT*BOARD_WIDTH*2] = {};
  indx = 0;
  for (int i = 0; i < BOARD_HEIGHT; i++) {
    for (int j = 0; j < BOARD_WIDTH; j++) {
      for(int k=0; k<2; k++) {
        // down then right
        cur_tar = {i+direc[k].x, j+direc[k].y};
        if(!check_inboard(cur_tar)) continue;
        swap(my_gameboard[i][j], my_gameboard[cur_tar.x][cur_tar.y]);
        bool result = check_my_eliminate({i-2, j-2});
        if(result) {
          elims[indx].pos = {i, j};
          elims[indx].tar = cur_tar;
          for (int i = 0; i < BOARD_HEIGHT-5; ++i) {
            for (int j = 0; j < BOARD_WIDTH-5; ++j) {
              int check_ret = my_check_line({i, j});
              if (!check_ret) {
                continue;
              }
              if (check_ret & 1) {
                elims[indx].cnt += 2;
              }
              if (check_ret & 2) {
                elims[indx].cnt += 2;
              }
              elims[indx].cnt++;
            }
          }
        }
        swap(my_gameboard[i][j], my_gameboard[cur_tar.x][cur_tar.y]);
        indx++;
      } 
    }
  }

  max_eli = 0;
  ElimData pos_cnt;
  for(int i=0; i<indx; i++) {
    pos_cnt = elims[indx];
    if(pos_cnt.cnt > max_eli) {
      max_eli = pos_cnt.cnt;
      pos1 = pos_cnt.pos;
      pos2 = pos_cnt.tar;
    }
  }
  if(check_adjacent(pos1, pos2)) 
    return;
  else {
    for(int i=0; i<indx; i++) {
      cout << elims[i].cnt << ' ';
    }
    cout << "\nfrom ai: " << pos1.x << ' ' << pos1.y << '\n' << pos2.x << ' ' << pos2.y << '\n';
    ai(pos1, pos2);
  }
}

