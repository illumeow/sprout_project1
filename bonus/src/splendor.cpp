#include "splendor.hpp"
#include <random>
#include <thread>
#include <chrono>

using namespace std;

Gem gameboard[BOARD_HEIGHT][BOARD_WIDTH];  // 10x10
bool moved_tags[BOARD_HEIGHT][BOARD_WIDTH];
bool elimi_tags[BOARD_HEIGHT][BOARD_WIDTH];
bool visited[BOARD_HEIGHT][BOARD_WIDTH];
int success_line[BOARD_HEIGHT][BOARD_WIDTH] = {};
Pos dir[4] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}}; // up, right, down, left
static int step_remained = STEP_LIMIT;
static int step_used = 0;
static int player_score = 0;
static int best_step = INT32_MAX;
static int best_score = 0;
mt19937 mt(RND_SEED);

void apply_special(Pos, Pos);

Gem get_gem(Pos pos) {
  return gameboard[pos.x][pos.y];
}

int check_line(Pos p) {
  int curr_gem = gameboard[p.x][p.y].type;
  int ret = 0;
  if (curr_gem == GEM_NULL) return ret;
  if (p.x != 0 and p.x != BOARD_HEIGHT - 1 and 
      curr_gem == gameboard[p.x - 1][p.y].type and 
      curr_gem == gameboard[p.x + 1][p.y].type)
    ret |= 1;
  if (p.y != 0 and p.y != BOARD_WIDTH - 1 and 
      curr_gem == gameboard[p.x][p.y - 1].type and 
      curr_gem == gameboard[p.x][p.y + 1].type)
    ret |= 2;
  return ret;
}

bool check_inboard(Pos t) {
  // TODO: Task 1-1
  if(0 <= t.x && t.x < BOARD_HEIGHT 
  && 0 <= t.y && t.y < BOARD_WIDTH) 
    return true;
  return false;
}

bool is_line(Pos p) {
  int curr_gem = gameboard[p.x][p.y].type;
  bool ret = false;
  if (curr_gem == GEM_NULL) return ret;
  if (p.x != 0 && p.x != BOARD_HEIGHT-1 && 
      curr_gem == gameboard[p.x-1][p.y].type && 
      curr_gem == gameboard[p.x+1][p.y].type)
    ret = true;
  if (p.y != 0 && p.y != BOARD_WIDTH-1 && 
      curr_gem == gameboard[p.x][p.y-1].type && 
      curr_gem == gameboard[p.x][p.y+1].type)
    ret = true;
  return ret;
}

bool check_eliminate(Pos *pos) {
  // TODO: Task 1-2
  for(int i=0; i<BOARD_HEIGHT; i++) {
    for(int j=0; j<BOARD_WIDTH; j++) {
      if(is_line({i, j})) {
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
  // TODO: Task 1-3
  // a or b not inboard
  if(!check_inboard(a) || !check_inboard(b)) return false;
  // not adjacent
  if(dist_sq(a, b) > 1) return false;
  // Q, z 不能替換
  if ((gameboard[a.x][a.y].ability == ABI_BOMB &&
      gameboard[b.x][b.y].ability == ABI_KILLSAME) ||
      (gameboard[b.x][b.y].ability == ABI_BOMB &&
      gameboard[a.x][a.y].ability == ABI_KILLSAME)) 
    return false;
  // Q, z, %不須連線
  if (gameboard[a.x][a.y].ability == ABI_BOMB ||
      gameboard[a.x][a.y].ability == ABI_KILLSAME ||
      gameboard[b.x][b.y].ability == ABI_BOMB ||
      gameboard[b.x][b.y].ability == ABI_KILLSAME ||
      gameboard[a.x][a.y].ability == ABI_PLANE ||
      gameboard[b.x][b.y].ability == ABI_PLANE) 
    return true;
  // swap(a, b) in gameboard then just check_eliminate(nullptr) and swap it back
  swap(gameboard[a.x][a.y], gameboard[b.x][b.y]);
  bool result = check_eliminate(nullptr);
  swap(gameboard[a.x][a.y], gameboard[b.x][b.y]);
  return result;
}

void elim_gem_special(Pos pos, Pos tar) {
  elimi_tags[pos.x][pos.y] = 1;
  if(gameboard[pos.x][pos.y].ability >= ABI_CROSS) 
    apply_special(pos, tar);
}

void apply_bomb(Pos pos) {
  // TODO: Task 2-1
  elimi_tags[pos.x][pos.y] = 1;
  gameboard[pos.x][pos.y].ability = ABI_NORMAL;
  for(int i=pos.x-2; i<=pos.x+2; i++) {
    for(int j=pos.y-2; j<=pos.y+2; j++) {
      if(check_inboard({i, j})) {
        if(gameboard[i][j].ability == ABI_KILLSAME) {
          gameboard[pos.x][pos.y].ability = ABI_BOMB;
        }
        elim_gem_special({i, j}, pos);
      }
    }
  }
  gameboard[pos.x][pos.y].ability = ABI_BOMB;
}

void apply_killsame(Pos pos, Pos tar) {
  // TODO: Task 2-2
  elimi_tags[pos.x][pos.y] = 1;
  gameboard[pos.x][pos.y].ability = ABI_NORMAL;
  int tartype;
  if(gameboard[tar.x][tar.y].ability == ABI_BOMB) {
    cout << "elim by bomb at " << tar.x << ", " << tar.y << '\n';
    gameboard[tar.x][tar.y].ability = ABI_NORMAL;
    tartype = gen_rand_type();
  }
  else {
    cout << "elim by " << gameboard[tar.x][tar.y].ability << " at " <<  tar.x << ", " << tar.y << '\n';
    tartype = gameboard[tar.x][tar.y].type;
  }
  cout << tartype << '\n';
  draw_board(1,0,100);
  for(int i=0; i<BOARD_HEIGHT; i++) {
    for(int j=0; j<BOARD_WIDTH; j++) {
        if(gameboard[i][j].type == tartype) 
          elim_gem_special({i, j}, pos);
    }
  }
  draw_board(1, 0, 100);
  gameboard[pos.x][pos.y].ability = ABI_KILLSAME;
}

void apply_cross(Pos pos) {
  // TODO: Task 2-3
  elimi_tags[pos.x][pos.y] = 1;
  gameboard[pos.x][pos.y].ability = ABI_NORMAL;
  for(int i=0; i<BOARD_WIDTH; i++) elim_gem_special({pos.x, i}, pos);
  for(int i=0; i<BOARD_HEIGHT; i++) elim_gem_special({i, pos.y}, pos);
  gameboard[pos.x][pos.y].ability = ABI_CROSS;
}

void apply_plane(Pos pos) {
  elimi_tags[pos.x][pos.y] = 1;
  gameboard[pos.x][pos.y].ability = ABI_NORMAL;
  Pos center = {gen_rand()%8 + 1, gen_rand()%8 + 1};
  cout << "center: " << center.x << ", " << center.y << '\n';
  for(int i=center.y-1; i<center.y+2; i++) elim_gem_special({center.x, i}, pos);
  for(int i=center.x-1; i<center.y+2; i++) elim_gem_special({i, center.y}, pos);
  gameboard[pos.x][pos.y].ability = ABI_PLANE;
}

void apply_special(Pos pos, Pos tar) {
  // TODO: Task 2-4
  if(gameboard[pos.x][pos.y].ability == ABI_BOMB) apply_bomb(pos);
  else if(gameboard[pos.x][pos.y].ability == ABI_KILLSAME) apply_killsame(pos, tar);
  else if(gameboard[pos.x][pos.y].ability == ABI_CROSS) apply_cross(pos);
  else if(gameboard[pos.x][pos.y].ability == ABI_PLANE) apply_plane(pos);
}

void dropping() {
  // TODO: Task 3
  for (int i = 0; i < BOARD_HEIGHT; ++i) {
    for (int j = 0; j < BOARD_WIDTH; ++j) {
      if (gameboard[9-i][j].ability != ABI_NULL) continue;
      int curr_height = 9-i;
      while (check_inboard({curr_height, j}) and gameboard[curr_height][j].ability == ABI_NULL)
        curr_height--;
      if (check_inboard({curr_height, j})) swap(gameboard[curr_height][j], gameboard[9-i][j]), moved_tags[9-i][j] = 1;
    }
  }

  for (int i = 0; i < BOARD_HEIGHT; ++i) {
    for (int j = 0; j < BOARD_WIDTH; ++j) {
      if (gameboard[i][j].ability == ABI_NULL) {
        gameboard[i][j].type = gen_rand_type();
        gameboard[i][j].ability = ABI_NORMAL;
        moved_tags[i][j] = 1;
      }
    }
  }
  return;
}

int menu() {
  cout << "\033[2J\033[1;1H";
  int game_mode = 0;
  cout << "======================================\n\n"
       << " Welcome to Sprout Crush!\n\n"
       << " Two game modes:\n"
       << " 1. Finite steps, max score!\n"
       << " 2. Target score, min steps!\n\n"
       << "======================================\n\n";

  while (true) {
    cout << "Select a mode, 1 or 2:\n";
    cin >> game_mode;
    cout << "Game mode: " << game_mode << endl;
    if (game_mode == 1 || game_mode == 2) break;
    else cout << "invalid mode! try again.\n\n";
  }

  cout << "\033[2J\033[1;1H";
  return game_mode;
}

int gen_rand() {
  return abs((int)mt());
}

int gen_rand_type() {
  return gen_rand() % 5 + 1;
}

string get_color(int type) {
  static string color_array[GEM_CNT] = {COLOR_NULL, COLOR_RUBY, COLOR_LAPIZ, COLOR_EMERALD, COLOR_AMBER, COLOR_AMETHYST};
  return color_array[type];
}

char get_style(int ability) {
  static char ability_array[ABI_CNT] = {' ', 'o', '+', 'Q', 'z', '%'};
  return ability_array[ability];
}

void gen_board() {
  for (int i = 0; i < BOARD_HEIGHT; ++i) {
    for (int j = 0; j < BOARD_WIDTH; ++j) {
      gameboard[i][j].type = gen_rand_type();
      gameboard[i][j].ability = gameboard[i][j].ability ? gameboard[i][j].ability : ABI_NORMAL;
    }
  }

  Pos bad_pos = {0, 0};
  while (check_eliminate(&bad_pos)) {
    gameboard[bad_pos.x][bad_pos.y].type = gen_rand_type();
  }
  return;
}

int dist_sq(Pos a, Pos b) {
  return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}


void eli_dfs(Pos pos, ElimiData *data, Pos *rnd_q) {
  visited[pos.x][pos.y] = 1;

  for (int i = 0; i < 4; ++i) {
    Pos tar = {pos.x + dir[i].x, pos.y + dir[i].y};
    if (check_inboard(tar) and !visited[tar.x][tar.y]
        and gameboard[pos.x][pos.y].type == gameboard[tar.x][tar.y].type
        and elimi_tags[tar.x][tar.y]) {
      eli_dfs(tar, data, rnd_q);
    }
  }
  data->total_elimi++;
  data->mid_elimi += success_line[pos.x][pos.y];
  success_line[pos.x][pos.y] = 0;
  if (moved_tags[pos.x][pos.y]) {
    rnd_q[data->rnd_cnt++] = pos;
    moved_tags[pos.x][pos.y] = 0;
  }
}

GemData gen_special(Pos pos) {
  static Pos random_queue[BOARD_HEIGHT * BOARD_WIDTH] = {};
  ElimiData data = {0, 0, 0};


  eli_dfs(pos, &data, random_queue);

  int abi = ABI_NORMAL;
  if (data.total_elimi == 4) {
    abi = ABI_CROSS;
  }
  else if (data.total_elimi > 4) {
    if (data.total_elimi - data.mid_elimi == 2)
      abi = ABI_KILLSAME;
    else
      abi = ABI_BOMB;
  }

  if (abi == ABI_NORMAL) return {};

  Pos ret_pos = random_queue[gen_rand() % data.rnd_cnt];
  Gem ret_gem = gameboard[ret_pos.x][ret_pos.y];
  ret_gem.ability = abi;
  GemData ret = {ret_pos, ret_gem};
  if (abi == ABI_KILLSAME || abi == ABI_BOMB) ret.gem.type = GEM_NULL;
  return ret;
}

GemData gen_plane(Pos pos) {
  bool plane = true;
  int cur_type = gameboard[pos.x][pos.y].type;
  for(int i=pos.x; i<pos.x+2; i++) {
    for(int j=pos.y; j<pos.y+2; j++) {
      if(cur_type != gameboard[i][j].type) {
        plane = false;
      }
    }
  }
  if(plane) {
    for(int i=pos.x; i<pos.x+2; i++) {
      for(int j=pos.y; j<pos.y+2; j++) {
        elimi_tags[i][j] = 1;
      }
    }
    Gem ret_gem;
    ret_gem.type = GEM_NULL;
    ret_gem.ability = ABI_PLANE;
    return {pos, ret_gem};
  }
  return {};
}

void eliminate(int mode, int combo) {
  int tmp_score = 0;

  // reset visited array for eli_dfs
  for (int i = 0; i < BOARD_HEIGHT; ++i) {
    for (int j = 0; j < BOARD_WIDTH; ++j) {
      visited[i][j] = 0;
    }
  }

  // tag the gems should be eliminate
  for (int i = 0; i < BOARD_HEIGHT; ++i) {
    for (int j = 0; j < BOARD_WIDTH; ++j) {
      int check_ret = check_line({i, j});

      if (!check_ret) {
        continue;
      }

      if (check_ret & 1) {
        elimi_tags[i - 1][j] = 1;
        elimi_tags[i + 1][j] = 1;
      }
      if (check_ret & 2) {
        elimi_tags[i][j - 1] = 1;
        elimi_tags[i][j + 1] = 1;
      }
      success_line[i][j] += (check_ret + 1) >> 1;
      elimi_tags[i][j] = 1;
    }
  }

  // record the special gem will be generate
  GemData gen_buff[BOARD_HEIGHT * BOARD_WIDTH] = {};
  int gen_cnt = 0;
  for (int i = 0; i < BOARD_HEIGHT; ++i) {
    for (int j = 0; j < BOARD_WIDTH; ++j) {
      if (success_line[i][j]) {
        GemData gem_data = gen_special({i, j});
        if (gem_data.gem.ability != ABI_NULL) {
          gen_buff[gen_cnt++] = gem_data;
        }
      }
    }
  }

  for (int i = 0; i < BOARD_HEIGHT-1; ++i) {
    for (int j = 0; j < BOARD_WIDTH-1; ++j) {
      GemData gem_data = gen_plane({i, j});
      if (gem_data.gem.ability == ABI_PLANE) {
        gen_buff[gen_cnt++] = gem_data;
      }
    }
  }

  // apply special ability of cross
  for (int i = 0; i < BOARD_HEIGHT; ++i) {
    for (int j = 0; j < BOARD_WIDTH; ++j) {
      if (elimi_tags[i][j] and gameboard[i][j].ability >= ABI_CROSS) {
        Pos tar = {};
        for (int k = 0; k < 4; ++k) {
          Pos curr_test = {i + dir[k].x, j + dir[k].y};
          if (check_inboard(curr_test) and moved_tags[curr_test.x][curr_test.y]) {
            tar = curr_test;
          }
        }
        cout << "apply " << i << ", " << j << " | tar " << tar.x << ", " << tar.y << '\n';
        apply_special({i, j}, tar);
      }
    }
  }

  draw_board(mode, combo, 1500);

  // generate special gem and calculate the score of them
  for (int i = 0; i < gen_cnt; ++i) {
    gameboard[gen_buff[i].pos.x][gen_buff[i].pos.y] = gen_buff[i].gem;
    elimi_tags[gen_buff[i].pos.x][gen_buff[i].pos.y] = 0;
    tmp_score += 100;
  }

  // eliminate the gem
  for (int i = 0; i < BOARD_HEIGHT; ++i) {
    for (int j = 0; j < BOARD_WIDTH; ++j) {
      moved_tags[i][j] = 0;
      if (elimi_tags[i][j]) {
        elimi_tags[i][j] = 0;
        gameboard[i][j].type = GEM_NULL;
        gameboard[i][j].ability = ABI_NULL;
        tmp_score += 100;
      }
    }
  }
  player_score += tmp_score * (10 + combo) / 10;
  return;
}

void clean_color() {
  cout << "\x1b[0m";
}

void draw_board(int mode, int combo, int time = DRAW_PAUSE_TIME) {
  cout << "\033[2J\033[1;1H";

  if (mode == MODE_STEP) {
    cout << "STEP REMAINED: " << step_remained << "\nSCORE: " << player_score << " COMBO: " << combo;
    if (best_score != 0) cout << " BEST SCORE: " << best_score;
    cout << "\n\n";
  }
  else if (mode == MODE_SCORE) {
    cout << "STEP USED: " << step_used << "\nSCORE/TARGET: " << player_score << "/" << SCORE_TARGET << " COMBO: " << combo;
    if (best_step != 0) cout << " BEST STEP: " << best_step;
    cout << "\n\n";
  }

  /* Chi-chun edit: Display line numbers for the user's convenience. */
  cout << " ";
  for (int i = 0; i < BOARD_WIDTH; ++i) {
    cout << " " << i;
  }
  cout << '\n';

  for (int i = 0; i < BOARD_HEIGHT; ++i) {
    /* Chi-chun edit: Display line numbers for the user's convenience. */
    cout << "\033[0m" << i << " ";
    for (int j = 0; j < BOARD_WIDTH; ++j) {
      string color = get_color(gameboard[i][j].type);
      string deco = elimi_tags[i][j] ? "5" : "0";
      string ansi = "\x1b[" + deco + ";" + color + "m";
      cout << ansi << get_style(gameboard[i][j].ability) << ' ';
    }
    cout << '\n';
  }
  clean_color();
  cout << "\n";
  this_thread::sleep_for(chrono::milliseconds(time));
  return;
}



bool check_dead() {
  bool is_dead = 1;
  for (int i = 0; i < BOARD_HEIGHT; ++i) {
    for (int j = 0; j < BOARD_WIDTH; ++j) {
      if (check_inboard({i, j + 1})) is_dead &= !(check_swap({i, j}, {i, j + 1}));
      if (check_inboard({i + 1, j})) is_dead &= !(check_swap({i, j}, {i + 1, j}));
    }
  }

  return is_dead;
}

bool game_end(int mode) {
  return (mode == MODE_STEP && step_remained == 0) || (mode == MODE_SCORE && player_score >= SCORE_TARGET);
}

void gem_swap(Pos a, Pos b) {
  moved_tags[a.x][a.y] = 1;
  moved_tags[b.x][b.y] = 1;

  if (gameboard[a.x][a.y].ability > ABI_CROSS || gameboard[b.x][b.y].ability > ABI_CROSS) {
    if (gameboard[a.x][a.y].ability > ABI_CROSS) elimi_tags[a.x][a.y] = 1;
    if (gameboard[b.x][b.y].ability > ABI_CROSS) elimi_tags[b.x][b.y] = 1;
    return;
  }

  swap(gameboard[a.x][a.y], gameboard[b.x][b.y]);
}

void init_global_variable() {
  step_remained = STEP_LIMIT;
  step_used = 0;
  player_score = 0;
}

void init_gameboard() {
  for (int i = 0; i < BOARD_HEIGHT; ++i) {
    for (int j = 0; j < BOARD_WIDTH; ++j) {
      gameboard[i][j].ability = ABI_NORMAL;
    }
  }
}

void game_init(int mode) {
  do {
    gen_board();
  } while(check_dead());
  init_global_variable();
  init_gameboard();
  draw_board(mode, 0);
  return;
}

bool check_str_int(string str) {
  if (str.length() >= 10) return 0;
  for (unsigned int i = 0; i < str.length(); ++i) {
    if (str[i] < '0' || str[i] > '9')
      return 0;
  }
  return 1;
}


clock_t start_time = 0, end_time= 0;
double cur_time, total_time, times[20];
int time_index = 0;
int main_game(int mode) {
  int running = 1;
  int step = 0;
  game_init(mode);
  do {
    Pos a, b;

#ifdef AI
    start_time = clock();
    ai (a, b);
    end_time = clock();
    cur_time = end_time-start_time;
    // cout << "used " << cur_time << " ms\n";
    total_time += cur_time;
    time_index = time_index>20?20:time_index;
    times[time_index++] = cur_time;
    cout << "ai returned: " << a.x << ' ' << a.y << " | " << b.x << ' ' << b.y << '\n';
    step_remained--;
    step_used++;
    if (check_swap(a, b)) {
      gem_swap(a, b);
      draw_board(mode, 0);
    }
    else if (game_end(mode)) {
      break;
    }
    else {
      continue;
    };
#else
    cout << "input two position:\n";
    string input[4];
    for (int i = 0; i < 4; ++i) cin >> input[i];
    int input_validity = 1;
    for (int i = 0; i < 4; ++i) {
      if (!check_str_int(input[i])) {
        input_validity = 0;
      }
    }
    if (input_validity) {
      a.x = stoi(input[0]);
      a.y = stoi(input[1]);
      b.x = stoi(input[2]);
      b.y = stoi(input[3]);
    }
    else {
      a.x = -1;
    }

    if (check_swap(a, b)) {
      gem_swap(a, b);
      step_remained--;
      step_used++;
      draw_board(mode, 0);
    }
    else {
      /* Chi-chun edit: Remind the user that invalid operations are taken */
      draw_board(mode, 0, 0);
      cout << "invalid operation!\n";
      continue;
    };
#endif

    int combo = 0;
    do {
      eliminate(mode, combo);
      draw_board(mode, combo);
      dropping();
      draw_board(mode, combo);
      combo++;
      cout << "Combo!" << endl;
      // this_thread::sleep_for(chrono::milliseconds(200));
    } while (check_eliminate(nullptr));

    // reset moved_tags
    for (int i = 0; i < BOARD_HEIGHT; ++i) {
      for (int j = 0; j < BOARD_WIDTH; ++j) {
        moved_tags[i][j] = 0;
      }
    }

    step++;
    while (check_dead()) {
      gen_board();
    }
    draw_board(mode, 0, 0);

    cout << "step_remained: " << step_remained << '\n';
    if (game_end(mode)) running = 0;
  } while (running);
  
  cout << mode << " | " << step_remained << '\n';
  // timer
  cout << "total_time: " << total_time << " ms\n";
  for(int i=0; i<20; i++) {
    cout << times[i] << ' ';
  }

  total_time = 0;
  cout << "\nGame over!";
  if (mode == MODE_STEP && player_score > best_score) {
    best_score = player_score;
    cout << " New record in mode 1!";
  }
  else if (mode == MODE_SCORE && step_used < best_step) {
    best_step = step_used;
    cout << " New record in mode 2!";
  }
  cout << endl;
  this_thread::sleep_for(chrono::milliseconds(3000));
  return 0;
}

int get_score() {
  return player_score;
}