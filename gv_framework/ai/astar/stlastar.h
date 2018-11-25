/*
//SN: modified it a little bit ...

A* Algorithm Implementation using STL is
Copyright (C)2001-2005 Justin Heyes-Jones
Permission is given by the author to freely redistribute and 
include this code in any program as long as this credit is 
given where due.
  COVERED CODE IS PROVIDED UNDER THIS LICENSE ON AN "AS IS" BASIS, 
  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, 
  INCLUDING, WITHOUT LIMITATION, WARRANTIES THAT THE COVERED CODE 
  IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
  OR NON-INFRINGING. THE ENTIRE RISK AS TO THE QUALITY AND 
  PERFORMANCE OF THE COVERED CODE IS WITH YOU. SHOULD ANY COVERED 
  CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT THE INITIAL 
  DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY 
  NECESSARY SERVICING, REPAIR OR CORRECTION. THIS DISCLAIMER OF 
  WARRANTY CONSTITUTES AN ESSENTIAL PART OF THIS LICENSE. NO USE 
  OF ANY COVERED CODE IS AUTHORIZED HEREUNDER EXCEPT UNDER
  THIS DISCLAIMER.
  Use at your own risk!
*/
#pragma once 
#include <algorithm> 

enum
{
	SEARCH_STATE_NOT_INITIALISED,
	SEARCH_STATE_SEARCHING,
	SEARCH_STATE_SUCCEEDED,
	SEARCH_STATE_FAILED,
	SEARCH_STATE_OUT_OF_MEMORY,
	SEARCH_STATE_INVALID
};


// The AStar search class. type_of_map_node is the users state space type
template <class type_of_map_node, class type_of_cost> class astar_path_finder
{

public: // data

	// A node represents a possible state in the search
	// The user provided state type is included inside this type
public:
	class path_node 
	{
		public:
			type_of_cost f; // sum of cumulative cost of predecessors and self and heuristic
			type_of_cost g; // cost of this node + it's predecessors
			type_of_cost h; // heuristic estimate of distance to goal
			path_node   *parent; // used during the search to record the parent of connection nodes
			path_node() :parent( 0 ),g( 0 ),h( 0 ),f( 0 )	{}
			void	*	operator new(size_t size)
			{
				GV_ASSERT(size<=32);
				return gv_global::pool_32.allocate();
			};
			void		operator delete	(void * p)
			{
				gv_global::pool_32.free(p);
			};
			type_of_map_node * m_map_node;
	};


	// For sorting the heap the STL needs compare function that lets us compare
	// the f value of two nodes

	class node_comparator 
	{
		public:
			bool operator() ( const path_node *x, const path_node *y ) const
			{
				return x->f > y->f;
			}
	};

public: // methods
		// constructor just initialises private data
	astar_path_finder() :
		m_state( SEARCH_STATE_NOT_INITIALISED ),
		m_is_request_cancelled( false )
	{
		m_start = m_goal = NULL; 
		m_max_node_used=0;
		m_node_used=0;
	}

	~astar_path_finder()
	{
		reset();
	}

	void reset()
	{
		m_max_node_used = 0;
		m_node_used = 0;
		free_all_nodes();
	}
	
	// call at any time to cancel the search and free up all the memory
	void cancel_search()
	{
		m_is_request_cancelled = true;
	}

	void update_h(path_node * node)
	{
		node->h = node->m_map_node->estimate_distance_to(m_goal->m_map_node);
		node->f = node->g + node->h;
	}

	// Set start and goal states
	void start( type_of_map_node * start, type_of_map_node * end )
	{
		reset();
		m_is_request_cancelled = false;
		m_start = alloc_node();
		m_goal = alloc_node();
		GV_ASSERT((m_start != NULL && m_goal != NULL));
		m_start->m_map_node = start;
		m_goal->m_map_node = end;
		m_state = SEARCH_STATE_SEARCHING;
		// Initialise the AStar specific parts of the start path_node
		// The user only needs fill out the state information
		m_start->g = 0; 
		update_h(m_start);
		m_start->parent = 0;
		// Push the start node on the Open list
		m_open_list.push_back( m_start ); // heap now unsorted
		// Initialise counter for search steps
		m_steps = 0;
		m_tag_64 = gen_tag();
	}

	gv_long gen_tag()
	{
		static volatile gv_long t = 0; 
		++t;
		if (t < 0) t = 0; 
		return t; 
	}
	// Advances search one step 
	unsigned int step()
	{
		// Firstly break if the user has not initialised the search
		GV_ASSERT( (m_state > SEARCH_STATE_NOT_INITIALISED) &&
				(m_state < SEARCH_STATE_INVALID) );

		// Next I want it to be safe to do a searchstep once the search has succeeded...
		if( (m_state == SEARCH_STATE_SUCCEEDED) ||
			(m_state == SEARCH_STATE_FAILED) 
		  )
		{
			return m_state; 
		}

		// Failure is defined as emptying the open list as there is nothing left to 
		// search...
		// New: Allow user abort
		if( m_open_list.is_empty() || m_is_request_cancelled )
		{
			free_all_nodes();
			m_state = SEARCH_STATE_FAILED;
			return m_state;
		}
		
		// Incremement step count
		m_steps ++;

		// Pop the best node (the one with the lowest f) 
		path_node *n = *m_open_list.last(); // get pointer to the node
		//std::pop_heap( m_open_list.begin(), m_open_list.end(), node_comparator() );
		m_open_list.pop_back();

		// Check for the goal, once we pop that we're done
		if (n->m_map_node==m_goal->m_map_node)
		{
			// The user is going to use the end path_node he passed in 
			// so copy the parent pointer of n 
			m_goal->parent = n->parent;
			m_goal->g = n->g;

			m_state = SEARCH_STATE_SUCCEEDED;

			return m_state;
		}
		else // not goal
		{

			// We now need to generate the connections of this node
			// The user helps us to do this, and we keep the new nodes in
			// m_connections ...
			m_connections.clear_and_reserve(); // empty vector of connection nodes to n
			m_connectionsCost.clear_and_reserve();
			// User provides this functions and uses AddSuccessor to add each connection of
			// node 'n' to m_connections
			n->m_map_node->get_connections( this, n->parent ? n->parent->m_map_node : NULL ); 
			// Now handle each connection to the current node ...
			int ccount = m_connections.size();
			assert(ccount == m_connectionsCost.size());
			for (int i = 0; i < ccount; i++)
			{
				path_node* connection = m_connections[i];
				if (connection->m_map_node->is_tag_for_close(m_tag_64))	{
					continue;
				}

				// 	The g value for this connection ...
				type_of_cost newg = n->g + m_connectionsCost[i];// n->m_map_node->distance_to_connection(connection->m_map_node);
				// Now we need to find whether the node is on the open or closed lists
				// If it is but the node that is already on them is better (lower g)
				// then we can forget about this connection
				// First linear search of open list to find node
				path_node * open_node = NULL;
				if (connection->m_map_node->is_tag_for_open(m_tag_64))	{
					open_node = (path_node *)connection->m_map_node->get_path_node();
				}
				if (open_node)	{
					// we found this state on open
					if (open_node->g <= newg)	{
						continue;
					}
					else{
						m_open_list.erase_item(open_node);
					}

				}
				
				// This node is the best node so far with this particular state
				// so lets keep it and set up its AStar specific data ...
				connection->parent = n;
				connection->g = newg;
				update_h(connection);
				m_open_list.insert_in_sorted_array(connection, node_comparator(), true);
				connection->m_map_node->tag_for_open(m_tag_64, connection);

				//std::push_heap( m_open_list.begin(), m_open_list.end(), node_comparator() );
			}
			// push n onto Closed, as we have expanded it now
			n->m_map_node->tag_for_close(m_tag_64,n);
			m_closed_list.push_back( n );

		} // end else (not goal so expand)
		return m_state; // Succeeded bool is false at this point. 
	}

	// User calls this to add a connection to a list of connections
	// when expanding the search frontier
	bool add_connections( type_of_map_node * map_node)
	{
		path_node *node = alloc_node();
		if( node )
		{
			node->m_map_node = map_node;
			m_connections.push_back( node );
			return true;
		}
		return false;
	}

	bool add_connectionCost(type_of_cost cst)
	{
		m_connectionsCost.push_back(cst);
		return true;
	}

	// Get start node
	path_node *get_goal()
	{
		return m_goal;
	}
	
	// Get the number of steps
	int get_steps() { return m_steps; }

	
private: // methods
	void free_all_nodes()
	{
		// iterate open list and delete all nodes
		for (int i = 0; i < m_node_list.size(); i++)
		{
			delete m_node_list[i];
		}
	
		m_node_list.clear();
		m_open_list.clear();
		m_closed_list.clear();
		m_goal = m_start = NULL;
	}

	// path_node memory management
	path_node *alloc_node()
	{
		path_node *p = new path_node;
		m_node_list.push_back(p);
		m_node_used ++; 
		m_max_node_used = gvt_max(m_max_node_used, m_node_used);
		return p;
	}


public:
	int m_max_node_used;
	int m_node_used;
private: // data
	// Heap (simple vector but used as a heap, cf. Steve Rabin's game gems article)
	gvt_array< path_node *> m_node_list;;
	gvt_array< path_node *> m_open_list;

	// Closed list is a vector.
	gvt_array< path_node * > m_closed_list;

	// connections is a vector filled out by the user each type connections to a node
	// are generated
	gvt_array< path_node * > m_connections;
	gvt_array< type_of_cost > m_connectionsCost;

	// State
	unsigned int m_state;

	// Counts steps
	int m_steps;
	
	// start and goal state pointers
	path_node *m_start;
	path_node *m_goal;
	bool m_is_request_cancelled;
	gv_long m_tag_64;

};
/*
class astar_map_node
{
public:
	virtual type_of_cost estimate_distance_to(astar_map_node *) = 0;			// Heuristic function which computes the estimated cost to the goal node
	virtual bool get_connections(astar_path_finder<astar_map_node, type_of_cost> *astarsearch, astar_map_node *parent_node) = 0; // Retrieves all connections to this node and adds them via astarsearch.addSuccessor()
	virtual type_of_cost distance_to_connection(astar_map_node * connection) = 0; // Computes the cost of travelling from this node to the connection node
	virtual bool is_same(astar_map_node * r) = 0; // Returns true if this node is the same as the r node
};*/




   
