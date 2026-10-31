/******************************************************************************
Copyright (c) 2020, Farbod Farshidian. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

 * Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

#include <pinocchio/fwd.hpp>
#include <pinocchio/multibody/model.hpp>
#include <pinocchio/parsers/urdf.hpp>

#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>

#include <urdf_parser/urdf_parser.h>

#include "ocs2_pinocchio_interface/urdf.h"

namespace ocs2 {

namespace {

std::string escapeXml(const std::string& in) {
  std::string out;
  out.reserve(in.size());
  for (const char c : in) {
    switch (c) {
      case '&':
        out += "&amp;";
        break;
      case '<':
        out += "&lt;";
        break;
      case '>':
        out += "&gt;";
        break;
      case '"':
        out += "&quot;";
        break;
      default:
        out += c;
    }
  }
  return out;
}

void writeOrigin(std::ostream& os, const ::urdf::Pose& pose, const std::string& indent) {
  double roll, pitch, yaw;
  pose.rotation.getRPY(roll, pitch, yaw);
  os << indent << "<origin xyz=\"" << pose.position.x << ' ' << pose.position.y << ' ' << pose.position.z << "\" rpy=\"" << roll << ' '
     << pitch << ' ' << yaw << "\"/>\n";
}

void writeGeometry(std::ostream& os, const ::urdf::Geometry& geometry, const std::string& indent) {
  os << indent << "<geometry>\n";
  const std::string inner = indent + "  ";
  switch (geometry.type) {
    case ::urdf::Geometry::SPHERE: {
      const auto& sphere = static_cast<const ::urdf::Sphere&>(geometry);
      os << inner << "<sphere radius=\"" << sphere.radius << "\"/>\n";
      break;
    }
    case ::urdf::Geometry::BOX: {
      const auto& box = static_cast<const ::urdf::Box&>(geometry);
      os << inner << "<box size=\"" << box.dim.x << ' ' << box.dim.y << ' ' << box.dim.z << "\"/>\n";
      break;
    }
    case ::urdf::Geometry::CYLINDER: {
      const auto& cylinder = static_cast<const ::urdf::Cylinder&>(geometry);
      os << inner << "<cylinder radius=\"" << cylinder.radius << "\" length=\"" << cylinder.length << "\"/>\n";
      break;
    }
    case ::urdf::Geometry::MESH: {
      const auto& mesh = static_cast<const ::urdf::Mesh&>(geometry);
      os << inner << "<mesh filename=\"" << escapeXml(mesh.filename) << "\" scale=\"" << mesh.scale.x << ' ' << mesh.scale.y << ' '
         << mesh.scale.z << "\"/>\n";
      break;
    }
    default:
      throw std::runtime_error("[getUrdfXmlString] Unsupported URDF geometry type: " + std::to_string(geometry.type));
  }
  os << indent << "</geometry>\n";
}

void writeMaterial(std::ostream& os, const ::urdf::Material& material, const std::string& indent) {
  os << indent << "<material name=\"" << escapeXml(material.name) << "\">\n";
  os << indent << "  <color rgba=\"" << material.color.r << ' ' << material.color.g << ' ' << material.color.b << ' ' << material.color.a
     << "\"/>\n";
  if (!material.texture_filename.empty()) {
    os << indent << "  <texture filename=\"" << escapeXml(material.texture_filename) << "\"/>\n";
  }
  os << indent << "</material>\n";
}

void writeLink(std::ostream& os, const ::urdf::Link& link) {
  os << "  <link name=\"" << escapeXml(link.name) << "\">\n";
  if (link.inertial) {
    const auto& inertial = *link.inertial;
    os << "    <inertial>\n";
    writeOrigin(os, inertial.origin, "      ");
    os << "      <mass value=\"" << inertial.mass << "\"/>\n";
    os << "      <inertia ixx=\"" << inertial.ixx << "\" ixy=\"" << inertial.ixy << "\" ixz=\"" << inertial.ixz << "\" iyy=\"" << inertial.iyy
       << "\" iyz=\"" << inertial.iyz << "\" izz=\"" << inertial.izz << "\"/>\n";
    os << "    </inertial>\n";
  }
  for (const auto& visual : link.visual_array) {
    os << "    <visual" << (visual->name.empty() ? "" : " name=\"" + escapeXml(visual->name) + "\"") << ">\n";
    writeOrigin(os, visual->origin, "      ");
    writeGeometry(os, *visual->geometry, "      ");
    if (visual->material) {
      writeMaterial(os, *visual->material, "      ");
    } else if (!visual->material_name.empty()) {
      os << "      <material name=\"" << escapeXml(visual->material_name) << "\"/>\n";
    }
    os << "    </visual>\n";
  }
  for (const auto& collision : link.collision_array) {
    os << "    <collision" << (collision->name.empty() ? "" : " name=\"" + escapeXml(collision->name) + "\"") << ">\n";
    writeOrigin(os, collision->origin, "      ");
    writeGeometry(os, *collision->geometry, "      ");
    os << "    </collision>\n";
  }
  os << "  </link>\n";
}

std::string jointTypeToString(int type) {
  switch (type) {
    case ::urdf::Joint::REVOLUTE:
      return "revolute";
    case ::urdf::Joint::CONTINUOUS:
      return "continuous";
    case ::urdf::Joint::PRISMATIC:
      return "prismatic";
    case ::urdf::Joint::FLOATING:
      return "floating";
    case ::urdf::Joint::PLANAR:
      return "planar";
    case ::urdf::Joint::FIXED:
      return "fixed";
    default:
      throw std::runtime_error("[getUrdfXmlString] Unknown URDF joint type: " + std::to_string(type));
  }
}

void writeJoint(std::ostream& os, const ::urdf::Joint& joint) {
  os << "  <joint name=\"" << escapeXml(joint.name) << "\" type=\"" << jointTypeToString(joint.type) << "\">\n";
  writeOrigin(os, joint.parent_to_joint_origin_transform, "    ");
  os << "    <parent link=\"" << escapeXml(joint.parent_link_name) << "\"/>\n";
  os << "    <child link=\"" << escapeXml(joint.child_link_name) << "\"/>\n";
  if (joint.type != ::urdf::Joint::FIXED && joint.type != ::urdf::Joint::FLOATING) {
    os << "    <axis xyz=\"" << joint.axis.x << ' ' << joint.axis.y << ' ' << joint.axis.z << "\"/>\n";
  }
  if (joint.dynamics) {
    os << "    <dynamics damping=\"" << joint.dynamics->damping << "\" friction=\"" << joint.dynamics->friction << "\"/>\n";
  }
  if (joint.limits) {
    os << "    <limit lower=\"" << joint.limits->lower << "\" upper=\"" << joint.limits->upper << "\" effort=\"" << joint.limits->effort
       << "\" velocity=\"" << joint.limits->velocity << "\"/>\n";
  }
  if (joint.safety) {
    os << "    <safety_controller soft_lower_limit=\"" << joint.safety->soft_lower_limit << "\" soft_upper_limit=\""
       << joint.safety->soft_upper_limit << "\" k_position=\"" << joint.safety->k_position << "\" k_velocity=\"" << joint.safety->k_velocity
       << "\"/>\n";
  }
  if (joint.calibration) {
    os << "    <calibration";
    if (joint.calibration->rising) {
      os << " rising=\"" << *joint.calibration->rising << "\"";
    }
    if (joint.calibration->falling) {
      os << " falling=\"" << *joint.calibration->falling << "\"";
    }
    os << "/>\n";
  }
  if (joint.mimic) {
    os << "    <mimic joint=\"" << escapeXml(joint.mimic->joint_name) << "\" multiplier=\"" << joint.mimic->multiplier << "\" offset=\""
       << joint.mimic->offset << "\"/>\n";
  }
  os << "  </joint>\n";
}

}  // namespace

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
PinocchioInterface getPinocchioInterfaceFromUrdfFile(const std::string& urdfFile) {
  ::urdf::ModelInterfaceSharedPtr urdfTree = ::urdf::parseURDFFile(urdfFile);
  if (urdfTree != nullptr) {
    return getPinocchioInterfaceFromUrdfModel(urdfTree);
  } else {
    throw std::invalid_argument("The file " + urdfFile + " does not contain a valid URDF model.");
  }
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
PinocchioInterface getPinocchioInterfaceFromUrdfFile(const std::string& urdfFile, const PinocchioInterface::JointModel& rootJoint) {
  ::urdf::ModelInterfaceSharedPtr urdfTree = ::urdf::parseURDFFile(urdfFile);
  if (urdfTree != nullptr) {
    return getPinocchioInterfaceFromUrdfModel(urdfTree, rootJoint);
  } else {
    throw std::invalid_argument("The file " + urdfFile + " does not contain a valid URDF model.");
  }
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
PinocchioInterface getPinocchioInterfaceFromUrdfString(const std::string& xmlString) {
  ::urdf::ModelInterfaceSharedPtr urdfTree = ::urdf::parseURDF(xmlString);
  if (urdfTree != nullptr) {
    return getPinocchioInterfaceFromUrdfModel(urdfTree);
  } else {
    throw std::invalid_argument("The XML stream does not contain a valid URDF model.");
  }
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
PinocchioInterface getPinocchioInterfaceFromUrdfString(const std::string& xmlString, const PinocchioInterface::JointModel& rootJoint) {
  ::urdf::ModelInterfaceSharedPtr urdfTree = ::urdf::parseURDF(xmlString);
  if (urdfTree != nullptr) {
    return getPinocchioInterfaceFromUrdfModel(urdfTree, rootJoint);
  } else {
    throw std::invalid_argument("The XML stream does not contain a valid URDF model.");
  }
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
PinocchioInterface getPinocchioInterfaceFromUrdfModel(const std::shared_ptr<::urdf::ModelInterface>& urdfTree) {
  pinocchio::ModelTpl<scalar_t> model;
  pinocchio::urdf::buildModel(urdfTree, model);
  return PinocchioInterface(model, urdfTree);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
PinocchioInterface getPinocchioInterfaceFromUrdfModel(const std::shared_ptr<::urdf::ModelInterface>& urdfTree,
                                                      const PinocchioInterface::JointModel& rootJoint) {
  pinocchio::ModelTpl<scalar_t> model;
  pinocchio::urdf::buildModel(urdfTree, rootJoint, model);
  return PinocchioInterface(model, urdfTree);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
std::string getUrdfXmlString(const ::urdf::ModelInterface& urdfModel) {
  std::ostringstream os;
  os << std::setprecision(std::numeric_limits<double>::max_digits10);
  os << "<?xml version=\"1.0\"?>\n";
  os << "<robot name=\"" << escapeXml(urdfModel.getName()) << "\">\n";
  for (const auto& material : urdfModel.materials_) {
    writeMaterial(os, *material.second, "  ");
  }
  for (const auto& link : urdfModel.links_) {
    writeLink(os, *link.second);
  }
  for (const auto& joint : urdfModel.joints_) {
    writeJoint(os, *joint.second);
  }
  os << "</robot>\n";
  return os.str();
}

}  // namespace ocs2
