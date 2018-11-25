////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// STL A* Search implementation
// (C)2001 Justin Heyes-Jones
//
// Finding a path on a simple grid maze
// This shows how to do shortest path finding using A*

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Global data
// The world map
const int MAP_WIDTH = 20;
const int MAP_HEIGHT = 20;

int world_map[MAP_WIDTH * MAP_HEIGHT] =
	{
		// 0001020304050607080910111213141516171819
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 00
		1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 1, // 01
		1, 9, 9, 1, 1, 9, 9, 9, 1, 9, 1, 9, 1, 9, 1, 9, 9, 9, 1, 1, // 02
		1, 9, 9, 1, 1, 9, 9, 9, 1, 9, 1, 9, 1, 9, 1, 9, 9, 9, 1, 1, // 03
		1, 9, 1, 1, 1, 1, 9, 9, 1, 9, 1, 9, 1, 1, 1, 1, 9, 9, 1, 1, // 04
		1, 9, 1, 1, 9, 1, 1, 1, 1, 9, 1, 1, 1, 1, 9, 1, 1, 1, 1, 1, // 05
		1, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 1, 1, 1, 1, 1, // 06
		1, 9, 9, 9, 9, 9, 9, 9, 9, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 1, // 07
		1, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 1, 1, 1, 1, 1, 1, 1, 1, // 08
		1, 9, 1, 9, 9, 9, 9, 9, 9, 9, 1, 1, 9, 9, 9, 9, 9, 9, 9, 1, // 09
		1, 9, 1, 1, 1, 1, 9, 1, 1, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 10
		1, 9, 9, 9, 9, 9, 1, 9, 1, 9, 1, 9, 9, 9, 9, 9, 1, 1, 1, 1, // 11
		1, 9, 1, 9, 1, 9, 9, 9, 1, 9, 1, 9, 1, 9, 1, 9, 9, 9, 1, 1, // 12
		1, 9, 1, 9, 1, 9, 9, 9, 1, 9, 1, 9, 1, 9, 1, 9, 9, 9, 1, 1, // 13
		1, 9, 1, 1, 1, 1, 9, 9, 1, 9, 1, 9, 1, 1, 1, 1, 9, 9, 1, 1, // 14
		1, 9, 1, 1, 9, 1, 1, 1, 1, 9, 1, 1, 1, 1, 9, 1, 1, 1, 1, 1, // 15
		1, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 1, 1, 1, 1, 1, // 16
		1, 1, 9, 9, 9, 9, 9, 9, 9, 1, 1, 1, 9, 9, 9, 1, 9, 9, 9, 9, // 17
		1, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 1, 1, 1, 1, 1, 1, 1, 1, // 18
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 19

};

// map helper functions
int get_map_cost(int x, int y)
{
	if (x < 0 || x >= MAP_WIDTH ||
		y < 0 || y >= MAP_HEIGHT)
	{
		return 9;
	}
	return world_map[(y * MAP_WIDTH) + x];
}

class test_map_node* world_map_nodes[MAP_WIDTH][MAP_HEIGHT];
test_map_node* get_map_node(int x, int y)
{
	if (x < 0 || x >= MAP_WIDTH ||
		y < 0 || y >= MAP_HEIGHT)
	{
		return NULL;
	}
	return world_map_nodes[x][y];
}

class test_map_node
{
public:
	gv_int x; // the (x,y) positions of the node
	gv_int y;
	gv_long m_open_tag;
	gv_long m_close_tag;
	void* m_path_node;

	test_map_node()
	{
		x = y = 0;
		m_open_tag = m_close_tag = -1;
	}
	test_map_node(int px, int py)
	{
		x = px;
		y = py;
	}

	gv_int estimate_distance_to(test_map_node* node)
	{
		return gvt_abs(x - node->x) + gvt_abs(y - node->y);
	}; // Heuristic function which computes the estimated cost to the goal node

	bool get_connections(astar_path_finder< test_map_node, gv_int >* astarsearch, test_map_node* parent_node)
	{
		int parent_x = -1;
		int parent_y = -1;
		if (parent_node)
		{
			parent_x = parent_node->x;
			parent_y = parent_node->y;
		}
		test_map_node* neighbour_node;

		// push each possible move except allowing the search to go backwards

		if ((get_map_cost(x - 1, y) < 9) && !((parent_x == x - 1) && (parent_y == y)))
		{
			neighbour_node = get_map_node(x - 1, y);
			astarsearch->add_connections(neighbour_node);
			astarsearch->add_connectionCost(get_map_cost(x - 1, y));
		}

		if ((get_map_cost(x, y - 1) < 9) && !((parent_x == x) && (parent_y == y - 1)))
		{
			neighbour_node = get_map_node(x, y - 1);
			astarsearch->add_connections(neighbour_node);
			astarsearch->add_connectionCost(get_map_cost(x, y - 1));
		}

		if ((get_map_cost(x + 1, y) < 9) && !((parent_x == x + 1) && (parent_y == y)))
		{
			neighbour_node = get_map_node(x + 1, y);
			astarsearch->add_connections(neighbour_node);
			astarsearch->add_connectionCost(get_map_cost(x + 1, y));
		}

		if ((get_map_cost(x, y + 1) < 9) && !((parent_x == x) && (parent_y == y + 1)))
		{
			neighbour_node = get_map_node(x, y + 1);
			astarsearch->add_connections(neighbour_node);
			astarsearch->add_connectionCost(get_map_cost(x, y + 1));
		}

		return true;
	};
	// Retrieves all connections to this node and adds them via astarsearch.addSuccessor()
	gv_int distance_to_connection(test_map_node* connection)
	{
		return get_map_cost(x, y);
	};
	// Computes the cost of travelling from this node to the connection node
	bool is_same(test_map_node* r)
	{
		return x == r->x && y == r->y;
	}; // Returns true if this node is the same as the r node

	bool is_tag_for_open(gv_long tag)
	{
		return tag == m_open_tag;
	}
	bool is_tag_for_close(gv_long tag)
	{
		return tag == m_close_tag;
	}
	void tag_for_open(gv_long tag, void* node)
	{
		m_open_tag = tag;
		m_close_tag = -1;
		m_path_node = node;
	}
	void tag_for_close(gv_long tag, void* node)
	{
		m_close_tag = tag;
		m_open_tag = -1;
		m_path_node = node;
	}
	void* get_path_node()
	{
		return m_path_node;
	}
};

void create_map_nodes()
{
	for (int x = 0; x < MAP_WIDTH; x++)
		for (int y = 0; y < MAP_HEIGHT; y++)
			world_map_nodes[x][y] = new test_map_node(x, y);
}

void clear_map_nodes()
{
	for (int x = 0; x < MAP_WIDTH; x++)
		for (int y = 0; y < MAP_HEIGHT; y++)
			GVM_SAFE_DELETE(world_map_nodes[x][y]);
}

void main(gvt_array< gv_string >& args)
{

	// Our sample problem defines the world as a 2d array representing a terrain
	// Each element contains an integer from 0 to 5 which indicates the cost
	// of travel across the terrain. Zero means the least possible difficulty
	// in travelling (think ice rink if you can skate) whilst 5 represents the
	// most difficult. 9 indicates that we cannot pass.

	// Create an instance of the search class...

	astar_path_finder< test_map_node, gv_int > astarsearch;

	unsigned int SearchCount = 0;

	const unsigned int NumSearches = 10;

	create_map_nodes();

	while (SearchCount < NumSearches)
	{

		// Create a start state
		test_map_node* nodeStart = get_map_node(rand() % MAP_WIDTH, rand() % MAP_HEIGHT);
		// Define the goal state
		test_map_node* nodeEnd = get_map_node(rand() % MAP_WIDTH, rand() % MAP_HEIGHT);

		astarsearch.start(nodeStart, nodeEnd);

		unsigned int SearchState;
		unsigned int SearchSteps = 0;

		do
		{
			SearchState = astarsearch.step();

		} while (SearchState == SEARCH_STATE_SEARCHING);

		if (SearchState == SEARCH_STATE_SUCCEEDED)
		{
			GVM_CONSOLE_OUT("Search found goal state\n");
		}
		else if (SearchState == SEARCH_STATE_FAILED)
		{
			GVM_CONSOLE_OUT("Search terminated. Did not find goal state\n");
		}

		// Display the number of loops the search went through
		GVM_CONSOLE_OUT("SearchSteps :  " << astarsearch.get_steps() << " for " << nodeStart->x << " ," << nodeStart->y << " to " << nodeEnd->x << " ," << nodeEnd->y << " max node:" << astarsearch.m_max_node_used);
		SearchCount++;
	}
	clear_map_nodes();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
