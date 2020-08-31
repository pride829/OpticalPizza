#pragma once
#include "names.h"
class NetworkGraph
{
public:
	bool is_virtual;
	int time_properties[4];

	//這裡f必須用指標的方式傳入
	NetworkGraph(std::ifstream& f);
	NetworkGraph(int time_properties[], std::vector<int> nodes_capacity, std::vector<std::vector<double>> links);
	~NetworkGraph();
	int virtual_total_LRC;
	double L_x, T_x, P_x;
	bool NetworkGraph::embed_request(NetworkGraph& in_g,
		int CAP_OR_LRC,
		int FR_OR_PATH_FIRST);
	Vertex search_by_index(int index);
	void print();
	void print(std::string file_name);
	void set_L_x(double in_L_x);
	void set_T_x(double in_T_x);
	void set_P_x(double in_P_x);
private:
	Graph g;
	VertexIndexMap vertex_index_map = get(vertex_index, g);
	VertexCapacityMap vertex_capacity_map = get(vertex_name, g);
	VertexLRCMap vertex_LRC_map = get(vertex_index2, g);
	EdgeIndexMap edge_index_map = get(edge_index, g);
	EdgeDistanceMap edge_distance_map = get(edge_weight, g);
	EdgeBWMap edge_BW_map = get(edge_capacity, g);
	
	int edge_count;
	int vertex_count;

	unsigned int **physical_capacity_timeline;
	char ***FS_timeline;
	int physical_vertex_LRC(Vertex);

};