/*
各種enumeration和using name和常數的定義，我先放在這個header file中。
我不確定是否有更優雅的方法，因為我沒看過有人這樣做，看看之後如何。
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

using namespace boost; // 在所有原始碼中都使用boost的命名空間

// VertexProperties中有一個property，將vertex_name_t指定為std::string屬性
using VertexProperties = property < vertex_index_t, int, property< vertex_index2_t , int, property<vertex_name_t, int > > >;
using EdgeProperties = property < edge_index_t, int, property < edge_weight_t, int, property<edge_capacity_t, double > > >;

// hash_setS代表這個graph中不會有平行的邊
using Graph = adjacency_list< hash_setS, vecS, undirectedS, VertexProperties, EdgeProperties >;

using Vertex = graph_traits<Graph>::vertex_descriptor;		//以 Vertex 做為 vertex_descriptor 的別名
using Edge = graph_traits<Graph>::edge_descriptor;			//以 Edge 做為 edge_descriptor 的別名

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

