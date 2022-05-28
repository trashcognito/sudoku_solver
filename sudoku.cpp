#include "sudoku.hpp"
#include <exception>
#include <vector>

Sudoku::Sudoku(std::vector<std::vector<int>> &grid) {
    solved = std::vector<std::vector<int>>();
    for (int i=0;i<9;i++) {
        std::vector<int> temp;
        temp.resize(9);
        solved.push_back(temp);
    }
    
    for (int i=0;i<9;i++) {
        for (int j=0;j<9;j++) {
            for (int k=0;k<9;k++) {
                this->data_possibilities[i][j][k] = true;
            }
        }
    }

    for (int i=0;i<9;i++) {
        for (int j=0;j<9;j++) {
            if (grid[i][j] != 0) {
                this->place_number(grid[i][j] - 1, i, j);
            }
        }
    }
}

bool Sudoku::place_number(int num, int y, int x) {
    if (!data_possibilities[num][y][x]) {
        throw "Impossible number placement!";
    }
    
    if (solved[y][x] != 0) {
        return false;
    }
    solved[y][x] = num + 1;

    for (int n_num=0;n_num<9;n_num++) {
        if (n_num == num) 
            continue;
        data_possibilities[n_num][y][x] = false;
    }
    for (int n_y=0;n_y<9;n_y++) {
        if (n_y == y) 
            continue;
        data_possibilities[num][n_y][x] = false;
    }
    for (int n_x=0;n_x<9;n_x++) {
        if (n_x == x) 
            continue;
        data_possibilities[num][y][n_x] = false;
    }

    //Eliminate all in group too
    for (int n_y=(y - (y%3)); n_y < (y - (y%3) + 3); n_y++) {
        for (int n_x=(x - (x%3)); n_x < (x - (x%3) + 3); n_x++) {
            if (n_x == x && n_y == y)
                continue;

            data_possibilities[num][n_y][n_x] = false;
        }
    }

    return true;
}

bool Sudoku::cycle() {
    bool done = true;
    for (auto rule : rules) {
        done &= !rule->apply(this->data_possibilities, [&] (int num, int y, int x) {done &= !this->place_number(num, y, x);});
    }
    return done;
}

void Sudoku::solve() {
    while (!this->cycle());
    //Classical solver failed, permutate!
    this->permutate();
}

void Sudoku::bind_rule(Rule *rule) {
    this->rules.push_back(rule);
}

std::vector<std::vector<int>> Sudoku::get() {
    return this->solved;
}

void Sudoku::permutate() {
    auto current_state = this->get();
    for (int y=0; y<9; y++) {
        for (int x=0; x<9; x++) {
            if (current_state[y][x] != 0)
                continue;

            for (int num=0;num<9;num++) {
                if (!data_possibilities[num][y][x]) 
                    continue;
                std::vector<std::vector<int>> s_copy;
                for (int i=0;i<9;i++) {
                    std::vector<int> temp;
                    temp.resize(9);
                    for (int j=0;j<9;j++) {
                        temp[j] = current_state[i][j];
                    }
                    s_copy.push_back(temp);
                }

                //Permute!
                s_copy[y][x] = num + 1;

                Sudoku permuted(s_copy);
                
                for (auto rule : rules) {
                    permuted.bind_rule(rule);
                }

                try {
                    permuted.solve();
                    //Solved successfully, copy into ourselves!
                    auto solution = permuted.get();

                    for (int i=0;i<9;i++) {
                        for (int j=0;j<9;j++) {
                            if (solution[i][j] != 0) {
                                this->place_number(solution[i][j] - 1, i, j);
                            }
                        }
                    }
                    
                } catch (char const *e) {
                    //failed attempt, move on to the next possibility
                    continue;
                }
            }
            

        }
    }
}

bool OnlyPossibilityRule::apply(bool (&possibilities)[9][9][9], std::function<void (int, int, int)> place_number_current) {
    for (int y=0; y<9; y++) {
        for (int x=0; x<9; x++) {
            int count=0;
            int lastnum=0;
            for (int num=0;num<9;num++) {
                if (possibilities[num][y][x]) {
                    count++;
                    lastnum = num;
                }
            }
            if (count == 0) {
                throw "No possibilities left for this cell!";
            }
            if (count == 1) {
                place_number_current(lastnum, y, x);
            }
        }
    }
    return false;
}

bool RowRule::apply(bool (&possibilities)[9][9][9], std::function<void (int, int, int)> place_number_current) {
    for (int y=0;y<9;y++) {
        for (int num=0;num<9;num++) {
            int count = 0;
            int last_x = 0;
            for (int x=0;x<9;x++) {
                if (possibilities[num][y][x]) {
                    count++;
                    last_x = x;
                }
            }
            if (count == 0) {
                throw "No possibilities left for this row!";
            }
            if (count == 1) {
                place_number_current(num, y, last_x);
            }
        }
    }
    return false;
}

bool ColumnRule::apply(bool (&possibilities)[9][9][9], std::function<void (int, int, int)> place_number_current) {
    for (int x=0;x<9;x++) {
        for (int num=0;num<9;num++) {
            int count = 0;
            int last_y = 0;
            for (int y=0;y<9;y++) {
                if (possibilities[num][y][x]) {
                    count++;
                    last_y = y;
                }
            }
            if (count == 0) {
                throw "No possibilities left for this row!";
            }
            if (count == 1) {
                place_number_current(num, last_y, x);
            }
        }
    }
    return false;
}

bool GroupRule::apply(bool (&possibilities)[9][9][9], std::function<void (int, int, int)> place_number_current) {
    for (int group_x=0;group_x<3;group_x++) {
        for (int group_y=0;group_y<3;group_y++) {
            for (int num=0;num<9;num++) {
                int count=0;
                int last_y = 0;
                int last_x = 0;
                for (int y=(group_y*3); y<((group_y+1)*3); y++) {
                    for (int x=(group_x*3); x<((group_x+1)*3); x++) {
                        if (possibilities[num][y][x]) {
                            count++;
                            last_y = y;
                            last_x = x;
                        }
                    }
                }
                if (count == 0) {
                    throw "No possibilities left for this group!";
                }
                if (count == 1) {
                    place_number_current(num, last_y, last_x);
                }
            }
        }
    }
    return false;
}

bool GroupSpeculateRule::apply(bool (&possibilities)[9][9][9], std::function<void (int, int, int)> place_number_current) {
    bool touched = false;
    for (int group_x=0;group_x<3;group_x++) {
        for (int group_y=0;group_y<3;group_y++) {
            for (int num=0;num<9;num++) {
                int count=0;
                int last_row = 0;
                //check rows
                for (int y=(group_y*3); y<((group_y+1)*3); y++) {
                    for (int x=(group_x*3); x<((group_x+1)*3); x++) {
                        if (possibilities[num][y][x]) {
                            count++;
                            last_row = y;
                            x = (group_x+1) * 3;
                        }
                    }
                }
                if (count == 0) {
                    throw "Invalid group! No rows with number!";
                }
                //Clear remaining row
                if (count == 1) {
                    for (int x=0;x<9;x++) {
                        if (x >= (group_x*3) && x < ((group_x+1)*3)) {
                            continue;
                        }
                        touched |= possibilities[num][last_row][x];
                        possibilities[num][last_row][x] = false;
                    }
                }

                //Now, repeat for columns:
                count=0;
                int last_col=0;
                for (int x=(group_x*3); x<((group_x+1)*3); x++) {
                    for (int y=(group_y*3); y<((group_y+1)*3); y++) {
                        if (possibilities[num][y][x]) {
                            count++;
                            last_col = x;
                            y = (group_y+1) * 3;
                        }
                    }
                }
                if (count == 0) {
                    throw "Invalid group! No rows with number!";
                }
                //Clear remaining row
                if (count == 1) {
                    for (int y=0;y<9;y++) {
                        if (y >= (group_y*3) && y < ((group_y+1)*3)) {
                            continue;
                        }
                        touched |= possibilities[num][y][last_col];
                        possibilities[num][y][last_col] = false;
                    }
                }
            }
        }
    }
    return touched;
}