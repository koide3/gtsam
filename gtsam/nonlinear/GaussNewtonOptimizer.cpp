/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation,
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 * @file    GaussNewtonOptimizer.cpp
 * @brief   
 * @author  Richard Roberts
 * @created Feb 26, 2012
 */

#include <gtsam/inference/EliminationTree.h>
#include <gtsam/linear/GaussianJunctionTree.h>
#include <gtsam/nonlinear/GaussNewtonOptimizer.h>

using namespace std;

namespace gtsam {

/* ************************************************************************* */
void GaussNewtonOptimizer::iterate() {

  const NonlinearOptimizerState& current = state_;

  // Linearize graph
  GaussianFactorGraph::shared_ptr linear = graph_.linearize(current.values, *params_.ordering);

  // Optimize
  VectorValues delta;
  {
    if ( params_.isMultifrontal() ) {
      delta = GaussianJunctionTree(*linear).optimize(params_.getEliminationFunction());
    }
    else if ( params_.isSequential() ) {
      delta = gtsam::optimize(*EliminationTree<GaussianFactor>::Create(*linear)->eliminate(params_.getEliminationFunction()));
    }
    else if ( params_.isCG() ) {
      throw runtime_error("todo: ");
    }
    else {
      throw runtime_error("Optimization parameter is invalid: GaussNewtonParams::elimination");
    }
  }

  // Maybe show output
  if(params_.verbosity >= NonlinearOptimizerParams::DELTA) delta.print("delta");

  // Create new state with new values and new error
  state_.values = current.values.retract(delta, *params_.ordering);
  state_.error = graph_.error(state_.values);
  ++ state_.iterations;
}

} /* namespace gtsam */
