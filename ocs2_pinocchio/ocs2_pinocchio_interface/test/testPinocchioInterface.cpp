#include <sstream>

#include <gtest/gtest.h>

#include <pinocchio/fwd.hpp>

#include <pinocchio/multibody/geometry.hpp>
#include <pinocchio/parsers/urdf.hpp>

#include <pinocchio/algorithm/jacobian.hpp>
#include <pinocchio/algorithm/kinematics.hpp>

#include <ocs2_pinocchio_interface/PinocchioFrameJacobian.h>
#include <ocs2_pinocchio_interface/PinocchioInterface.h>
#include <ocs2_pinocchio_interface/urdf.h>
#include "CartPoleUrdf.h"
#include "ManipulatorArmUrdf.h"

TEST(testPinocchioInterface, buildFromXml) {
  auto pinocchio = ocs2::getPinocchioInterfaceFromUrdfString(cartPoleUrdf);
  std::cout << pinocchio;
}

TEST(testPinocchioInterface, urdfXmlStringRoundTrip) {
  const auto pinocchioInterface = ocs2::getPinocchioInterfaceFromUrdfString(manipulatorArmUrdf);
  const auto& model = pinocchioInterface.getModel();

  // the exported XML must describe the same kinematic tree
  const auto exported = ocs2::getUrdfXmlString(*pinocchioInterface.getUrdfModelPtr());
  const auto reparsed = ocs2::getPinocchioInterfaceFromUrdfString(exported);
  EXPECT_EQ(reparsed.getModel().nq, model.nq);
  EXPECT_EQ(reparsed.getModel().njoints, model.njoints);
  EXPECT_EQ(reparsed.getModel().nframes, model.nframes);

  // and yield the same collision geometry as the original XML
  pinocchio::GeometryModel originalGeom, exportedGeom;
  std::istringstream originalStream(manipulatorArmUrdf);
  std::istringstream exportedStream(exported);
  pinocchio::urdf::buildGeom(model, originalStream, pinocchio::COLLISION, originalGeom);
  pinocchio::urdf::buildGeom(model, exportedStream, pinocchio::COLLISION, exportedGeom);

  ASSERT_GT(originalGeom.ngeoms, 0);
  ASSERT_EQ(exportedGeom.ngeoms, originalGeom.ngeoms);
  for (size_t i = 0; i < originalGeom.ngeoms; ++i) {
    const auto& expected = originalGeom.geometryObjects[i];
    const auto& actual = exportedGeom.geometryObjects[i];
    EXPECT_EQ(actual.name, expected.name);
    EXPECT_EQ(actual.parentJoint, expected.parentJoint);
    EXPECT_EQ(actual.geometry->getNodeType(), expected.geometry->getNodeType());
    // RPY extraction near pitch = pi/2 (gimbal lock) is accurate to ~1e-11 only
    EXPECT_TRUE(actual.placement.isApprox(expected.placement, 1e-9)) << expected.name;
  }
}

TEST(testPinocchioInterface, frameJacobianReadOnly) {
  auto pinocchioInterface = ocs2::getPinocchioInterfaceFromUrdfString(manipulatorArmUrdf);
  const auto& model = pinocchioInterface.getModel();
  auto& data = pinocchioInterface.getData();

  const ocs2::vector_t q = ocs2::vector_t::Random(model.nq);
  pinocchio::computeJointJacobians(model, data, q);
  pinocchio::updateFramePlacements(model, data);

  for (const auto rf : {pinocchio::LOCAL, pinocchio::WORLD, pinocchio::LOCAL_WORLD_ALIGNED}) {
    for (pinocchio::FrameIndex frameId = 0; frameId < model.nframes; ++frameId) {
      // invalidate the frame placement to detect writes to data
      data.oMf[frameId] = pinocchio::SE3::Identity();
      const pinocchio::Data& constData = data;

      ocs2::matrix_t J = ocs2::matrix_t::Zero(6, model.nv);
      ocs2::getFrameJacobianReadOnly(model, constData, frameId, rf, J);
      EXPECT_TRUE(data.oMf[frameId].isIdentity());

      ocs2::matrix_t Jexpected = ocs2::matrix_t::Zero(6, model.nv);
      pinocchio::Data dataCopy = data;
      pinocchio::getFrameJacobian(model, dataCopy, frameId, rf, Jexpected);
      EXPECT_TRUE(J.isApprox(Jexpected)) << "frame " << model.frames[frameId].name << ", reference frame " << rf;
    }
  }
}
