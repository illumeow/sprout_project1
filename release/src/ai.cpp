#include "splendor.hpp"

void ai(Pos& pos1, Pos& pos2) {
  // TODO: Task 4.
  /*
  初步想法: 
  優先消除特殊寶石，掃過盤面知道每種寶石各有幾個
  之後用"能消除多少寶石"來決定要消除哪個特殊寶石
  若沒有特殊寶石則試著做出來，都做不出來就隨機選一個消除?
  不確定能不能預測drop以後的結果(不含新生成的)來決定
  */
  // You should remove the code below and determine the four values by yourself.
  pos1.x = gen_rand() % BOARD_HEIGHT;
  pos1.y = gen_rand() % BOARD_WIDTH;
  pos2.x = gen_rand() % BOARD_HEIGHT;
  pos2.y = gen_rand() % BOARD_WIDTH;

  return;
}
