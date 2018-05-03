/*
 * MPC_DummySimulator.cpp
 *
 *  Created on: Apr 15, 2018
 *      Author: farbod
 */

#include <string>
#include <iostream>

#include <boost/filesystem.hpp>

#include "ocs2_anymal_interface/OCS2AnymalInterface.h"
#include "ocs2_quadruped_interface/DummySimulator.h"

using namespace anymal;

int main( int argc, char* argv[] )
{

	typedef OCS2AnymalInterface ocs2_robot_interface_t;

	const std::string robotName = "anymal";
	const OCS2AnymalInterface::scalar_t mcLoopFrequency = 250;

	/******************************************************************************************************/
	if ( argc <= 1) throw std::runtime_error("No task file specified. Aborting.");
	boost::filesystem::path filePath(__FILE__);
	std::string taskFile = filePath.parent_path().parent_path().generic_string() + "/config/" + std::string(argv[1]) + "/task.info";
	std::cout << "Loading task file: " << taskFile << std::endl;

	ocs2_robot_interface_t::Ptr optimizationInterfacePtr( new ocs2_robot_interface_t(taskFile) );
	switched_model::DummySimulator<12> dummySimulator(optimizationInterfacePtr, robotName);

	dummySimulator.init(argc, argv, mcLoopFrequency);

	dummySimulator.run();

	xpp_msgs::RobotStateCartesian point;

}

