#include "names.h"
#include "VirtualRequestVector.h"
#include <fstream>
#include <iostream>
#include <string>
#include "time.h"
double vone(int CAP_OR_LRC, int FR_OR_PATH_FIRST, double alpha) {
	double final_br_rate;

	std::stringstream br_result_s;

	br_result_s << "ID,BR,CK" << std::endl;

	std::ifstream p_f("input_files/physical_network.txt");

	if (!p_f)
	{
		std::cout << "找不到physical_network file" << std::endl;
		return EXIT_FAILURE;
	}

	NetworkGraph p = NetworkGraph(p_f);

	std::ifstream v_f(INPUT_FILE_NAME);

	if (!v_f)
	{
		std::cout << "找不到virtual_requests file" << std::endl;
		return EXIT_FAILURE;
	}

	VirtualRequestVector v = VirtualRequestVector(v_f, alpha);

	double successed_count = 0;
	clock_t first_t = clock();
	for (int i = 0; i < v.v.size(); i++) {

		if (p.embed_request(v[i], CAP_OR_LRC, FR_OR_PATH_FIRST)) {
			successed_count += 1;

			if (C_OUTPUT_IS_SUCCESS)
				std::cout << "Embedding successed." << std::endl;
		}
		else {
			if (C_OUTPUT_IS_SUCCESS)
				std::cout << "Embedding failed." << std::endl;
		}

		if (i % COUNTY_CYCLE == 0 || i == v.v.size()-1) {
			double block_rate = (i != 0) ? (double)1 - (successed_count / (double)(i+1)) : (double)0;
			if (C_OUTPUT_BR_RATE)
				std::cout << "Block rate at req. " << i << " = " <<
				block_rate << std::endl;
			final_br_rate = block_rate;

			
			br_result_s << i << ", " << block_rate << ", " << clock() - first_t << std::endl;
		}
	}

	std::string file_name_temp;
	file_name_temp = "result_data/br/br_result_" +
		std::to_string(CAP_OR_LRC) +
		"_" +
		std::to_string(FR_OR_PATH_FIRST) +
		"_" +
		std::to_string(alpha) + ".csv";
	std::ofstream file_br_result(file_name_temp, std::ios_base::trunc);
	file_br_result << br_result_s.rdbuf();
	file_br_result.close();

	return final_br_rate;
}
