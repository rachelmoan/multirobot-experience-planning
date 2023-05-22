///
/// 
///

#include <boost/heap/pairing_heap.hpp>
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>

#include <string>
#include<boost/tokenizer.hpp>
#include <algorithm>    // std::shuffle
#include <random>      // std::default_random_engine
#include <chrono>       // std::chrono::system_clock

using boost::heap::pairing_heap;
using boost::heap::compare;
using boost::unordered_map;
using boost::unordered_set;
using namespace std;


class ProblemProblemInstance {
public:
	int m_numCols;
	int m_numRows;
	int m_mapSize;

	ProblemProblemInstance(){}
	ProblemProblemInstance(const std::string& _mapFileName, const std::string& _agentFileName, 
		int numAgents = 0, int numRows = 0, int numCols = 0, int numObstacles = 0, int warehouseWidth = 0);

	void printAgents() const;

    inline bool isObstacle(int loc) const { return obstacleMap[loc]; }
    inline bool validMove(int curr, int next) const;
    list<int> getNeighbors(int curr) const;

    inline int linearizeCoordinate(int row, int col) const { return ( this->m_numCols * row + col); }
    inline int getRowCoordinate(int id) const { return id / this->m_numCols; }
    inline int getColCoordinate(int id) const { return id % this->m_numCols; }
    inline pair<int, int> getCoordinate(int id) const { return make_pair(id / this->m_numCols, id % this->m_numCols); }
    inline int getCols() const { return m_numCols; }

    inline int getManhattanDistance(int loc1, int loc2) const {
        int loc1_x = getRowCoordinate(loc1);
        int loc1_y = getColCoordinate(loc1);
        int loc2_x = getRowCoordinate(loc2);
        int loc2_y = getColCoordinate(loc2);
        return abs(loc1_x - loc2_x) + abs(loc1_y - loc2_y);
    }

    inline int getManhattanDistance(const pair<int, int>& loc1, const pair<int, int>& loc2) const {
        return abs(loc1.first - loc2.first) + abs(loc1.second - loc2.second);
    }

	int getDegree(int loc) const
	{
		assert(loc >= 0 && loc < m_mapSize && !obstacleMap[loc]);
		int degree = 0;
		if (0 <= loc - m_numCols && !obstacleMap[loc - m_numCols])
			degree++;
		if (loc + m_numCols < m_mapSize && !obstacleMap[loc + m_numCols])
			degree++;
		if (loc % m_numCols > 0 && !obstacleMap[loc - 1])
			degree++;
		if (loc % m_numCols < m_numCols - 1 && !obstacleMap[loc + 1])
			degree++;
		return degree;
	}

	int getDefaultNumberOfAgents() const { return numAgents; }

private:

	  vector<bool> obstacleMap;
	  string m_mapFileName;
	  string m_agentFilename;

	  int numAgents;
	  vector<int> startLocations;
	  vector<int> goalLocations;

	  bool loadMap();
	  void printMap() const;
	  void saveMap() const;

	  bool loadAgents();
	  void saveAgents() const;

	  void generateConnectedRandomGrid(int rows, int cols, int obstacles); // initialize new [rows x cols] map with random obstacles
	  void generateRandomAgents(int warehouseWidth);
	  bool addObstacle(int obstacle); // add this obsatcle only if the map is still connected
	  bool isConnected(int start, int goal); // run BFS to find a path between start and goal, return true if a path exists.

	  int randomWalk(int loc, int steps) const;

	  // Class  SingleAgentSolver can access private members of Node 
	  friend class SingleAgentSolver;
};

bool ProblemInstance::loadMap() {
	using namespace boost;
	using namespace std;

	ifstream myfile(m_mapFileName.c_str());

	if (!myfile.is_open())
		return false;

	string line;
	tokenizer< char_separator<char> >::iterator beg;
	getline(myfile, line);
	
	char_separator<char> sep(",");
	tokenizer< char_separator<char> > tok(line, sep);
	beg = tok.begin();
	m_numRows = atoi((*beg).c_str()); // read number of rows
	beg++;
	m_numCols = atoi((*beg).c_str()); // read number of cols
	
	mapSize = m_numCols * m_numRows;

	m_obstacleMap.resize(mapSize, false);

	// read map (and start/goal locations)
	for (int i = 0; i < m_numRows; i++) {
		getline(myfile, line);
		for (int j = 0; j < m_numCols; j++) {
			my_map[linearizeCoordinate(i, j)] = (line[j] != '.');
		}
	}
	myfile.close();

	return true;
}


void ProblemInstance::printMap() const
{
	for (int i = 0; i< m_numRows; i++)
	{
		for (int j = 0; j < m_numCols; j++)
		{
			if (this->my_map[linearizeCoordinate(i, j)])
				std::cout << '@';
			else
				std::cout << '.';
		}
		std::cout << std::endl;
	}
}


void ProblemInstance::saveMap() const
{
	ofstream myfile;
	myfile.open(m_mapFileName);

	if (!myfile.is_open()) {
		std::cout << "Failed to save the map to " << m_mapFileName << std::endl;
		return;
	}

	myfile << m_numRows << "," << m_numCols << std::endl;

	for (int i = 0; i < m_numRows; i++) {
		for (int j = 0; j < m_numCols; j++) {
			if (my_map[linearizeCoordinate(i, j)])
				myfile << "@";
			else
				myfile << ".";
		}
		myfile << std::endl;
	}
	myfile.close();
}



