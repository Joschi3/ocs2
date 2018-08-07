/*
 * RosMsgConversions.h
 *
 *  Created on: May 23, 2018
 *      Author: farbod
 */

namespace ocs2{

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <size_t STATE_DIM, size_t INPUT_DIM>
template <class ContainerAllocator>
void RosMsgConversions<STATE_DIM, INPUT_DIM>::CreateObservationMsg(
		const system_observation_t& observation,
		ocs2_comm_interfaces::mpc_observation_<ContainerAllocator>& observationMsg) {

	observationMsg.time = observation.time();

	observationMsg.state.value.resize(STATE_DIM);
	for (size_t i=0; i<STATE_DIM; i++)
		observationMsg.state.value[i] = observation.state(i);

	observationMsg.input.value.resize(INPUT_DIM);
	for (size_t i=0; i<INPUT_DIM; i++)
		observationMsg.input.value[i] = observation.input(i);

	observationMsg.subsystem = observation.subsystem();
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <size_t STATE_DIM, size_t INPUT_DIM>
template <class ContainerAllocator>
void RosMsgConversions<STATE_DIM, INPUT_DIM>::ReadObservationMsg(
		const ocs2_comm_interfaces::mpc_observation_<ContainerAllocator>& observationMsg,
		system_observation_t& observation) {

	observation.time() = observationMsg.time;

	observation.state() = Eigen::Map<const Eigen::Matrix<float,STATE_DIM,1>> (
			observationMsg.state.value.data(), STATE_DIM).template cast<scalar_t>();

	observation.input() = Eigen::Map<const Eigen::Matrix<float,INPUT_DIM,1>> (
			observationMsg.input.value.data(), INPUT_DIM).template cast<scalar_t>();

	observation.subsystem() = observationMsg.subsystem;
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <size_t STATE_DIM, size_t INPUT_DIM>
template <class ContainerAllocator>
void RosMsgConversions<STATE_DIM, INPUT_DIM>::CreateModeSequenceMsg(
		const scalar_array_t& eventTimes,
		const size_array_t& subsystemsSequence,
		ocs2_comm_interfaces::mode_sequence_<ContainerAllocator>& modeSequenceMsg) {

	// event time sequence
	modeSequenceMsg.eventTimes.clear();
	modeSequenceMsg.eventTimes.reserve(eventTimes.size());
	for (const scalar_t& ti: eventTimes)
		modeSequenceMsg.eventTimes.push_back(ti);

	// subsystem sequence
	modeSequenceMsg.subsystems.clear();
	modeSequenceMsg.subsystems.reserve(subsystemsSequence.size());
	for (const size_t& si: subsystemsSequence)
		modeSequenceMsg.subsystems.push_back(si);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <size_t STATE_DIM, size_t INPUT_DIM>
template <class ContainerAllocator>
void RosMsgConversions<STATE_DIM, INPUT_DIM>::ReadModeSequenceMsg(
		const ocs2_comm_interfaces::mode_sequence_<ContainerAllocator>& modeSequenceMsg,
		scalar_array_t& eventTimes,
		size_array_t& subsystemsSequence) {

	const size_t numSubsystems = modeSequenceMsg.subsystems.size();
	if (modeSequenceMsg.eventTimes.size() != numSubsystems-1)
		throw std::runtime_error("The received message has incompatible "
				"array sizes for the eventTimes and subsystemsSequence.");

	// event time sequence
	eventTimes.clear();
	eventTimes.reserve(numSubsystems-1);
	for (const scalar_t& ti: modeSequenceMsg.eventTimes)
		eventTimes.push_back(ti);

	// subsystem sequence
	subsystemsSequence.clear();
	subsystemsSequence.reserve(numSubsystems);
	for (const size_t& si: modeSequenceMsg.subsystems)
		subsystemsSequence.push_back(si);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <size_t STATE_DIM, size_t INPUT_DIM>
template <class ContainerAllocator>
void RosMsgConversions<STATE_DIM, INPUT_DIM>::CreateTargetTrajectoriesMsg(
		const cost_desired_trajectories_t& costDesiredTrajectories,
		ocs2_comm_interfaces::mpc_target_trajectories_<ContainerAllocator>& targetTrajectoriesMsg) {

	const scalar_array_t& desiredTimeTrajectory = costDesiredTrajectories.desiredTimeTrajectory();
	const dynamic_vector_array_t& desiredStateTrajectory = costDesiredTrajectories.desiredStateTrajectory();
	const dynamic_vector_array_t& desiredInputTrajectory = costDesiredTrajectories.desiredInputTrajectory();

	// time and state
	size_t N = desiredStateTrajectory.size();
	targetTrajectoriesMsg.timeTrajectory.resize(N);
	targetTrajectoriesMsg.stateTrajectory.resize(N);
	for (size_t i=0; i<N; i++) {
		targetTrajectoriesMsg.timeTrajectory[i] = desiredTimeTrajectory[i];

		targetTrajectoriesMsg.stateTrajectory[i].value = std::vector<float>(desiredStateTrajectory[i].data(),
				desiredStateTrajectory[i].data()+desiredStateTrajectory[i].size());
	}  // end of i loop

	// input
	N = desiredInputTrajectory.size();
	targetTrajectoriesMsg.inputTrajectory.resize(N);
	for (size_t i=0; i<N; i++) {
		targetTrajectoriesMsg.inputTrajectory[i].value = std::vector<float>(desiredInputTrajectory[i].data(),
				desiredInputTrajectory[i].data()+desiredInputTrajectory[i].size());
	}  // end of i loop
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <size_t STATE_DIM, size_t INPUT_DIM>
template <class ContainerAllocator>
void RosMsgConversions<STATE_DIM, INPUT_DIM>::ReadTargetTrajectoriesMsg(
			const ocs2_comm_interfaces::mpc_target_trajectories_<ContainerAllocator>& targetTrajectoriesMsg,
			cost_desired_trajectories_t& costDesiredTrajectories) {

	scalar_array_t& desiredTimeTrajectory = costDesiredTrajectories.desiredTimeTrajectory();
	dynamic_vector_array_t& desiredStateTrajectory = costDesiredTrajectories.desiredStateTrajectory();
	dynamic_vector_array_t& desiredInputTrajectory = costDesiredTrajectories.desiredInputTrajectory();

	// state and time
	size_t N = targetTrajectoriesMsg.stateTrajectory.size();
	desiredTimeTrajectory.resize(N);
	desiredStateTrajectory.resize(N);
	for (size_t i=0; i<N; i++) {
		desiredTimeTrajectory[i] = targetTrajectoriesMsg.timeTrajectory[i];

		desiredStateTrajectory[i] = Eigen::Map<const Eigen::VectorXf>(targetTrajectoriesMsg.stateTrajectory[i].value.data(),
				targetTrajectoriesMsg.stateTrajectory[i].value.size()).template cast<scalar_t>();
	}  // end of i loop

	// input
	N = targetTrajectoriesMsg.inputTrajectory.size();
	desiredInputTrajectory.resize(N);
	for (size_t i=0; i<N; i++) {
		desiredInputTrajectory[i] = Eigen::Map<const Eigen::VectorXf>(targetTrajectoriesMsg.inputTrajectory[i].value.data(),
				targetTrajectoriesMsg.inputTrajectory[i].value.size()).template cast<scalar_t>();
	}  // end of i loop
}

} // namespace ocs2
