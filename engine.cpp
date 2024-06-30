#include <algorithm>
#include <random>
#include <iostream>
#include <thread>

#include "board.hpp"
#include "engine.hpp"
#include "butils.hpp"

using namespace std; 

clock_t start_time;
// Define a structure to hold move information
struct MoveInfo {
    U16 move;
    int priority; // Priority based on heuristic
};

//checks if time is more than 1.5 seconds for each move
// bool check_time(clock_t t const Board& board){
//     clock_t current_time = clock();
//     double time_sec = (double)(current_time -t)/CLOCKS_PER_SEC;    

//     if (board.data.board_type == SEVEN_THREE) { //2 secs per move for 7_3 board
//         if(time_sec>3){
//             // cout<<"time up"<<endl;
//             return true;
//         }
//         return false;}

//     if (board.data.board_type == EIGHT_FOUR) { //3 sec per move for 8_4 board
//         if(time_sec>4){
//             // cout<<"time up"<<endl;
//             return true;
//         }
//         return false;}

//     if (board.data.board_type == EIGHT_TWO) {//4 sec per move for 8_2 board
//         if(time_sec>5){
//             // cout<<"time up"<<endl;
//             return true;
//         }
//         return false;}
//     }

// Comparison function for sorting moves
bool compareMoves(const MoveInfo& a, const MoveInfo& b) {
    return a.priority > b.priority;
}

// Function to order moves based on a heuristic
void order_moves(std::unordered_set<U16>& moveset, const Board& board) {
    std::vector<MoveInfo> moveList;
    // if check_time(start_time, board){
    //     return score;}
    // Evaluate and assign priorities to each move
    for (const auto& move : moveset) {
        // Make a copy of the board to simulate the move without modifying the original
        Board copiedBoard(board);
        copiedBoard.do_move_without_flip_(move);

        int priority = 0;

        // Heuristic 1: Prioritize captures
        if (copiedBoard.under_threat(move)) {
            priority += 100;
        }

        // Heuristic 2: Prioritize checks
        if (copiedBoard.in_check()) {
            priority += 50;
        }

        moveList.push_back({move, priority});
    }

    // Sort moves based on priority
    std::sort(moveList.begin(), moveList.end(), compareMoves);

    // Update the original moveset with the sorted moves
    moveset.clear();
    for (const auto& moveInfo : moveList) {
        moveset.insert(moveInfo.move);
    }
}

void Engine::find_best_move(const Board& b) {

    //start the timer
    start_time = clock();
    // Get legal moves
    auto moveset = b.get_legal_moves();

    if (moveset.empty()) {
        // No legal moves, handle this case if needed
        return;
    }

    order_moves(moveset, b);
    
    // Initialize best move and evaluation values
    U16 bestMove = *moveset.begin();
    int bestValue = std::numeric_limits<int>::min(); // Negative infinity
    int depth = 5;
    
    // Iterate through legal moves
    for (const auto& move : moveset) {
        // Make a copy of the board to simulate the move without modifying the original
        Board copiedBoard(b);
        copiedBoard.do_move_(move);
        
        // Evaluate the position using minimax with alpha-beta pruning
        int eval = minimax(copiedBoard, depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), false);

        // Update the best move if a better move is found
        if (eval > bestValue) {
            bestValue = eval;
            bestMove = move;
        }
    }

    // Set the best move
    this->best_move = bestMove;
}

int Engine::minimax(const Board& board, int depth, int alpha, int beta, bool maximizingPlayer) {
    // Base case: reached the maximum depth or terminal state
    if (depth == 0 || board.in_check()) {
        return evaluate(board);
    }

    auto moveset = board.get_legal_moves();

    if (maximizingPlayer) {
        int maxEval = std::numeric_limits<int>::min();

        // if check_time(start_time,board){
        // return score;}

        for (const auto& move : moveset) {

            // if check_time(start_time,board){
            // return score;}

            Board copiedBoard(board); // copy board
            copiedBoard.do_move_without_flip_(move);
            int eval = minimax(copiedBoard, depth - 1, alpha, beta, false);

            // if check_time(start_time,board){
            // return score;}

            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) {
                break; // Prune the remaining branches
            }
        }
        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max();
        // if check_time(start_time,board){
        // return score;}

        for (const auto& move : moveset) {

            // if check_time(start_time,board){
            // return score;}

            Board copiedBoard(board); // copy board
            copiedBoard.do_move_(move);
            int eval = minimax(copiedBoard, depth - 1, alpha, beta, true);

            // if check_time(start_time,board){
            // return score;}

            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) {
                break; // Prune the remaining branches
            }
        }
        return minEval;
    }
}

int Engine::evaluate(const Board& board) {
    int score = 0;
    // if check_time(start_time,board){
    //     return 0;
    // }
    for (int x = 0; x < 7; ++x) {
        // if check_time(start_time,board){
        // return score;}

        for (int y = 0; y < 7; ++y) {
            int pos = x * 7 + y;

            U8 piece = board.data.board_0[pos];
            

            if (piece && EMPTY) {
                continue; // Skip empty squares
            }
            
            if (piece & WHITE) {
                if (piece & PAWN) {
                    score += 1;
                } else if (piece & ROOK) {
                    score += 5;
                } else if (piece & BISHOP) {
                    score += 3;
                } else if (piece & KING) {
                    score += 0;
                } else if (piece & PAWN) {
                    score += 5;
                } else if (piece & KNIGHT) {
                    score += 3;
                } else if (piece & PAWN_BISHOP) {
                    score += 0;
                } else if (piece & PAWN_ROOK) {
                    score += 8;
                }
            } else if (piece & BLACK) {
                if (piece & PAWN) {
                    score += 1;
                } else if (piece & ROOK) {
                    score += 5;
                } else if (piece & BISHOP) {
                    score += 3;
                } else if (piece & KING) {
                    score += 0;
                } else if (piece & PAWN) {
                    score += 5;
                } else if (piece & KNIGHT) {
                    score += 3;
                } else if (piece & PAWN_BISHOP) {
                    score += 0;
                } else if (piece & PAWN_ROOK) {
                    score += 8;
                }
            }
        }
    }
    // if check_time(start_time,board){
    //     return score;}

    if (board.data.board_type == SEVEN_THREE) {
        score += 8;
    }

    if (board.data.board_type == EIGHT_FOUR) {
        score += 3;
    }

    if (board.data.board_type == EIGHT_TWO) {
        score += 5;
    }

    if (board.data.player_to_play == BLACK) {
        score = -score; // Invert the score for the opponent.
    }

    return score;
}
