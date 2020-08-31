#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/copy.hpp>
#include "names.h"
#include "NetworkGraph.h"
#include "yen_ksp.hpp"

template<typename T>
int mlvl(int type, T distance)
{
	if (type == 1) return 1;

	int m;

	if (distance <= 1200)
		m = 4;
	else if (distance <= 2400)
		m = 3;
	else if (distance <= 4800)
		m = 2;
	else if (distance <= 9600)
		m = 1;
	else
		m = 1;	

	return m;
}

// 實體網路的constructor
NetworkGraph::NetworkGraph(std::ifstream& f) {
	if(C_OUTPUT_CONS_PHY)
	std::cout << "###Constructing physical network...###" << std::endl;
	is_virtual = false;

	std::string line;
	int c;
	// 讀入所有的點和其容量
	std::getline(f, line);
	std::istringstream(line) >> c;
	// 新增所有實體節點
	int index = 0;
	while (c != -1) {

		Vertex u;
		u = add_vertex(g);
		//vertex_index_map[u] = index++;
		vertex_capacity_map[u] = c;

		//DEBUG 實體網路頂點
		if(C_OUTPUT_CONS_PHY)
		std::cout << "Vertex " << vertex_index_map[u] << " with capacity " << vertex_capacity_map[u] << " inserted." << std::endl;

		std::getline(f, line);

		std::istringstream(line) >> c;
	}
	// 新增所有實體鏈路

	while (!f.eof()) {

		int n1, n2, d;
		std::getline(f, line);
		std::istringstream(line) >> n1 >> n2 >> d;

		Vertex u, v;
		Edge e;
		bool inserted;
		u = search_by_index(n1);
		v = search_by_index(n2);
		tie(e, inserted) = add_edge(u, v, g);
		if (inserted) {
			edge_distance_map[e] = d;
			edge_index_map[e] = index++;
			//DEBUG 實體網路邊
			if(C_OUTPUT_CONS_PHY)
			std::cout << "Edge " << edge_index_map(e) << " with vertex " << vertex_index_map[u]
				<< " and vertex " << vertex_index_map[v] << " inserted. And its distance is " << edge_distance_map[e] << "." << std::endl;
		}
		else {
			//DEBUG 新增實體網路邊失敗
			if(C_OUTPUT_CONS_PHY)
			std::cout << "Failed to insert edge with vertex " << vertex_index_map[u]
				<< " and vertex " << vertex_index_map[v] << "." << std::endl;
		}
	}
	//std::cout << "Number of edges = " << num_edges(g) << std::endl; //DEBUG 顯示edge的數量

	//將每一個實體節點的capacity timeline全部設定為該節點的預設capacity。這就是在還沒有任何請求進來之前節點的處理能力。
	physical_capacity_timeline = (unsigned int**)malloc(num_vertices(g) * sizeof(unsigned int*));
	unsigned int *physical_capacity_timeline_storage = (unsigned int*)malloc(num_vertices(g) * TIMELINE_MAX * sizeof(unsigned int));
	for (std::pair<vertex_iterator, vertex_iterator> v_p = vertices(g); *v_p.first != *v_p.second; *v_p.first++) {
		physical_capacity_timeline[vertex_index_map[*v_p.first]] = &physical_capacity_timeline_storage[vertex_index_map[*v_p.first] * TIMELINE_MAX];
		for (int j = 0; j < TIMELINE_MAX; j++) {
			int temp = vertex_capacity_map[*v_p.first];
			//std::cout << (char)((int)temp + (int)'0') << std::endl;
			physical_capacity_timeline[vertex_index_map[*v_p.first]][j] = temp;
		}
	}

	//將每一個實體邊的每一個時間點的FS全部設定為0
	int edge_num = num_edges(g);
	FS_timeline = (char***)malloc(edge_num * sizeof(char**));
	for (int i = 0; i < edge_num; i++) {
		FS_timeline[i] = (char**)malloc(TIMELINE_MAX * sizeof(char*));
		for (int j = 0; j < TIMELINE_MAX; j++) {
			FS_timeline[i][j] = (char*)malloc(B * sizeof(char));
			for (int k = 0; k < B; k++)
				FS_timeline[i][j][k] = 0;
		}
	}

	//計算每一個實體節點的初始LRC
	for (std::pair<vertex_iterator, vertex_iterator> v_p = vertices(g); *v_p.first != *v_p.second; *v_p.first++) {
		vertex_LRC_map[*v_p.first] = physical_vertex_LRC(*v_p.first);
	}
	if(C_OUTPUT_CONS_PHY)
	std::cout << "###Finished constructing physical network.###" << std::endl << std::endl;
};	/* NetworkGraph() */

//虛擬網路的constructor
NetworkGraph::NetworkGraph(int time_properties[], std::vector<int> nodes_capacity, std::vector<std::vector<double>> links) {

	//DEBUG 開始建立虛擬網路
	//std::cout << "###Constructing virtual network request...###" << std::endl;

	is_virtual = true;
	virtual_total_LRC = 0;
	//新增時間資訊
	for (int i = 0; i < 4; i++)
		this->time_properties[i] = time_properties[i];

	bool inserted;

	Vertex u, v;
	//新增所有node和其capacity
	for (int c : nodes_capacity) {
		u = add_vertex(g);
		//虛擬節點capacity coumption
		vertex_capacity_map[u] = c;
		//vertex_index_map[u] = i; // 這一行顯示無法修改vertex_index_map的值，很奇怪，
		//應該是因為vertex_index_map在add_vertex的時候就按照順序新增了

		//DEBUG 新增一個節點到圖中
		//std::cout << "Inserted node " << vertex_index_map[u] << " with capacity consume " << vertex_capacity_map[u] << std::endl;
	}

	Edge e;

	// 將edges新增到g中
	int edge_index = 0;
	for (std::vector<double> l : links) {

		u = search_by_index((int)l[0]);
		v = search_by_index((int)l[1]);
		tie(e, inserted) = add_edge(u, v, g);

		if (inserted) {
			edge_BW_map[e] = l[2];
			edge_index_map[e] = edge_index++;
			//DEBUG 新增一個鏈路到圖中
			/*
			std::cout << "Insert link " << edge_index_map[e] << " with node " << vertex_index_map[u]
				<< " and node " << vertex_index_map[v] << " and its capacity is " << edge_BW_map[e] << std::endl;
			*/
		}
		else {
			//DEBUG 新增鏈路失敗
			std::cout << "Failed to insert link with node " << vertex_index_map[u]
				<< " and node " << vertex_index_map[v] << std::endl;
		}
	} /* for */

	//Traverse每一個虛擬點，並且計算其LRC
	for (std::pair<vertex_iterator, vertex_iterator> v_p = vertices(g); *v_p.first != *v_p.second; *v_p.first++) {
		double lrc = 0;
		for (std::pair<out_edge_iterator, out_edge_iterator> e_p = out_edges((*v_p.first), g); e_p.first != e_p.second; e_p.first++)
			lrc += edge_BW_map[*e_p.first];

		lrc *= vertex_capacity_map[*v_p.first];
		vertex_LRC_map[*v_p.first] = lrc;
		virtual_total_LRC += lrc;
		//DEBUG 輸出每一個虛擬點的LRC
		/*
		std::cout << "Node " << vertex_index_map[*v_p.first]
			<< " has LRC " << vertex_LRC_map[*v_p.first] << std::endl;
		*/
	}
	//DEBUG 建構虛擬網路結束
	//std::cout << "###Finished constructing virtual network request.###" << std::endl << std::endl;
}	/* NetworkGraph() */

NetworkGraph::~NetworkGraph() {
	//釋放指標空間
	free(FS_timeline);
	free(physical_capacity_timeline);
}

// 印出該實體或虛擬網路的資料，debug用。
void NetworkGraph::print() {

	if (is_virtual) {}
	else {
		// DEBUG: 顯示每一個實體節點的capacity timeline
		for (std::pair<vertex_iterator, vertex_iterator> v_p = vertices(g); *v_p.first != *v_p.second; *v_p.first++) {
			std::cout << "Capacity timeline of vertex " << vertex_index_map(*v_p.first) << ":" << std::endl;
			for (int i = 0; i < TIMELINE_MAX; i++)
				std::cout << physical_capacity_timeline[vertex_index_map(*v_p.first)][i] << " ";
			std::cout << std::endl;
		}
		std::cout << std::endl;

		// DEBUG: 顯示每一個實體邊的每一個時間點的FS，注意在B跟TIMELINE_MAX為正常的情況下，這會輸出非常非常多東西
		for (std::pair<edge_iterator, edge_iterator> e_p = edges(g); e_p.first != e_p.second; e_p.first++) {
			std::cout << "FS timeline of edge " << edge_index_map(*e_p.first) << std::setw(3) << " at all time:" << std::endl;
			for (int j = 0; j < TIMELINE_MAX; j++) {
				std::cout << "Time " << j << ": ";
				for (int i = 0; i < B; i++)
					std::cout << (char)(FS_timeline[edge_index_map(*e_p.first)][j][i] + (int)'0');
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
}

//將該網路印到檔案中
void NetworkGraph::print(std::string file_name) {

	std::stringstream result_ss;

	if (is_virtual) {}
	else {
		// DEBUG:顯示每一個實體節點的capacity timeline
		for (std::pair<vertex_iterator, vertex_iterator> v_p = vertices(g); *v_p.first != *v_p.second; *v_p.first++) {
			result_ss << "Capacity timeline of vertex " << vertex_index_map(*v_p.first) << ":" << std::endl;
			for (int i = 0; i < TIMELINE_MAX; i++)
				result_ss << physical_capacity_timeline[vertex_index_map(*v_p.first)][i] << " ";
			result_ss << std::endl;
		}
		result_ss << std::endl;

		// DEBUG: 顯示每一個實體邊的每一個時間點的FS，注意在B跟TIMELINE_MAX為正常的情況下，這會輸出非常非常多東西
		for (std::pair<edge_iterator, edge_iterator> e_p = edges(g); e_p.first != e_p.second; e_p.first++) {
			result_ss << "FS timeline of edge " << edge_index_map(*e_p.first) << std::setw(3) << " at all time:" << std::endl;
			for (int j = 0; j < TIMELINE_MAX; j++) {
				result_ss << "Time " << j << ": ";
				for (int i = 0; i < B; i++)
					result_ss << (char)(FS_timeline[edge_index_map(*e_p.first)][j][i] + (int)'0');
				result_ss << std::endl;
			}
			result_ss << std::endl;
		}
		result_ss << std::endl;
	}

	std::ofstream file_result(file_name, std::ios_base::trunc);
	file_result.rdbuf()->pubsetbuf(NULL, 102400);
	file_result << result_ss.rdbuf();
	file_result.close();
}

// 將虛擬網路in_g嵌入此實體網路
bool NetworkGraph::embed_request(NetworkGraph& in_g, 
								int CAP_OR_LRC, 
								int FR_OR_PATH_FIRST) {

	if (is_virtual) {
		std::cout << "Virtual network can not be embed." << std::endl;
		return false;
	}

	if (!in_g.is_virtual) {
		std::cout << "Embedding network has to be virtual." << std::endl;
		return false;
	}

	// 建立一個capacity_vec和vir_LRC_vec
	VertexCapacityMap in_g_capacity_map = get(vertex_name, in_g.g);
	VertexLRCMap in_g_LRC_map = get(vertex_index2, in_g.g);

	std::vector<int> capacity_vec = std::vector<int>();
	for (std::pair<vertex_iterator, vertex_iterator> v_p = vertices(in_g.g);
		*v_p.first != *v_p.second; *v_p.first++)
		capacity_vec.push_back(in_g_capacity_map[*v_p.first]);

	std::vector<int> vir_LRC_vec = std::vector<int>();
	for (std::pair<vertex_iterator, vertex_iterator> v_p = vertices(in_g.g);
		*v_p.first != *v_p.second; *v_p.first++)
		vir_LRC_vec.push_back(in_g_LRC_map[*v_p.first]);

	// cap_vec_max_ptr從大到小指向capacity_vec(或者說in_g.g的頂點)的位置
	int max, index;
	std::vector<int> cap_vec_max_ptr = std::vector<int>();
	if (CAP_OR_LRC == 0) { //CAP優先
		do {
			max = -1;
			index = -1;
			for (int i = 0; i < capacity_vec.size(); i++)
				if (capacity_vec[i] > max) {
					max = capacity_vec[i];
					index = i;
				}
			if (index != -1) {
				capacity_vec[index] = -1;
				cap_vec_max_ptr.push_back(index);
			}
		} while (index != -1);

		capacity_vec.clear();
		for (std::pair<vertex_iterator, vertex_iterator> v_p = vertices(in_g.g);
			*v_p.first != *v_p.second; *v_p.first++)
			capacity_vec.push_back(in_g_capacity_map[*v_p.first]);

	}
	else {
		do {
			max = -1;
			index = -1;
			for (int i = 0; i < vir_LRC_vec.size(); i++)
				if (vir_LRC_vec[i] > max) {
					max = vir_LRC_vec[i];
					index = i;
				}
			if (index != -1) {
				vir_LRC_vec[index] = -1;
				cap_vec_max_ptr.push_back(index);
			}
		} while (index != -1);

		vir_LRC_vec.clear();
		for (std::pair<vertex_iterator, vertex_iterator> v_p = vertices(in_g.g);
			*v_p.first != *v_p.second; *v_p.first++)
			vir_LRC_vec.push_back(in_g_LRC_map[*v_p.first]);

	}

	EdgeIndexMap g_edge_index_map = get(edge_index, g);

	//DEBUG std::cout << capacity_vec[cap_vec_max_ptr[0]] << std::endl;
	bool is_request_embed = false; //用來判斷這個需求是否成功嵌入，如果下面的for迴圈成功執行到最後，則是成功
	bool is_vertices_embed = false;
	int max_cap_required = capacity_vec[cap_vec_max_ptr[0]];
	std::vector<std::pair<int, int>> g_temp_vr_mapping = std::vector<std::pair<int, int>>();
	//std::cout << "max_cap_required: " << max_cap_required << std::endl;
	//巡覽每個時間點
	int t_em; //嵌入時間點(如果成功的話)
	for (int t = in_g.time_properties[Time_Attribute::time_started];
		t + in_g.time_properties[Time_Attribute::delta_time] < in_g.time_properties[Time_Attribute::time_ended];
		t++) {

		// max_cap_required = 最大的capacity需求

		Graph g_temp;
		copy_graph(g, g_temp);

		// 依序檢查g_temp中的每個點是否在時間區段t~t+delta_time中符合capacity限制。
		// 不符合者把他的邊刪除，因為刪除點會產生錯誤，我不知道為什麼。
		VertexIndexMap g_temp_vertex_index_map = get(vertex_index, g_temp);
		graph_traits<Graph>::vertex_iterator vi, vi_end, next;
		tie(vi, vi_end) = vertices(g_temp);
		for (next = vi; vi != vi_end; vi = next) {
			++next;
			bool pass = true;
			for (int t2 = t; t2 < t + in_g.time_properties[Time_Attribute::delta_time]; t2++) {
				if (physical_capacity_timeline[g_temp_vertex_index_map[*vi]][t2] < max_cap_required) {
					//std::cout << physical_capacity_timeline[g_temp_vertex_index_map[*vi]][t2] << std::endl;
					pass = false;
					break;
				}
			}
			if (!pass) {
				//std::cout << g_temp_vertex_index_map[*vi] << std::endl;
				clear_vertex(*vi, g_temp); // <-----------
				//std::cout << "Cleared!" << std::endl;
				//std::cout << degree(*vi, g_temp) << std::endl;
			}
			//std::cout << num_vertices(g_temp) << std::endl;
		}

		std::vector<int> LRC_vec = std::vector<int>();

		// 更新g_temp中的所有LRC
		VertexLRCMap g_temp_vertex_LRC_map = get(vertex_index2, g_temp);
		VertexCapacityMap g_temp_vertex_cap_map = get(vertex_name, g_temp);
		for (std::pair<vertex_iterator, vertex_iterator> v_p = vertices(g_temp);
			*v_p.first != *v_p.second; *v_p.first++) {
			int c = g_temp_vertex_cap_map[*v_p.first];
			int d = degree(*v_p.first, g_temp);
			g_temp_vertex_LRC_map[*v_p.first] = c * d;
			LRC_vec.push_back(g_temp_vertex_LRC_map[*v_p.first]);
		}

		/* // DEBUG
		for (std::pair<vertex_iterator, vertex_iterator> v_p = vertices(g_temp);
			*v_p.first != *v_p.second; *v_p.first++)
			std::cout << "Vertex " << g_temp_vertex_index_map[*v_p.first]
			<< " has LRC " << g_temp_vertex_LRC_map[*v_p.first] << std::endl;
		*/

		std::vector<int> LRC_vec_max_ptr = std::vector<int>();
		do {
			max = -1;
			index = -1;
			for (int i = 0; i < LRC_vec.size(); i++)
				if (LRC_vec[i] > max) {
					max = LRC_vec[i];
					index = i;
				}
			if (index != -1) {
				LRC_vec[index] = -1;
				LRC_vec_max_ptr.push_back(index);
			}
		} while (index != -1);

		std::vector<int> g_temp_cap_vec = std::vector<int>();
		VertexCapacityMap temp_temp_capacity_map = get(vertex_name, g_temp);
		for (std::pair<vertex_iterator, vertex_iterator> v_p = vertices(g_temp);
			*v_p.first != *v_p.second; *v_p.first++)
			g_temp_cap_vec.push_back(temp_temp_capacity_map[*v_p.first]);


		int j = 0;
		g_temp_vr_mapping.clear();
		bool embed = true;
		// LRC_vec_max_ptr[j]對應到實體網路 cap_vec_max_ptr[i]對應到虛擬網路
		for (int i = 0; i < capacity_vec.size(); i++) {
			if (g_temp_cap_vec[LRC_vec_max_ptr[j]] >= capacity_vec[cap_vec_max_ptr[i]])
				g_temp_vr_mapping.push_back({ cap_vec_max_ptr[i] , LRC_vec_max_ptr[j] });
			else
				i--;
			j++;

			if (j >= g_temp_cap_vec.size()) {
				// 此次嵌入點嵌入失敗
				embed = false;
				break;
			}
		}
		if (!embed) continue;
		is_vertices_embed = true;
		t_em = t;



		//**********將每條邊按照BW排序**********//
		// 建立一個BW_vec 為每一條邊的BW大小按照順序排列
		// FSC_vec_max_ptr從大到小指向BW_vec(或者說in_g.g的邊)的位置
		EdgeBWMap temp_BW_map = get(edge_capacity, in_g.g);
		std::vector<double> BW_vec = std::vector<double>();
		std::vector<Edge> edge_vec = std::vector<Edge>();
		for (std::pair<edge_iterator, edge_iterator> e_p = edges(in_g.g);
			e_p.first != e_p.second;
			e_p.first++) {
			BW_vec.push_back(temp_BW_map[*e_p.first]);
			edge_vec.push_back(*e_p.first);
		}

		std::vector<int> FSC_vec_max_ptr = std::vector<int>();
		do {
			max = -1;
			index = -1;
			for (int i = 0; i < BW_vec.size(); i++)
				if (BW_vec[i] > max) {
					max = BW_vec[i];
					index = i;
				}

			if (index != -1) {
				BW_vec[index] = -1;
				FSC_vec_max_ptr.push_back(index);
			}
		} while (index != -1);

		//重新推回BW_vec
		BW_vec.clear();
		for (std::pair<edge_iterator, edge_iterator> e_p = edges(in_g.g);
			e_p.first != e_p.second;
			e_p.first++)
			BW_vec.push_back(temp_BW_map[*e_p.first]);
		//**********將每條邊按照BW排序**********//


		//**********將虛擬點和實體點對應**********//
		//shortest_pairs代表有許多對點組成的向量，每一組都是實體點和虛擬點
		VertexIndexMap in_g_vertex_index_map = get(vertex_index, in_g.g);
		VertexIndexMap g_vertex_index_map = get(vertex_index, g);
		std::vector<std::pair<Vertex, Vertex>> shortest_pairs =
			std::vector<std::pair<Vertex, Vertex>>();
		// 巡覽in_g.g裡面的每一條邊
		for (std::pair<edge_iterator, edge_iterator> e_p = edges(in_g.g);
			e_p.first != e_p.second;
			e_p.first++) {

			Vertex s = source(*e_p.first, in_g.g);
			Vertex t = target(*e_p.first, in_g.g);

			std::pair<Vertex, Vertex> v_pair = std::pair<Vertex, Vertex>();
			//巡覽g_temp_vr_mapping，找出跟這條邊的s和t相同的點
			for (int i = 0; i < g_temp_vr_mapping.size(); i++) {

				if (g_temp_vr_mapping[i].first == in_g_vertex_index_map(s)) {

					int physical_index = g_temp_vr_mapping[i].second;

					for (std::pair<vertex_iterator, vertex_iterator> v_p = vertices(g);
						*v_p.first != *v_p.second;
						*v_p.first++) {

						if (g_temp_vr_mapping[i].second == g_vertex_index_map[*v_p.first]) {
							v_pair.first = *v_p.first;
							break;
						}
					}
				}
				if (g_temp_vr_mapping[i].first == in_g_vertex_index_map(t)) {

					int physical_index = g_temp_vr_mapping[i].second;

					for (std::pair<vertex_iterator, vertex_iterator> v_p = vertices(g);
						*v_p.first != *v_p.second;
						*v_p.first++) {

						if (g_temp_vr_mapping[i].second == g_vertex_index_map[*v_p.first]) {
							v_pair.second = *v_p.first;

							break;
						}
					}
				}
			}
			shortest_pairs.push_back(v_pair);
		}
		//**********將虛擬點和實體點對應**********//


		EdgeDistanceMap in_g_distance_map = get(edge_weight, in_g.g);
		EdgeIndexMap in_g_edge_index_map = get(edge_index, in_g.g);
		EdgeBWMap in_g_edge_BW_map = get(edge_capacity, in_g.g);
		EdgeDistanceMap g_edge_distance_map = get(edge_weight, g);
		int embed_a;
		int path_index;
		using Path = std::list<boost::detail::edge_desc_impl<boost::undirected_tag, unsigned int>, std::allocator<boost::detail::edge_desc_impl<boost::undirected_tag, unsigned int> > >;
		//PNS由{path, int, int}組成
		using PNS = std::tuple<
			Path,
			int,
			int>;

		std::vector<PNS> path_and_starts = std::vector<PNS>();

		//巡覽每一條虛擬邊，因為每一條邊都要嵌入
		for (std::pair<edge_iterator, edge_iterator> e_p = edges(in_g.g);
			e_p.first != e_p.second;
			e_p.first++) {

			PNS temp_PNS = PNS();
			embed_a = -1; //embed_a即為嵌入的那個時間點
			int in_g_edge_index = in_g_edge_index_map[*e_p.first];

			auto yen = yen_ksp(g,
				shortest_pairs[in_g_edge_index].first,
				shortest_pairs[in_g_edge_index].second,
				g_edge_distance_map,
				g_vertex_index_map,
				K_PATH);

			double BW = in_g_edge_BW_map[*e_p.first];

			//巡覽yen回傳的每一個path
			if (FR_OR_PATH_FIRST == 0)
				for (int a = 0; a < B; a++) {
					bool path_embed = false;
					Path embed_path;
					int embed_FSC;
					for (auto weight_and_path : yen) {
						path_index = 0;
						int m = mlvl(1, weight_and_path.first);
						int FSC = BW / Cslot / m + 2;

						bool flag = true;
						//巡覽path中的每一條邊
						for (auto edge : weight_and_path.second) {
							//依序巡覽並檢查每一個頻譜槽是否可用
							for (int b = a; b < a + FSC; b++) {
								if (b >= B) {
									flag = false;
									break;
								}
								//依序巡覽某一個特定的頻譜槽在所有時間區間內都是否可用
								for (int t_j = t;
									(t_j < t + in_g.time_properties[Time_Attribute::delta_time]);
									t_j++) {
									//如果不能的話，flag = false，跳出檢查時間區間
									if (t_j >= TIMELINE_MAX || FS_timeline[g_edge_index_map[edge]][t_j][b] != 0) {
										flag = false;
										break;
									}

								}
								if (!flag) //跳出檢查每一個頻譜槽
									break;
							}
							if (!flag) //跳出檢查每一條邊
								break;
						}
						if (flag) { //如果flag==true，則找到合適的embed_a並且跳出，反之則a++
							embed_a = a;
							embed_path = weight_and_path.second;
							embed_FSC = FSC;
							break;
						}
					} /*巡覽每條路徑 嘗試嵌入*/
					//如果embed_a == -1，代表該頻譜槽無法嵌入實體圖
					if (embed_a == -1) {
						continue;
					}
					else { //反之則將該路徑，嵌入頻譜，以及嵌入長度(FSC)記錄下來，並且中斷巡覽頻譜槽的迴圈
						std::get<0>(temp_PNS) = embed_path;
						std::get<1>(temp_PNS) = embed_a;
						std::get<2>(temp_PNS) = embed_FSC;
						path_embed = true;
						break;
					}
				} /* 從a 到 B-FSC */
			else
				for (auto weight_and_path : yen)
				{
					path_index = 0;
					int m = mlvl(1, weight_and_path.first);
					//int FSC = ((BW / m) + 1) + 1; //後面的+1即為guardband
					int FSC = BW / Cslot / m + 2;
					//巡覽每一個可能的頻譜槽區塊開始
					for (int a = 0; a < B - FSC; a++) {
						bool flag = true;
						//巡覽path中的每一條邊
						for (auto edge : weight_and_path.second) {
							//依序巡覽並檢查每一個頻譜槽是否可用
							for (int b = a; b < a + FSC; b++) {
								//依序巡覽某一個特定的頻譜槽在所有時間區間內都是否可用
								for (int t_j = t;
									(t_j < t + in_g.time_properties[Time_Attribute::delta_time]) && (t_j < TIMELINE_MAX);
									t_j++) {
									//如果不能的話，flag = false，跳出檢查時間區間
									if (FS_timeline[g_edge_index_map[edge]][t_j][b] != 0) {
										flag = false;
										break;
									}
								}
								if (!flag) //跳出檢查每一個頻譜槽
									break;
							}
							if (!flag) //跳出檢查每一條邊
								break;
						}
						if (flag) { //如果flag==true，則找到合適的embed_a並且跳出，反之則a++
							embed_a = a;
							break;
						}
					} /* 從a 到 B-FSC */

					//如果embed_a == -1，代表該路徑無法嵌入實體圖
					if (embed_a == -1)
						path_index++;
					else { //反之則將該路徑，嵌入頻譜，以及嵌入長度(FSC)記錄下來，並且中斷巡覽路徑的迴圈
						std::get<0>(temp_PNS) = weight_and_path.second;
						std::get<1>(temp_PNS) = embed_a;
						std::get<2>(temp_PNS) = FSC;
						break;
					}
				} /*巡覽每條路徑 嘗試嵌入*/

				//所有路徑都失敗
			if (embed_a == -1) { // 從實體點u到實體點v之間沒有可用路徑存在，前往下一個時間點t+1
				break;
			}

			//反之，則將正確的路徑，開始的頻譜槽位置，以及FSC記錄下來
			path_and_starts.push_back(temp_PNS);

		}/* 巡覽每條虛擬需求*/

		if (embed_a == -1) // 這個時間失敗了
			continue;

		/*
				for (std::pair<vertex_iterator, vertex_iterator> v_p = vertices(g); *v_p.first != *v_p.second; *v_p.first++) {
			std::cout << "Capacity timeline of vertex " << vertex_index_map(*v_p.first) << ":" << std::endl;
			for (int i = 0; i < TIMELINE_MAX; i++)
				std::cout << physical_capacity_timeline[vertex_index_map(*v_p.first)][i] << " ";
			std::cout << std::endl;
		}
		*/

		for (std::pair<int, int> vertex_index_pair : g_temp_vr_mapping) {
			for (int i = t; i < t + in_g.time_properties[Time_Attribute::delta_time]; i++)
				physical_capacity_timeline[vertex_index_pair.second][i] -=
				in_g_capacity_map[vertex_index_pair.first];
		}

		//巡覽每一個已嵌入路徑
		for (auto pns : path_and_starts) {
			/* // DEBUG : 輸出嵌入點
			std::cout << "pns.second = " << pns.second << " & t = " << t << std::endl;
			for (auto edge : pns.first) {
				std::cout << g_edge_index_map[edge] << ":";
			}
			std::cout << std::endl;
			*/
			//巡覽該路徑上的每一條邊
			for (auto edge : std::get<0>(pns)) {
				//DEBUG 輸出
				if (C_OUTPUT_EMBEDDING_PATH)
					std::cout << "嵌入點 " << g_vertex_index_map[source(edge, g)] << " 到點 " <<
					g_vertex_index_map[target(edge, g)] << " 從時間 " << t << " 開始持續 " <<
					in_g.time_properties[Time_Attribute::delta_time] << " 的第 " <<
					std::get<1>(pns) << " 到 " << std::get<1>(pns) + std::get<2>(pns) << " 格" << std::endl;

				for (int t_j = t;
					t_j < t + in_g.time_properties[Time_Attribute::delta_time] && t_j < TIMELINE_MAX;
					t_j++) {

					for (int a_i = std::get<1>(pns); a_i < std::get<1>(pns) + std::get<2>(pns); a_i++) {
						FS_timeline[g_edge_index_map[edge]][t_j][a_i] = 1;
					}
				}
			}
			if (C_OUTPUT_EMBEDDING_PATH)
				std::cout << std::endl;
		}
		is_request_embed = true;
		break;
	} /* Time while */

	if (!is_request_embed) {
		if (C_OUTPUT_FAILED_TYPE) {
			if (is_vertices_embed)
				std::cout << "Vertices embedding failed." << std::endl;
			else
				std::cout << "Path embedding failed." << std::endl;
		}
	}
	return is_request_embed;
}

// 計算單一實體節點的LRC
int NetworkGraph::physical_vertex_LRC(Vertex u) {

	if (is_virtual) {
		std::cout << "Warning: Tried to compute virtual vertex LRC with physical function." << std::endl;
		return -1;
	}
	return degree(u, g)* vertex_capacity_map[u];
}

Vertex NetworkGraph::search_by_index(int index) {
	for (std::pair<vertex_iterator, vertex_iterator> v_p = vertices(g); *v_p.first != *v_p.second; *v_p.first++)
		if (vertex_index_map[*v_p.first] == index)
			return *v_p.first;
	return NULL;
}

void NetworkGraph::set_L_x(double in_L_x) {
	L_x = in_L_x;

}
void NetworkGraph::set_T_x(double in_T_x) {
	T_x = in_T_x;
}
void NetworkGraph::set_P_x(double in_P_x) {
	P_x = in_P_x;

}