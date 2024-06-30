#pragma once

#include "engine_base.hpp"
#include <atomic>

class Engine : public AbstractEngine {
public:
    U16 best_move;
    void find_best_move(const Board& b) override;
    int minimax(const Board& board, int depth, int alpha, int beta, bool maximizingPlayer);
    int evaluate(const Board& board);
};
