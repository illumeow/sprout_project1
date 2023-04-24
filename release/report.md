
task 3:
original code:
    voiddropping(){//TODO:Task3for(inti=0;i<BOARD_HEIGHT;++i){for(intj=0;j<BOARD_WIDTH;++j){if(gameboard[i][j].ability!=ABI_NULL)continue;intcurr_height=i;while(check_inboard({curr_height,j})andgameboard[curr_height][j].ability==ABI_NULL)curr_height--;if(check_inboard({curr_height,j}))swap(gameboard[curr_height][j],gameboard[i][j]);moved_tags[i][j]=1;}}for(inti=0;i<BOARD_HEIGHT;++i){for(intj=0;j<BOARD_WIDTH;++j){if(gameboard[i][j].ability==ABI_NULL){gameboard[i][j].type=gen_rand_type();gameboard[i][j].ability=ABI_NORMAL;moved_tags[i][j]=1;}}}return;}

fixed code:
    voiddropping(){//TODO:Task3for(inti=0;i<BOARD_HEIGHT;++i){for(intj=0;j<BOARD_WIDTH;++j){if(gameboard[9-i][j].ability!=ABI_NULL)continue;intcurr_height=9-i;while(check_inboard({curr_height,j})andgameboard[curr_height][j].ability==ABI_NULL)curr_height--;if(check_inboard({curr_height,j}))swap(gameboard[curr_height][j],gameboard[9-i][j]),moved_tags[9-i][j]=1;}}for(inti=0;i<BOARD_HEIGHT;++i){for(intj=0;j<BOARD_WIDTH;++j){if(gameboard[i][j].ability==ABI_NULL){gameboard[i][j].type=gen_rand_type();gameboard[i][j].ability=ABI_NORMAL;moved_tags[i][j]=1;}}}return;}

edit distance: 9
hooray!