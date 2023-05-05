#include "splendor.hpp"
#include <bits/stdc++.h>
using namespace std;


Gem my_gameboard[BOARD_HEIGHT][BOARD_WIDTH];
Gem my_gameboard_backup[BOARD_HEIGHT][BOARD_WIDTH];
int type_list[GEM_CNT];
Pos direc[4] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}}; // down right up left


struct ElimData{
  Pos pos;
  Pos tar;
  int cnt;
  int abi_type;
};


void cout_pos(Pos pos, bool newline=false) {
  cout << pos.x << ", " << pos.y;
  if(newline) cout << '\n';
}

void cout_pos_tar(Pos pos, Pos tar) {
  cout_pos(pos);
  cout << " | ";
  cout_pos(tar, true);
}

void cout_abi(int abi) {
  if(abi == ABI_NULL) cout << "null" << '\n';
  else if(abi == ABI_NORMAL) cout << "normal" << '\n';
  else if(abi == ABI_BOMB) cout << "bomb" << '\n';
  else if(abi == ABI_KILLSAME) cout << "killsame" << '\n';
  else if(abi == ABI_CROSS) cout << "cross" << '\n';
}

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

bool check_my_eliminate() {
  for(int i=0; i<BOARD_HEIGHT; i++) {
    for(int j=0; j<BOARD_WIDTH; j++) {
      if(my_is_line({i, j}))
        return true;
    }
  }
  return false;
}

bool check_my_gameboard_eliminate() {
  for(int i=0; i<BOARD_HEIGHT; i++) {
    for(int j=0; j<BOARD_WIDTH; j++) {
      if(my_is_line({i, j}))
        return true;
    }
  }
  return false;
}

int my_check_line(Pos p) {
  int curr_gem = my_gameboard[p.x][p.y].type;
  int ret = 0;
  if (curr_gem == GEM_NULL) return ret;
  if (p.x != 0 && p.x != BOARD_HEIGHT && 
      curr_gem == my_gameboard[p.x - 1][p.y].type && 
      curr_gem == my_gameboard[p.x + 1][p.y].type)
    ret |= 1;
  if (p.y != 0 && p.y != BOARD_WIDTH && 
      curr_gem == my_gameboard[p.x][p.y - 1].type && 
      curr_gem == my_gameboard[p.x][p.y + 1].type)
    ret |= 2;
  return ret;
}

bool check_indeque(Pos pos) {
  if(0 <= pos.x && pos.x < BOARD_HEIGHT-5 
  && 0 <= pos.y && pos.y < BOARD_WIDTH-5) 
    return true;
  return false;
}

void backup_gameboard() {
  for(int i=0; i<BOARD_HEIGHT; i++) {
    for(int j=0; j<BOARD_WIDTH; j++) {
      my_gameboard_backup[i][j] = my_gameboard[i][j];
    }
  }
}

void recover_gameboard() {
  for(int i=0; i<BOARD_HEIGHT; i++) {
    for(int j=0; j<BOARD_WIDTH; j++) {
      my_gameboard[i][j] = my_gameboard_backup[i][j];
    }
  }
}

void check_special(Pos, Pos, int&);

void remove_gem(Pos pos) {
  my_gameboard[pos.x][pos.y].type = GEM_NULL;
  my_gameboard[pos.x][pos.y].ability = ABI_NULL;
}

void remove_gem_special(Pos pos, Pos elim_by, int &elim_cnt) {
  my_gameboard[pos.x][pos.y].type = GEM_NULL;
  if(my_gameboard[pos.x][pos.y].ability >= ABI_CROSS) {
    check_special(pos, elim_by, elim_cnt);
  }
  my_gameboard[pos.x][pos.y].ability = ABI_NULL;
  // output_my_gameboard();
}

void recover_gem_type(Pos pos, int type) {
  my_gameboard[pos.x][pos.y].type = type;
}

Pos tmp_tar, tmp_pos;
int cur_type;

void check_cross(Pos &pos, Pos &tar, int &elim_cnt) {
  int tmp_abi;
  // cout_pos_tar(pos, tar);
  cur_type = my_gameboard[pos.x][pos.y].type;
  
  if(cur_type != my_gameboard[tar.x][tar.y].type) {
    swap(my_gameboard[pos.x][pos.y], my_gameboard[tar.x][tar.y]);
    
    Pos tars[4] = {};
    int i = 0;
    for(int j=0; j<4; j++) {
      tmp_tar = {tar.x+direc[j].x, tar.y+direc[j].y};
      if((pos.x != tmp_tar.x || pos.y != tmp_tar.y)
      && check_inboard(tmp_tar)) {
        tars[i++] = tmp_tar;
      }
    }

    /*
    for(int i=0; i<3; i++) {
      cout_pos(tars[i]);
      cout << " | ";
    }
    cout << '\n';
    output_my_gameboard();
    */

    // + 可動
    if(check_my_gameboard_eliminate()
    && (my_is_line(tars[0])
    || my_is_line(tars[1])
    || my_is_line(tars[2]))) {
      remove_gem(tar);
      elim_cnt++;
      for(int i=0; i<BOARD_WIDTH; i++) {
        if(my_gameboard[tar.x][i].ability > ABI_NULL){
          tmp_abi = my_gameboard[tar.x][i].ability;
          remove_gem_special({tar.x, i}, pos, elim_cnt);
          if(tmp_abi == ABI_NORMAL)
            elim_cnt++;
        } 
      }
      for(int i=0; i<BOARD_HEIGHT; i++) {
        if(my_gameboard[i][tar.y].ability > ABI_NULL) {
          tmp_abi = my_gameboard[i][tar.y].ability;
          remove_gem_special({i, tar.y}, pos, elim_cnt);
          if(tmp_abi == ABI_NORMAL)
            elim_cnt++;
        }
      }
      return;
    }

    swap(my_gameboard[pos.x][pos.y], my_gameboard[tar.x][tar.y]);

    // 別人過來(中間)
    if(check_inboard({tar.x*2-pos.x, tar.y*2-pos.y})) {
      if(cur_type == my_gameboard[tar.x*2-pos.x][tar.y*2-pos.y].type) {
        // output_my_gameboard();
        int max_cnt;
        int tmp_cnt_cross[4] = {};
        if(tar.x == pos.x) {
          for(int i=0; i<3; i+=2) { // 0 2
            tmp_pos = {tar.x+direc[i].x, tar.y};
            if(cur_type == my_gameboard[tmp_pos.x][tmp_pos.y].type) {
            
              swap(my_gameboard[tar.x][tar.y], my_gameboard[tmp_pos.x][tmp_pos.y]);
              // output_my_gameboard();
              remove_gem(pos);
              tmp_cnt_cross[i/2]++;
              for(int j=0; j<BOARD_WIDTH; j++) {
                if(my_gameboard[pos.x][j].ability > ABI_NULL){
                  // cout << "cross: " << tmp_cnt_cross[i/2] << '\n';
                  tmp_abi = my_gameboard[pos.x][j].ability;
                  remove_gem_special({pos.x, j}, pos, tmp_cnt_cross[i/2]);
                  if(tmp_abi == ABI_NORMAL)
                    tmp_cnt_cross[i/2]++;
                } 
              }
              for(int j=0; j<BOARD_HEIGHT; j++) {
                if(my_gameboard[j][pos.y].ability > ABI_NULL) {
                  // cout << "cross: " << tmp_cnt_cross[i/2] << '\n';
                  tmp_abi = my_gameboard[j][pos.y].ability;
                  remove_gem_special({j, pos.y}, pos, tmp_cnt_cross[i/2]);
                  if(tmp_abi == ABI_NORMAL)
                    tmp_cnt_cross[i/2]++;
                }
              }
              recover_gameboard();
            }
          }
          max_cnt = 0;
          for(int i=0; i<3; i+=2) {
            if(tmp_cnt_cross[i/2] > max_cnt) {
              max_cnt = tmp_cnt_cross[i/2];
              // cout << "change1\n";
              pos = {tar.x+direc[i].x, tar.y};
            }
          }
        }
        else {
          for(int i=1; i<4; i+=2) { // 1 3
            tmp_pos = {tar.x, tar.y+direc[i].y};
            if(cur_type == my_gameboard[tmp_pos.x][tmp_pos.y].type) {
              // cout_pos(tmp_pos);
              // cout << " is good\n";
              swap(my_gameboard[tar.x][tar.y], my_gameboard[tmp_pos.x][tmp_pos.y]);
              tmp_cnt_cross[i/2]++;
              for(int j=0; j<BOARD_WIDTH; j++) {
                if(my_gameboard[pos.x][j].ability > ABI_NULL){
                  tmp_abi = my_gameboard[pos.x][j].ability;
                  remove_gem_special({pos.x, j}, pos, tmp_cnt_cross[(i-1)/2]);
                  if(tmp_abi == ABI_NORMAL)
                    tmp_cnt_cross[(i-1)/2]++;
                }
              }
              for(int j=0; j<BOARD_HEIGHT; j++) {
                if(my_gameboard[j][pos.y].ability > ABI_NULL) {
                  tmp_abi = my_gameboard[j][pos.y].ability;
                  remove_gem_special({j, pos.y}, pos, tmp_cnt_cross[(i-1)/2]);
                  if(tmp_abi == ABI_NORMAL)
                    tmp_cnt_cross[(i-1)/2]++;
                }
              }
              recover_gameboard();
            }
          }
          max_cnt = 0;
          for(int i=1; i<4; i+=2) {
            if(tmp_cnt_cross[(i-1)/2] > max_cnt) {
              max_cnt = tmp_cnt_cross[(i-1)/2];
              // cout << "change3\n";
              pos = {tar.x, tar.y+direc[i].y};
            }
          }
        }
        if(max_cnt > 0) {
          elim_cnt = max_cnt;
          return;
        }
        // cout << "set to ";
        // cout_pos_tar(pos, tar);
      }
    }

    // 別人過來(末端)(靠近pos)
    for(int i=0; i<4; i++) {
      tmp_tar = {tar.x+direc[i].x, tar.y+direc[i].y};
      if(check_inboard(tmp_tar) && !(tmp_tar.x == pos.x && tmp_tar.y == pos.y)) {
        swap(my_gameboard[tar.x][tar.y], my_gameboard[tmp_tar.x][tmp_tar.y]);
        if(my_is_line(pos)) {
          remove_gem(pos);
          elim_cnt++;
          for(int i=0; i<BOARD_WIDTH; i++) {
            if(my_gameboard[pos.x][i].ability > ABI_NULL){
              tmp_abi = my_gameboard[pos.x][i].ability;
              remove_gem_special({pos.x, i}, pos, elim_cnt);
              if(tmp_abi == ABI_NORMAL)
                elim_cnt++;
            }
          }
          for(int i=0; i<BOARD_HEIGHT; i++) {
            if(my_gameboard[i][pos.y].ability > ABI_NULL) {
              tmp_abi = my_gameboard[i][pos.y].ability;
              remove_gem_special({i, pos.y}, pos, elim_cnt);
              if(tmp_abi == ABI_NORMAL)
                elim_cnt++;
            }
          }
          pos = tmp_tar;
          return;
        }
        swap(my_gameboard[tar.x][tar.y], my_gameboard[tmp_tar.x][tmp_tar.y]);
      }
    }
  }
  // 別人過來(末端)(靠近tar)
  tmp_pos = {tar.x*2-pos.x, tar.y*2-pos.y};
  for(int i=0; i<4; i++) {
    tmp_tar = {tmp_pos.x+direc[i].x, tmp_pos.y+direc[i].y};
    if(check_inboard(tmp_tar) && !(tmp_tar.x == tar.x && tmp_tar.y == tar.y)) {
      swap(my_gameboard[tmp_pos.x][tmp_pos.y], my_gameboard[tmp_tar.x][tmp_tar.y]);
      
      // availible
      if(my_is_line(tar)) {
        remove_gem(pos);
        elim_cnt++;
        for(int i=0; i<BOARD_WIDTH; i++) {
          if(my_gameboard[pos.x][i].ability > ABI_NULL){
            tmp_abi = my_gameboard[pos.x][i].ability;
            remove_gem_special({pos.x, i}, pos, elim_cnt);
            if(tmp_abi == ABI_NORMAL)
              elim_cnt++;
          }
        }
        for(int i=0; i<BOARD_HEIGHT; i++) {
          if(my_gameboard[i][pos.y].ability > ABI_NULL) {
            tmp_abi = my_gameboard[i][pos.y].ability;
            remove_gem_special({i, pos.y}, pos, elim_cnt);
            if(tmp_abi == ABI_NORMAL)
              elim_cnt++;
          }
        }
        // cout << "change2\n";
        pos = tmp_pos;
        tar = tmp_tar;
        return;
      }
      swap(my_gameboard[tmp_pos.x][tmp_pos.y], my_gameboard[tmp_tar.x][tmp_tar.y]);
    }
  }
}

void check_cross_call(Pos pos, int &elim_cnt) {
  int tmp_abi;
  remove_gem(pos);
  elim_cnt++;
  for(int i=0; i<BOARD_WIDTH; i++) {
    if(my_gameboard[pos.x][i].ability > ABI_NULL){
      // cout << "cross: " << elim_cnt << '\n';
      tmp_abi = my_gameboard[pos.x][i].ability;
      remove_gem_special({pos.x, i}, pos, elim_cnt);
      if(tmp_abi == ABI_NORMAL)
        elim_cnt++;
    } 
  }
  for(int i=0; i<BOARD_HEIGHT; i++) {
    if(my_gameboard[i][pos.y].ability > ABI_NULL) {
      // cout << "cross: " << elim_cnt << '\n';
      tmp_abi = my_gameboard[i][pos.y].ability;
      remove_gem_special({i, pos.y}, pos, elim_cnt);
      if(tmp_abi == ABI_NORMAL)
        elim_cnt++;
    }
  }
}
void check_bomb(Pos pos, Pos &tar, int &elim_cnt) {
  int tmp_abi;
  remove_gem(pos);
  elim_cnt++;
  for(int i=0; i<4; i++) {
    tar = {pos.x+direc[i].x, pos.y+direc[i].y};
    if(check_inboard(tar)) break;
  }
  for(int i=pos.x-2; i<=pos.x+2; i++) {
    for(int j=pos.y-2; j<=pos.y+2; j++) {
      if(check_inboard({i, j})) {
        if(my_gameboard[i][j].ability > ABI_NULL) {
          // cout << "bomb_cnt: "<< elim_cnt << '\n';
          tmp_abi = my_gameboard[i][j].ability;
          remove_gem_special({i, j}, pos, elim_cnt);
          if(tmp_abi == ABI_NORMAL)
            elim_cnt++;
          // else { cout_abi(tmp_abi); }
        }
      }
    }
  }
}

void check_killsame(Pos pos, Pos elim_by, int &elim_cnt) {
  int tmp_abi;
  remove_gem(pos);
  elim_cnt++;
  /*elim by z*/
  if(my_gameboard[elim_by.x][elim_by.y].ability == ABI_KILLSAME) {
    elim_cnt = -100;
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
        tmp_abi = my_gameboard[i][j].ability;
        remove_gem_special({i, j}, pos, elim_cnt);
        if(tmp_abi == ABI_NORMAL)
          elim_cnt++;
      }
    }
  }
  return;
}

void check_special(Pos pos, Pos elim_by, int &elim_cnt) {
  if(my_gameboard[pos.x][pos.y].ability == ABI_BOMB) check_bomb(pos, elim_by, elim_cnt);
  else if(my_gameboard[pos.x][pos.y].ability == ABI_KILLSAME) check_killsame(pos, elim_by, elim_cnt);
  else if(my_gameboard[pos.x][pos.y].ability == ABI_CROSS) check_cross_call(pos, elim_cnt);
}

deque<deque<int>> five_times_five_board(5);

void output_deque() {
  cout << "---------\n";
  for(auto i: five_times_five_board) {
    for(auto j: i) {
        string color = get_color(j);
        string ansi = "\x1b[" + color + "m";
        cout << ansi << "x ";
    }
    cout << "\x1b[0m";
    cout << '\n';
  }
  cout << "---------\n";
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

void clear_deque() {
  for(int i=0; i<BOARD_HEIGHT-5; i++) {
    five_times_five_board[i].clear();
  }
}

int my_check_line_deque(Pos p) {
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

void output_elimi_tags() {
  cout << "---------\n";
  for(int i=0; i<5; i++) {
    for(int j=0; j<5; j++) {
      cout << my_elimi_tags[i][j] << ' ';
    }
    cout << '\n';
  }
  cout << "---------\n";
}

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
  my_special_dfs(pos, &data);
  int ret = ABI_NORMAL;
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
  // reset my_visited for my_special_dfs
  for (int i = 0; i < BOARD_HEIGHT-5; ++i) {
    for (int j = 0; j < BOARD_WIDTH-5; ++j) {
      my_visited[i][j] = 0;
      my_elimi_tags[i][j] = 0;
    }
  }
  // cout << "reseted visited" << '\n';
  // tag the gems should be eliminate
  for (int i = 0; i < BOARD_HEIGHT-5; ++i) {
    for (int j = 0; j < BOARD_WIDTH-5; ++j) {
      int check_ret = my_check_line_deque({i, j});

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
  // output_deque();
  // output_elimi_tags();
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

bool check_adjacent(Pos a, Pos b) {
  return dist_sq(a, b) == 1?true:false;
}




void ai(Pos& pos1, Pos& pos2) {
  // cout << "ai" << endl;
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
  // init vars
  int indx = 0;
  init_typelist();
  bool have_special = false;
  GemData special[BOARD_HEIGHT*BOARD_WIDTH] = {};
  int max_eli;
  Pos cur_tar;

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
    // cout << "special! counted: " << indx << '\n';
    // init vars
    ElimData eli_data[BOARD_HEIGHT * BOARD_WIDTH] = {};
    GemData current_gem;
    int abi, cur_eli;
    Pos cur_pos, temp_pos;
    // for each special(GemData)
    for(int i=0; i<indx; i++) {
      current_gem = special[i];
      abi = current_gem.gem.ability;
      cur_pos = current_gem.pos;
      eli_data[i].pos = current_gem.pos;
      // cout << i << " got pos ";
      // cout_pos(cur_pos, true);
      Pos tar, poslst[4], tarlst[4];
      int tmp_cnt[5] = {};
      backup_gameboard();
      if(abi == ABI_CROSS) {
        temp_pos = cur_pos;
        for(int j=0; j<4; j++) {
          cur_pos = temp_pos;
          tar = {cur_pos.x+direc[j].x, cur_pos.y+direc[j].y};
          // cout << "tar: ";
          // cout_pos(tar, true);
          if(check_inboard(tar)) {
            // cout << "check :";
            // cout_pos_tar(cur_pos, tar);
            check_cross(cur_pos, tar, tmp_cnt[j]);
            recover_gameboard();
            poslst[j] = cur_pos;
            tarlst[j] = tar;
          }
        }
        int max_cnt = 0;
        for(int j=0; j<4; j++) {
          if(tmp_cnt[j] > max_cnt) {
            max_cnt = tmp_cnt[j];
            eli_data[i].pos = poslst[j];
            eli_data[i].tar = tarlst[j];
            // cout_pos_tar(eli_data[i].pos, eli_data[i].tar);
            // cout << tmp_cnt[j] << '\n';
          }
        }
        eli_data[i].cnt = max_cnt;
      }
      else if(abi == ABI_BOMB) {
        check_bomb(cur_pos, eli_data[i].tar, eli_data[i].cnt);
        recover_gameboard();
      }
      else if(abi == ABI_KILLSAME) {
        for(int j=0; j<4; j++) {
          tar = {cur_pos.x+direc[j].x, cur_pos.y+direc[j].y};
          if(check_inboard(tar)) {
            check_killsame(cur_pos, tar, tmp_cnt[j]);
            recover_gameboard();
          }
        }
        int max_cnt = 0;
        for(int j=0; j<4; j++) {
          if(tmp_cnt[j] > max_cnt) {
            max_cnt = tmp_cnt[j];
            eli_data[i].tar = {cur_pos.x+direc[j].x, cur_pos.y+direc[j].y};
          }
        }
        eli_data[i].cnt = max_cnt;
      }
    }
    max_eli = 0;
    pos1 = {BOARD_HEIGHT, BOARD_WIDTH};
    for(int i=0; i<indx; i++) {
      if(!check_inboard(eli_data[i].tar)) continue;
      // cout_pos_tar(eli_data[i].pos, eli_data[i].tar);
      // cout << eli_data[i].cnt << '\n';
      cur_eli = eli_data[i].cnt;
      cur_pos = eli_data[i].pos;
      cur_tar = eli_data[i].tar;
      if(cur_eli > max_eli) {
        max_eli = cur_eli;
        pos1 = cur_pos;
        pos2 = cur_tar;
      }
    }
    if(check_inboard(pos1) && check_adjacent(pos1, pos2))
      return;
    // else
      // cout << "\nfrom ai: " << pos1.x << ' ' << pos1.y << '\n' << pos2.x << ' ' << pos2.y << '\n';
  }
    
  /*try to gen special: Q > Z > + */
  // cout << "searching special" << '\n';
  // init vars
  indx = 0;
  bool duplicate = false;
  clear_deque();
  copy_board_to_deque(0, 0);
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
        for(int k=0; k<2; k++) {
          Pos tar = {pos.x+direc[k].x, pos.y+direc[k].y};
          if(check_indeque(tar)) {
            // cout << "\ncheck deque" << '\n';
            swap(five_times_five_board[pos.x][pos.y], five_times_five_board[tar.x][tar.y]);
            int special_type = my_elimi_special();
            swap(five_times_five_board[pos.x][pos.y], five_times_five_board[tar.x][tar.y]);
            duplicate = false;
            for(int i=0; i<indx; i++) {
              if(special_and_poses[i].abi_type == special_type
              && special_and_poses[i].pos.x == pos.x+shift_down
              && special_and_poses[i].pos.y == pos.y+shift_right
              && special_and_poses[i].tar.x == tar.x+shift_down
              && special_and_poses[i].tar.y == tar.y+shift_right)
                duplicate = true;
            }
            if(special_type >= ABI_CROSS && !duplicate) {
              special_and_poses[indx].abi_type = special_type;
              special_and_poses[indx].pos = {pos.x+shift_down, pos.y+shift_right};
              special_and_poses[indx].tar = {tar.x+shift_down, tar.y+shift_right};
              ability_cnt[special_type]++;
              // cout_pos_tar(special_and_poses[indx].pos, special_and_poses[indx].tar);
              // cout_abi(special_and_poses[indx].abi_type);
              // cout << ability_cnt[ABI_BOMB] << ' ' << ability_cnt[ABI_KILLSAME] << ' ' << ability_cnt[ABI_CROSS] << '\n';
              if(special_type == ABI_BOMB) {
                brk = true;
                if(2 <= shift_down && shift_down <= 3
                && 2 <= shift_right && shift_right <= 3)
                  special_and_poses[indx].cnt = 1;
                indx++;
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
      clear_deque();
      shift_down++;
      copy_board_to_deque(shift_down, 0);
    }
    // output_deque();
  }

  // cout << ability_cnt[ABI_BOMB] << ' ' << ability_cnt[ABI_KILLSAME] << ' ' << ability_cnt[ABI_CROSS] << '\n';
  if(indx > 0) {
    // can gen special
    // cout << "gennable\n";
    ElimData special_pos;
    if(ability_cnt[ABI_BOMB] > 0) {
      // cout << "bomb\n";
      for(int i=0; i<indx; i++) {
        special_pos = special_and_poses[i];
        if(special_pos.abi_type == ABI_BOMB) {
          pos1 = special_pos.pos;
          pos2 = special_pos.tar;
          if(special_pos.cnt == 1)
            break;
        }
      }
    }
    else if(ability_cnt[ABI_KILLSAME] > 0) {
      // cout << "killsame\n";
      for(int i=0; i<indx; i++) {
        special_pos = special_and_poses[i];
        if(special_pos.abi_type == ABI_KILLSAME) {
          pos1 = special_pos.pos;
          pos2 = special_pos.tar;
          break;
        }
      }
    }
    else {
      // cout << "cross\n";
      for(int i=0; i<indx; i++) {
        special_pos = special_and_poses[i];
        if(special_pos.abi_type == ABI_CROSS) {
          pos1 = special_pos.pos;
          pos2 = special_pos.tar;
          break;
        }
      }
    }

    if(check_adjacent(pos1, pos2))
      return;
    //else
    //  cout << "\nfrom ai: " << pos1.x << ' ' << pos1.y << '\n' << pos2.x << ' ' << pos2.y << '\n';
  }

  /* best one */
  // cout << "\nfind the best one" << '\n';
  ElimData elims[BOARD_HEIGHT*BOARD_WIDTH*2] = {};
  indx = 0;
  for (int i = 0; i < BOARD_HEIGHT; i++) {
    for (int j = 0; j < BOARD_WIDTH; j++) {
      for(int k=0; k<2; k++) {
        // down then right
        cur_tar = {i+direc[k].x, j+direc[k].y};
        if(!check_inboard(cur_tar)) continue;
        swap(my_gameboard[i][j], my_gameboard[cur_tar.x][cur_tar.y]);
        if(check_my_eliminate()) {
          elims[indx].pos = {i, j};
          elims[indx].tar = cur_tar;
          for (int i = 0; i < BOARD_HEIGHT; ++i) {
            for (int j = 0; j < BOARD_WIDTH; ++j) {
              int check_ret = my_check_line({i, j});
              if (!check_ret) {
                continue;
              }
              if (check_ret & 1) {
                // verti
                elims[indx].cnt += 2;
              }
              if (check_ret & 2) {
                // hori
                elims[indx].cnt += 2;
              }
              elims[indx].cnt++;
            }
          }
          /*
          cout << '\n';
          cout_pos({i, j});
          cout << " | " << elims[indx].cnt << '\n';
          output_my_gameboard();
          */
          indx++;
        }
        /*
        else {
          output_my_gameboard();
        }
        */
        swap(my_gameboard[i][j], my_gameboard[cur_tar.x][cur_tar.y]);
      } 
    }
  }

  max_eli = 0;
  ElimData pos_cnt;
  /*
  for(int i=0; i<indx; i++) {
    cout_pos_tar(elims[i].pos, elims[i].tar);
    cout << "cnt: " << elims[i].cnt << '\n';
  }
  */
  for(int i=0; i<indx; i++) {
    pos_cnt = elims[i];
    if(pos_cnt.cnt > max_eli) {
      max_eli = pos_cnt.cnt;
      pos1 = pos_cnt.pos;
      pos2 = pos_cnt.tar;
    }
  }
  if(check_adjacent(pos1, pos2)) {
    // terminate();
    return;
  }
  else {
    while(true) {
      pos1.x = gen_rand() % BOARD_HEIGHT;
      pos1.y = gen_rand() % BOARD_WIDTH;
      pos2.x = gen_rand() % BOARD_HEIGHT;
      pos2.y = gen_rand() % BOARD_WIDTH;
      swap(my_gameboard[pos1.x][pos1.y], my_gameboard[pos2.x][pos2.y]);
      if(check_eliminate) break;
      swap(my_gameboard[pos1.x][pos1.y], my_gameboard[pos2.x][pos2.y]);
    }
  }
}
