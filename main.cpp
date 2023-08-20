#include <iostream>
#include <vector>
#include <set>
#include <cmath>
#include <chrono>

using namespace std;

struct Edge {
    int u;
    int v;
};

int n, m;
vector<int>  node_weight{};
vector<int>  node_degree{};
vector<Edge> edges{};

// ACO
int bkv, nv_bkv, n_ants;
double beta_val = 1;
double alpha    = 0.9;
double q0 = 0.9;
double tau0;
vector<int>  ant_position;
vector<bool> ant_keep_moving;

// solution
set<int> solution_node;
int solution_value;

void read_instance() {
    cin >> n >> m;
    node_weight.reserve(n);
    node_degree.reserve(n);
    int sum_w = 0;
    for (int i=0; i<n; i++) {
        cin >> node_weight[i];
        sum_w+=node_weight[i];
        node_degree[i] = 0;
    }
    solution_value = sum_w+2;
    for (int i=0; i<m; i++) {
        Edge e{};
        cin >> e.u >> e.v;
        node_degree[e.u]++;
        node_degree[e.v]++;
        edges.push_back(e);
    }
}

void initialize_values(vector<double>& tau_value, vector<set<int>>& solution_by_ant, vector<vector<int>>& c_value) {
    tau0 = double(n*(n-nv_bkv))/bkv;

    ant_position.reserve(n_ants);
    ant_keep_moving.reserve(n_ants);
    solution_by_ant.reserve(n_ants);

    tau_value.reserve(n);
    c_value.reserve(n_ants);

    for (int k = 0; k < n_ants; k++) {
        c_value[k] = vector<int>(n, 0);
        solution_by_ant[k] = set<int>();
    }

    for (int u = 0; u < n; u++) {
        tau_value[u] = tau0;
    }
}

void reset_c_values(vector<vector<int>>& c_value) {
    for (int k = 0; k < n_ants; ++k) {
        for (int u = 0; u < n; ++u) {
            c_value[k][u] = node_degree[u];
        }
    }
}

void set_ants_position(vector<set<int>>& solution_by_ant) {
    for (int k = 0; k < n_ants; ++k) {
        ant_position[k] = rand() % n;
        ant_keep_moving[k] = true;
        solution_by_ant[k].insert(ant_position[k]);
    }
}

double get_eta_value(int ant, int u, vector<vector<int>>& c_value) {
    return double(c_value[ant][u]) / node_weight[u];
}

int move_ant(int ant, vector<double>& tau_value, vector<vector<int>>& c_value) {
    vector<double> preference(n, 0);
    double sum_pref = 0;
    double max_pref = 0;
    int best_pos = 0;
    for (int u = 0; u < n; ++u) {
        preference[u] = tau_value[u] * pow(get_eta_value(ant, u, c_value), beta_val);
        sum_pref += preference[u];
        if (preference[u] > max_pref) {
            max_pref = preference[u];
            best_pos = u;
        }
    }
    // Choose a q
    double r = ((double)rand()/(double)RAND_MAX);
    if (r < q0) {
        ant_position[ant] = best_pos;
        return best_pos;
    } else {
        int u=0;
        while (true) {
            if (((double)rand()/(double)RAND_MAX) < preference[u]/sum_pref) {
                ant_position[ant] = u;
                return u;
            }
            u = (u+1) % n;
        }
    }
}

void update_c_value(int u, set<int>& ant_solution, vector<int>& ant_c_value) {
    ant_solution.insert(u);
    ant_c_value[u] = 0;
    for (auto edge : edges) {
        // v is the opposite node, or -1 if the edge does not contain u
        int v = (u==edge.u) ? edge.v : ((u==edge.v) ? edge.u : -1);
        if ((v != -1)) {
            // the edge is covered by u, so v doesn't need to cover it anymore
            ant_c_value[v] = max(0, ant_c_value[v]-1);
        }
    }
}

int ant_colony() {
    double delta_tau = 0;
    vector<double> tau_value{};
    vector<set<int>> solution_by_ant{};
    vector<vector<int>> c_value_by_ant{};

    initialize_values(tau_value, solution_by_ant, c_value_by_ant);

    int no_improve_counter = 0;
    int curr_best_val = solution_value;
    //  Cycle
    while (no_improve_counter < 100) {
        // Clear old solution
       for (int k=0; k<n_ants; ++k) {
           solution_by_ant[k].clear();
       }
       // Set psi values to node degree
        reset_c_values(c_value_by_ant);
       // Select initial position
       set_ants_position(solution_by_ant);

       for (int k = 0; k < n_ants; ++k) {
           update_c_value(ant_position[k], solution_by_ant[k], c_value_by_ant[k]);
       }
       // Step
       bool keep_moving = true;
       while(keep_moving) {
           for (int k = 0; k < n_ants; ++k) {
               if (not ant_keep_moving[k]) continue;
               // move ant
               int new_pos = move_ant(k, tau_value, c_value_by_ant);
               // Update solution and c_values for ant k
               solution_by_ant[k].insert(new_pos);
               update_c_value(new_pos, solution_by_ant[k], c_value_by_ant[k]);
           }
           // Update tau
           for (int node = 0; node < n; node++) {
               // node is in best solution
               if (solution_node.find(node) != solution_node.end())
                   tau_value[node] = (1 - alpha) * tau_value[node] + alpha * tau0;
               else
                   tau_value[node] = (1 - alpha) * tau_value[node];
           }

           // Update keep moving value
           keep_moving = false;
           for (int k = 0; k < n_ants; ++k) {
               if (not ant_keep_moving[k]) continue;
               int c_sum = 0;
               for (int u = 0; u < n; ++u) {
                   c_sum += c_value_by_ant[k][u];
               }
               ant_keep_moving[k] = c_sum > 0;
               keep_moving |= ant_keep_moving[k];
           }
       }

       for (int k=0; k<n_ants; ++k) {
           int ant_result = 0;
           for (int node : solution_by_ant[k]) {
               ant_result += node_weight[node];
           }
           if (ant_result < solution_value) {
               solution_node.clear();
               for (int node : solution_by_ant[k]) {
                   solution_node.insert(node);
               }
               solution_value = ant_result;
           }
       }

        no_improve_counter++;
        if (solution_value < curr_best_val) {
            curr_best_val = solution_value;
            no_improve_counter = 0;
            double sum_weight = 0;
            for (int u : solution_node) {
                sum_weight += node_weight[u];
            }
            delta_tau = 1 / sum_weight;
        }

        // Global preference update
        for (int u=0; u<n; u++) {
            // Tau
            if (solution_node.find(u) != solution_node.end()) {
                tau_value[u] = (1-alpha)*tau_value[u] + alpha*delta_tau;
            } else {
                tau_value[u] = (1-alpha)*tau_value[u];
            }
        }
    }

    return 0;
}

int main(int argc, char* argv[]) {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    if (argc != 5) {
        cerr << "usage: ./aco ants num_v_bkv bkv instance_name < instance" << endl;
        return -1;
    }
    n_ants = stoi(argv[1]);
    nv_bkv = stoi(argv[3]);
    bkv = stoi(argv[4]);
    string instance_name{argv[2]};
    srand(0xc0ff33);
    read_instance();
    auto t1 = high_resolution_clock::now();
    ant_colony();
    auto t2 = high_resolution_clock::now();
    duration<double, std::milli> ms_double = t2 - t1;
    cout << instance_name
         << "," << n_ants
         << "," << solution_node.size()
         << "," << solution_value
         << "," << ms_double.count()
         << endl;
    return 0;
}
