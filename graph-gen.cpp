 /**************************************\
 *	     GRAPH GENERATOR.				*
 *	created by Jpricarte, 2023			*
 *  feel free to use anyway you want   	*
 *	params: n m seed, where:			*
 *	n = number of vertices				*
 *	m = number of edges					*
 *  seed = you know, the random seed	* 
 \**************************************/

////////////////////////////////////////////
/// Output format <this is a comment>:	  //
///					  //
/// n m       <num of vertices and edges> //
/// w_1       <weight of first vertex>    //
/// w_2					  //
/// ...					  //
/// w_n       <weight of last vertex>	  //
/// n_u1 n_v2 <first edge>		  //
/// ...					  //
/// n_um n_vm <last edge>		  //
///					  //
////////////////////////////////////////////

#include <iostream>
#include <random>
#include <string>
#include <algorithm>
#include <cmath>

using namespace std;

const int c_nMinWeigth 	     = 20;
const int c_nPossibleWeigths = 100;

int n, m;
long int seed;

struct Edge {
	int u;
	int v;
};

int main(int argc, char* argv[]) {
	// define params
	n = stoi(argv[1]);
	m = stoi(argv[2]);
	seed = stol(argv[3]);
	srand(seed);

	// defining variables to generate graph
    vector<int> lstNodeDegree(n, 0);

	// Generating edges
	vector<Edge> lstEdges{};
	int k=0;
	while(k < m) {
		int u = random() % n;
		int v;
		do { v = random() % n; } while (u==v);
		if (u > v)
			swap(u, v);
        
        lstNodeDegree[u]++;
        lstNodeDegree[v]++;
		Edge e {u, v};

        bool bExists = false;
        for (auto el : lstEdges) {
            if ((e.u==el.u) and (e.v==el.v) ){
                bExists = true;
                break;
            }
        }
        if (not bExists) {
            lstEdges.push_back(e);
            k++;
        }
    }

    // printing values
    cout << n << " " << m << endl;

    for (int k=0; k<n; k++) {
        cout << (random() % c_nPossibleWeigths) + c_nMinWeigth << endl;
    }

	for (auto e : lstEdges) {
		cout << e.u << " " << e.v << endl;
	}

	return 0;
}

