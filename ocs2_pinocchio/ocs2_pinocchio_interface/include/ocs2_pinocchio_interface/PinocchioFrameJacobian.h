#pragma once

#include <pinocchio/algorithm/frames.hpp>

namespace ocs2 {

/**
 * Computes the Jacobian of a frame without modifying the pinocchio data.
 *
 * The frame-id overload of pinocchio::getFrameJacobian() writes data.oMf[frameId], which used to force a copy of the
 * complete pinocchio data in const contexts. The (joint, placement) overload called here only reads data (checked for
 * pinocchio 4.1), it is just not declared const.
 *
 * @note requires data to be updated with:
 *       pinocchio::computeJointJacobians(model, data, q)
 *
 * @param [in] model: pinocchio model
 * @param [in] data: pinocchio data
 * @param [in] frameId: index of the frame
 * @param [in] referenceFrame: reference frame in which the Jacobian is expressed
 * @param [out] J: 6 x nv frame Jacobian
 */
template <typename Scalar, int Options, template <typename, int> class JointCollectionTpl, typename Matrix6xLike>
void getFrameJacobianReadOnly(const pinocchio::ModelTpl<Scalar, Options, JointCollectionTpl>& model,
                              const pinocchio::DataTpl<Scalar, Options, JointCollectionTpl>& data, pinocchio::FrameIndex frameId,
                              pinocchio::ReferenceFrame referenceFrame, const Eigen::MatrixBase<Matrix6xLike>& J) {
  const auto& frame = model.frames[frameId];
  pinocchio::getFrameJacobian(model, const_cast<pinocchio::DataTpl<Scalar, Options, JointCollectionTpl>&>(data), frame.parentJoint,
                              frame.placement, referenceFrame, J.const_cast_derived());
}

}  // namespace ocs2
