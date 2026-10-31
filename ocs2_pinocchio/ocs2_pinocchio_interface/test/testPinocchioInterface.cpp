#include <sstream>

#include <gtest/gtest.h>

#include <pinocchio/fwd.hpp>

#include <pinocchio/multibody/geometry.hpp>
#include <pinocchio/parsers/urdf.hpp>

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
