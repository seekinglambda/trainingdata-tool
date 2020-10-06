#if !defined(TRAININGDATA_H_INCLUDED)
#define TRAININGDATA_H_INCLUDED

#include "neural/encoder.h"
#include "neural/network.h"
#include "neural/writer.h"

lczero::V5TrainingData get_v5_training_data(
        lczero::GameResult game_result, const lczero::PositionHistory& history,
        lczero::Move played_move, lczero::MoveList legal_moves, float Q, float M);

#endif
