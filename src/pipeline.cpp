#include "pipeline.hpp"

Pipeline::Pipeline(std::string inlet, std::string outlet) {
	this->inlet = InletFactory::getInstance().get(inlet);
	this->outlet = OutletFactory::getInstance().get(outlet);
}

void Pipeline::addPipe(std::string pipe) {
	IPipe *pipeSegment = PipeFactory::getInstance().get(pipe);

	pipes.push_back(pipeSegment);
}
