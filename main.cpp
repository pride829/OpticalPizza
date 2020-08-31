/*
�ت��Gmain.cpp�O��ӱM�ת������A�b�����Ұ����Ʊ�����u���p��B�z�ݨD�ɶ��A�I�s�B�z���ֻݨD����k(function)�A�H�ε��w�Ӥ�k�һݨD
���������ҰѼơC
�ޥμ��Y�G���Fstandard io���~�Amain.cpp���ӥu�ޥ�vone.hpp�M<chrono>�Achrono�ΨӳB�z�p�ɡC

�b�C�@���ɮ�(*.cpp�Ϊ̬O*.hpp)�������ӭn���o�}�Y�����ѡA�ΨӪ�ܳo���ɮת��ت��H�Υ��ޥΤF���Ǽ��Y(header)�ɮסC
�p�G�A�n�s�g���ѡA�ШϥΥ��Ϊ��r��(�A�ֱ���Octrl + ,)�M�y��(�C�ֱ���Octrl + .)�A�o�ˤ���n�\Ū�C
�p�G�A�n�b���Ѥ����ϥΤ���}������������W���A�Цb����W������άA���[�W�^��W�١A�Ҧp������(virtual)����(optical)����(network)
�O�J(embedding)�C�]�����M�������n�g�A���^��W������n���C
*/
#include <iostream>
#include <chrono>
#include <sstream>
#include <fstream>
#include <string>
#include "vone.h"
#include "time.h"

int main()
{
	std::cout << "Processing start..." << std::endl;
	std::stringstream total_ck_result_s; // �����`�@�᪺�ɶ�
	
	double min_br_rate;
	int best_CAP_OR_LRC, best_FR_OR_PATH_FIRST;
	double best_alpha;

	for (int CAP_OR_LRC = 1; CAP_OR_LRC < 2; CAP_OR_LRC++) {
		for (int FR_OR_PATH_FIRST = 0; FR_OR_PATH_FIRST < 1; FR_OR_PATH_FIRST++) {
			std::stringstream best_result_s; //�����C��alpha�Ⱦ֦��̧C���_�v��
			std::stringstream alpha_result_s; //�����C��alpha�Ȫ��C�ӵ��G
			alpha_result_s << "Alpha,BR,Time" << std::endl;

			min_br_rate = 1;
			for (double alpha = 0; alpha <= 1; alpha += 0.1) {
				clock_t t = clock();
				std::string temp;
				temp = std::to_string(CAP_OR_LRC) +
					"_" +
					std::to_string(FR_OR_PATH_FIRST) +
					"_" +
					std::to_string(alpha);

				std::cout << "Proccessing with parameters " + temp << std::endl;

				double br_rate = vone(CAP_OR_LRC, FR_OR_PATH_FIRST, alpha);

				alpha_result_s << std::to_string(alpha) << "," <<
					std::to_string(br_rate) << "," <<
					std::to_string(clock() - t) << std::endl;

				if (br_rate < min_br_rate) {
					min_br_rate = br_rate;
					best_CAP_OR_LRC = CAP_OR_LRC;
					best_FR_OR_PATH_FIRST = FR_OR_PATH_FIRST;
					best_alpha = alpha;
				}

				std::cout << CAP_OR_LRC << "_" << FR_OR_PATH_FIRST << "_" << alpha <<
					":" << br_rate << std::endl;
			}

			std::cout << "BEST:" << best_CAP_OR_LRC << "_"
				<< best_FR_OR_PATH_FIRST << "_" <<
				best_alpha << ":" << min_br_rate << std::endl;

			best_result_s << "BEST:" << best_CAP_OR_LRC << "_"
				<< best_FR_OR_PATH_FIRST << "_" <<
				best_alpha << ":" << min_br_rate << std::endl;

			std::string file_name_temp;
			file_name_temp = "result_data/best/best_result_" +
				std::to_string(best_CAP_OR_LRC) +
				"_" +
				std::to_string(best_FR_OR_PATH_FIRST) +
				"_" +
				std::to_string(best_alpha) + ".csv";

			std::ofstream file_best_result(file_name_temp, std::ios_base::trunc);
			file_best_result << best_result_s.rdbuf();
			file_best_result.close();

			file_name_temp = "result_data/alpha/alpha_result_" +
				std::to_string(CAP_OR_LRC) +
				"_" +
				std::to_string(FR_OR_PATH_FIRST) + ".csv";

			std::ofstream alpha_result(file_name_temp, std::ios_base::trunc);
			alpha_result << alpha_result_s.rdbuf();
			alpha_result.close();

		}
	}

	system("PAUSE");

	return 0;
}