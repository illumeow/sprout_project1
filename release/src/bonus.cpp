#include "splendor.hpp"
// bonus task

// add z, z swap
void apply_killsame(Pos pos, Pos tar) {
  elimi_tags[pos.x][pos.y] = 1;
  gameboard[pos.x][pos.y].ability = ABI_NORMAL;

  int tartype;
  if(gameboard[tar.x][tar.y].ability == ABI_BOMB) {
    gameboard[tar.x][tar.y].ability = ABI_NORMAL;
    tartype = gen_rand_type();
  }
  else if(gameboard[tar.x][tar.y].ability == ABI_KILLSAME) {
    // elim all
    for(int i=0; i<BOARD_HEIGHT; i++) {
        for(int j=0; j<BOARD_WIDTH; j++) {
            elim_gem_special({i, j}, pos);
        }
    }
    gameboard[pos.x][pos.y].ability = ABI_KILLSAME;  
    return;
  }
  else {
    tartype = gameboard[tar.x][tar.y].type;
  }

  for(int i=0; i<BOARD_HEIGHT; i++) {
    for(int j=0; j<BOARD_WIDTH; j++) {
        if(gameboard[i][j].type == tartype) 
          elim_gem_special({i, j}, pos);
    }
  }
  gameboard[pos.x][pos.y].ability = ABI_KILLSAME;
}

// add a new kind of special gem: plane

/*
add #define ABI_PLANE 5 and change #define ABI_CNT 5->6
also modify get_style(), apply_special(), eliminate(), check_swap()
*/
// modify func
char get_style(int ability) {
  static char ability_array[ABI_CNT] = {' ', 'o', '+', 'Q', 'z', '%'};
  return ability_array[ability];
}

void apply_special(Pos pos, Pos tar) {
  // TODO: Task 2-4
  if(gameboard[pos.x][pos.y].ability == ABI_BOMB) apply_bomb(pos);
  else if(gameboard[pos.x][pos.y].ability == ABI_KILLSAME) apply_killsame(pos, tar);
  else if(gameboard[pos.x][pos.y].ability == ABI_CROSS) apply_cross(pos);
  else if(gameboard[pos.x][pos.y].ability == ABI_PLANE) apply_plane(pos);
}

// for eliminate(), add this after recording the special gems and before apply special ability
for (int i = 0; i < BOARD_HEIGHT; ++i) {
  for (int j = 0; j < BOARD_WIDTH; ++j) {
    GemData gem_data = gen_plane({i, j});
    if (gem_data.gem.ability == ABI_PLANE) {
      gen_buff[gen_cnt++] = gem_data;
    }
  }
}

// add these in check_swap in the fourth if
gameboard[a.x][a.y].ability == ABI_PLANE || gameboard[b.x][b.y].ability == ABI_PLANE

// new funcs
// search for a plane
// add this anywhere before eliminate(), recommend just before it
GemData gen_plane(Pos pos) {
  if(check_inboard({pos.x+1, pos.y+1})) {
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
}

// this should be BEFORE apply_special()
void apply_plane(Pos pos) {
  elimi_tags[pos.x][pos.y] = 1;
  gameboard[pos.x][pos.y].ability = ABI_NORMAL;
  Pos center = {gen_rand()%8 + 1, gen_rand()%8 + 1};
  for(int i=center.y-1; i<center.y+2; i++) elim_gem_special({center.x, i}, pos);
  for(int i=center.x-1; i<center.y+2; i++) elim_gem_special({i, center.y}, pos);
  gameboard[pos.x][pos.y].ability = ABI_PLANE;
}