#pragma once
#include <vector>
#include <functional>

class Rule {
    public:
        virtual bool apply(bool (&possibilities)[9][9][9], std::function<void(int, int, int)> place_number_current) = 0;
};

class Sudoku {
    public:
        Sudoku(std::vector<std::vector<int>> &grid);
        void solve();
        void bind_rule(Rule *rule);
        std::vector<std::vector<int>> get();
    private:
        bool place_number(int num, int y, int x);
        bool cycle();
        void permutate();
        bool data_possibilities[9][9][9];
        std::vector<Rule *> rules;
        std::vector<std::vector<int>> solved;
};

class OnlyPossibilityRule : public Rule {
    public:
    bool apply(bool (&possibilities)[9][9][9], std::function<void(int, int, int)> place_number_current);
};

class RowRule : public Rule {
    public:
    bool apply(bool (&possibilities)[9][9][9], std::function<void(int, int, int)> place_number_current);
};

class ColumnRule : public Rule {
    public:
    bool apply(bool (&possibilities)[9][9][9], std::function<void(int, int, int)> place_number_current);
};

class GroupRule : public Rule {
    public:
    bool apply(bool (&possibilities)[9][9][9], std::function<void(int, int, int)> place_number_current);
};

class GroupSpeculateRule : public Rule {
    public:
    bool apply(bool (&possibilities)[9][9][9], std::function<void(int, int, int)> place_number_current);
};