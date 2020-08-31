/*
目的：main.cpp是整個專案的介面，在之中所做的事情應當只有計算處理需求時間，呼叫處理光纖需求的方法(function)，以及給定該方法所需求
的實驗環境參數。
引用標頭：除了standard io之外，main.cpp應該只引用vone.hpp和<chrono>，chrono用來處理計時。

在每一個檔案(*.cpp或者是*.hpp)中都應該要有這開頭的註解，用來表示這個檔案的目的以及它引用了那些標頭(header)檔案。
如果你要編寫註解，請使用全形的逗號(，快捷鍵是ctrl + ,)和句號(。快捷鍵是ctrl + .)，這樣比較好閱讀。
如果你要在註解之中使用比較艱澀難懂的中文名詞，請在中文名詞之後用括號加上英文名稱，例如說虛擬(virtual)光學(optical)網路(network)
嵌入(embedding)。因為雖然中文比較好寫，但英文名詞比較好懂。
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
	std::stringstream total_ck_result_s; // 紀錄總共花的時間
	
	double min_br_rate;
	int best_CAP_OR_LRC, best_FR_OR_PATH_FIRST;
	double best_alpha;

	for (int CAP_OR_LRC = 1; CAP_OR_LRC < 2; CAP_OR_LRC++) {
		for (int FR_OR_PATH_FIRST = 0; FR_OR_PATH_FIRST < 1; FR_OR_PATH_FIRST++) {
			std::stringstream best_result_s; //紀錄每個alpha值擁有最低阻斷率者
			std::stringstream alpha_result_s; //紀錄每個alpha值的每個結果
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