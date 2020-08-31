#include <vector>
#include <iostream>
#include <fstream>
#include "NetworkGraph.h"
#include "VirtualRequestVector.h"

bool compareNetworkGraph(NetworkGraph i1, NetworkGraph i2)
{
	return (i1.P_x < i2.P_x);
}

// �qf���إߦh�ӵ��������ШD�զ���vector
VirtualRequestVector::VirtualRequestVector(std::ifstream& f, double alpha) {

	max_LRC = -1;
	min_LRC = INT_MAX;
	max_time = -1;
	min_time = INT_MAX;
	v = std::vector<NetworkGraph>();

	while (!f.eof()) {

		std::string line;
		std::getline(f, line);

		// Ū�J�ɶ��ƭ�
		int time_properties[4];
		std::istringstream(line) >> time_properties[Time_Attribute::time_arrived] >> time_properties[Time_Attribute::time_started]
			>> time_properties[Time_Attribute::delta_time] >> time_properties[Time_Attribute::time_ended];

		/*std::cout << time_properties[Time_Attribute::time_arrived] << " " << time_properties[Time_Attribute::time_started]
			<< " " << time_properties[Time_Attribute::delta_time] << " " << time_properties[Time_Attribute::time_ended] << std::endl;*/ // debug

			// Ū�J�C�@��node��capacity
		std::vector<int> nodes_capacity = std::vector<int>();
		for (int temp = 0; std::getline(f, line);) {

			std::istringstream(line) >> temp;
			if (temp == -1) break;

			nodes_capacity.push_back(temp);
		}

		// Ū�J�C�@��link�M��BW
		std::vector<std::vector<double>> links = std::vector<std::vector<double>>();
		for (double temp[3]; std::getline(f, line);) {

			std::istringstream(line) >> temp[0] >> temp[1] >> temp[2];
			if (temp[0] == -1) break;

			//std::istringstream(line) >> temp[1] >> temp[2]; // �o�˪��g�k�O�|�X�����A���ڤ����D������C

			//std::cout << temp[0] << " " << temp[1] << " " << temp[2] << std::endl; // DEBUG

			std::vector<double> temp_vector = std::vector<double>();
			temp_vector.push_back(temp[0]);
			temp_vector.push_back(temp[1]);
			temp_vector.push_back(temp[2]);

			links.push_back(temp_vector);
		}
		NetworkGraph temp_g = NetworkGraph(time_properties, nodes_capacity, links);
		if (temp_g.virtual_total_LRC > max_LRC)
			max_LRC = temp_g.virtual_total_LRC;
		
		if(temp_g.virtual_total_LRC < min_LRC)
			min_LRC = temp_g.virtual_total_LRC;

		if (temp_g.time_properties[time_started] > max_time)
			max_time = temp_g.time_properties[time_started];

		if (temp_g.time_properties[time_started] < min_time)
			min_time = temp_g.time_properties[time_started];

		v.push_back(temp_g);

	} /* while */
	
	for (int i = 0; i < v.size(); i++) {
		
		double L_x = ((double)v[i].virtual_total_LRC - (double)min_LRC) / (((double)max_LRC - (double)min_LRC)) ;

		int ts = v[i].time_properties[time_started];

		double T_x = ( (double)max_time - (double)ts ) / ( (double)max_time - (double)min_time );
		
		v[i].set_L_x(L_x);
		v[i].set_T_x(T_x);
		v[i].set_P_x(alpha * L_x + (1 - alpha) * T_x);
		//DEBUG
		//std::cout << v[i].L_x << " " << v[i].T_x << " " << v[i].P_x << std::endl;
	}
	
	std::sort(v.begin(), v.end(), compareNetworkGraph); //����P_x�Ƨ�

} /* VirtualRequestVector */

// ����[]�B��l �o�ˤ�����[
NetworkGraph VirtualRequestVector::operator[](int index) {
	return v[index];
}