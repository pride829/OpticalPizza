#pragma once
#include <vector>
#include "NetworkGraph.h"
class VirtualRequestVector
{
public:

	std::vector<NetworkGraph> v;

	int max_LRC, min_LRC, max_time, min_time;

	VirtualRequestVector(std::ifstream& f, double alpha);

	NetworkGraph operator[](int index);
};

