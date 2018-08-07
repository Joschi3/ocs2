/*
 * TargetTrajectories_ROS_Interface.h
 *
 *  Created on: May 27, 2018
 *      Author: farbod
 */

#ifndef TARGETTRAJECTORIES_ROS_INTERFACE_OCS2_H_
#define TARGETTRAJECTORIES_ROS_INTERFACE_OCS2_H_

#include <string>
#include <vector>

#include <ros/ros.h>

#include <ocs2_core/cost/CostDesiredTrajectories.h>

// MPC messages
#include <ocs2_comm_interfaces/mpc_target_trajectories.h>

#include "ocs2_comm_interfaces/ocs2_ros_interfaces/common/RosMsgConversions.h"

namespace ocs2{

/**
 * This class implements TargetTrajectories communication interface using ROS.
 *
 * @tparam SCALAR_T: scalar type.
 */
template <typename SCALAR_T>
class TargetTrajectories_ROS_Interface
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	typedef SCALAR_T scalar_t;

	typedef CostDesiredTrajectories<scalar_t> cost_desired_trajectories_t;
	typedef typename cost_desired_trajectories_t::scalar_array_t 		 	scalar_array_t;
	typedef typename cost_desired_trajectories_t::dynamic_vector_t 			dynamic_vector_t;
	typedef typename cost_desired_trajectories_t::dynamic_vector_array_t 	dynamic_vector_array_t;

	TargetTrajectories_ROS_Interface(
			const std::string& nodeName = "robot_mpc");

	virtual ~TargetTrajectories_ROS_Interface() = default;

	/**
	 * Resets the class to its instantiate state.
	 */
	virtual void reset() {}

	/**
	 * This is the main routine which launches the publisher node for MPC's
	 * desired trajectories.
	 *
	 * @param [in] argc: Command line number of arguments.
	 * @param [in] argv: Command line vector of arguments.
	 */
	void launchNodes(int argc, char* argv[]);

	/**
	 * Publishes the target trajectories.
	 *
	 * @param [in] costDesiredTrajectories: The target trajectories.
	 */
	void publishTargetTrajectories(
			const cost_desired_trajectories_t& costDesiredTrajectories);

protected:
	std::string nodeName_;

	// Publisher
	::ros::Publisher mpcTargetTrajectoriesPublisher_;

	// ROS messages
	ocs2_comm_interfaces::mpc_target_trajectories mpcTargetTrajectoriesMsg_;
};

} // namespace ocs2

#include "implementation/TargetTrajectories_ROS_Interface.h"

#endif /* TARGETTRAJECTORIES_ROS_INTERFACE_OCS2_H_ */
