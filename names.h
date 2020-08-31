/*
�U��enumeration�Musing name�M�`�ƪ��w�q�A�ڥ���b�o��header file���C
�ڤ��T�w�O�_�����u������k�A�]���ڨS�ݹL���H�o�˰��A�ݬݤ���p��C
*/
#pragma once
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/utility.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/tuple/tuple.hpp>

static int B = 100;
static int TIMELINE_MAX = 1000;
static int K_PATH = 5;
static int COUNTY_CYCLE = 50;
static double Cslot = 12.5;
static int MLVL_TYPE = 1;
static bool C_OUTPUT_EMBEDDING_PATH = false;
static bool C_OUTPUT_IS_SUCCESS = false;
static bool C_OUTPUT_FAILED_TYPE = false;
static bool C_OUTPUT_CONS_PHY = false;
static bool C_OUTPUT_BR_RATE = true;
static std::string INPUT_FILE_NAME = std::string("input_files/virtual_requests_501.txt");

using namespace boost; // �b�Ҧ���l�X�����ϥ�boost���R�W�Ŷ�

// VertexProperties�����@��property�A�Nvertex_name_t���w��std::string�ݩ�
using VertexProperties = property < vertex_index_t, int, property< vertex_index2_t , int, property<vertex_name_t, int > > >;
using EdgeProperties = property < edge_index_t, int, property < edge_weight_t, int, property<edge_capacity_t, double > > >;

// hash_setS�N��o��graph�����|�����檺��
using Graph = adjacency_list< hash_setS, vecS, undirectedS, VertexProperties, EdgeProperties >;

using Vertex = graph_traits<Graph>::vertex_descriptor;		//�H Vertex ���� vertex_descriptor ���O�W
using Edge = graph_traits<Graph>::edge_descriptor;			//�H Edge ���� edge_descriptor ���O�W

using VertexIndexMap = property_map<Graph, vertex_index_t>::type;
using VertexCapacityMap = property_map<Graph, vertex_name_t>::type;
using VertexLRCMap = property_map<Graph, vertex_index2_t>::type;
using EdgeIndexMap = property_map<Graph, edge_index_t>::type;
using EdgeDistanceMap = property_map<Graph, edge_weight_t>::type; 
using EdgeBWMap = property_map<Graph, edge_capacity_t>::type;

typedef typename boost::graph_traits<Graph>::edge_iterator edge_iterator;
typedef typename boost::graph_traits<Graph>::vertex_iterator vertex_iterator;
typedef typename boost::graph_traits<Graph>::out_edge_iterator out_edge_iterator;

enum Time_Attribute { time_arrived, time_started, delta_time, time_ended };

