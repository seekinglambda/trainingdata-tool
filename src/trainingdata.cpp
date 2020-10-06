#include "trainingdata.h"

#include <cstring>

uint64_t resever_bits_in_bytes(uint64_t v) {
  v = ((v >> 1) & 0x5555555555555555ull) | ((v & 0x5555555555555555ull) << 1);
  v = ((v >> 2) & 0x3333333333333333ull) | ((v & 0x3333333333333333ull) << 2);
  v = ((v >> 4) & 0x0F0F0F0F0F0F0F0Full) | ((v & 0x0F0F0F0F0F0F0F0Full) << 4);
  return v;
}

lczero::V5TrainingData get_v5_training_data(
        lczero::GameResult game_result, const lczero::PositionHistory& history,
        lczero::Move played_move, lczero::MoveList legal_moves, float Q, float M) {
  lczero::V5TrainingData result;

  // Set version.
  result.version = 5;

  // Illegal moves will have "-1" probability
  for (auto& probability : result.probabilities) {
    probability = -1;
  }

  // Populate legal moves with probability "0"
  for (lczero::Move move : legal_moves) {
    result.probabilities[move.as_nn_index(0)] = 0;
  }

  // Assign "1" (100%) to the move that was actually played
  result.probabilities[played_move.as_nn_index(0)] = 1.0f;

  // Populate planes.
  lczero::InputPlanes planes =
          EncodePositionForNN(
            pblczero::NetworkFormat::INPUT_CLASSICAL_112_PLANE,
            history, 8, lczero::FillEmptyHistory::FEN_ONLY, nullptr);
  int plane_idx = 0;
  for (auto& plane : result.planes) {
    plane = resever_bits_in_bytes(planes[plane_idx++].mask);
  }

  const auto& position = history.Last();
  // Populate castlings.
  const auto& castlings = position.GetBoard().castlings();
  result.castling_us_ooo =
          castlings.we_can_000() ? 1 : 0;
  result.castling_us_oo =
          castlings.we_can_00() ? 1 : 0;
  result.castling_them_ooo =
          castlings.they_can_000() ? 1 : 0;
  result.castling_them_oo =
          castlings.they_can_00() ? 1 : 0;

  // Other params.
  result.side_to_move_or_enpassant = position.IsBlackToMove() ? 1 : 0;
  result.rule50_count = position.GetRule50Ply();
  result.invariance_info = 0;

  // Game result.
  if (game_result == lczero::GameResult::WHITE_WON) {
    result.result = position.IsBlackToMove() ? -1 : 1;
  } else if (game_result == lczero::GameResult::BLACK_WON) {
    result.result = position.IsBlackToMove() ? 1 : -1;
  } else {
    result.result = 0;
  }

  // Q for Q+Z training
  result.root_q = result.best_q = 0.0f;
  // We have no D information
  result.root_d = result.best_d = 0.0f;

  result.root_m = result.best_m = M;
  return result;
}
