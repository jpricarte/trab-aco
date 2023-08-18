#include <iostream>
#include <vector>
#include <set>
#include <cmath>

using namespace std;

struct Edge {
    int u;
    int v;
};

int n, m;
vector<int>  node_weight{};
vector<Edge> edges{};

// ACO
int n_ants = 2;
double beta_val = 0.5;
double rho_val  = 0.9;
double tau0;
double q0 = 0.9;
vector<int>  ant_position;
vector<bool> ant_keep_moving;
vector< vector<int> > psi_value;
vector<double> tau_value;
vector< set<int>* > solution_by_ant;

// solution
set<int> solution_node;
int solution_value;

void read_instance() {
    cin >> n >> m;
    node_weight.reserve(n);
    for (int i=0; i<n; i++) {
        cin >> node_weight[i];
    }
    for (int i=0; i<m; i++) {
        Edge e{};
        cin >> e.u >> e.v;
        edges.push_back(e);
    }
}

void initialize_values() {
    tau0 = n*n;
    solution_value = m*m*m;
    solution_node = set<int>();
    ant_position = vector<int>(n_ants);
    ant_keep_moving = vector<bool>(n_ants);
    tau_value = vector<double>(n_ants);
    solution_by_ant = vector< set<int>* >(n_ants, nullptr);
    psi_value = vector< vector<int> >(n_ants, vector<int>(n));

    for (int k = 0; k < n_ants; k++) {
        psi_value[k] = vector<int>(n, 0);
    }

    for (int u = 0; u < n; u++) {
        tau_value[u] = tau0;
    }
}

void set_ants_position() {
    for (int k = 0; k < n_ants; ++k) {
        ant_position[k] = rand() % n;
        ant_keep_moving[k] = true;
        solution_by_ant[k]->insert(ant_position[k]);
    }
}

void reset_psi_values() {
    for (int k = 0; k < n_ants; ++k) {
        for (int u = 0; u < n; ++u) {
            psi_value[k][u] = 0;
        }
        for (auto edge : edges) {
            psi_value[k][edge.u] += 1;
            psi_value[k][edge.v] += 1;
        }
    }
}

int c_value(int ant, int u) {
    return psi_value[ant][u];
}

double get_eta_value(int ant, int u) {
    return double(c_value(ant, u)) / node_weight[u];
}

void move_ant(int ant) {
    int old_position = ant_position[ant];
    vector<double> preference(n, 0);
    double sum_pref = 0;
    double max_pref = 0;
    int best_pos = 5;
    for (int u = 0; u < n; ++u) {
        preference[u] = tau_value[u] * pow(get_eta_value(ant, u), beta_val);
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
    } else {
        for (int u=0; u<n; ++u) {
            if (((double)rand()/(double)RAND_MAX) < preference[u]/sum_pref) {
                ant_position[ant] = u;
                break;
            }
        }
    }
}

int ant_colony() {
    int no_improv_counter = 0;
    int curr_best_val = solution_value;
    //  Cycle
    while (no_improv_counter < 100) {
        for (int k=0; k<n_ants; ++k) {
            if (solution_by_ant[k]->size() > 0) {
                solution_by_ant[k]->clear();
            }
        }
        reset_psi_values();
        set_ants_position();
        for (int k = 0; k < n_ants; ++k) {
                int u = ant_position[k];
                psi_value[k][u] = max(0, psi_value[k][u]-1);
        }
        // Step
        bool keep_moving = true;
        while(keep_moving) {
            for (int k = 0; k < n_ants; ++k) {
                if (not ant_keep_moving[k]) continue;
                // move ant
                move_ant(k);
                // Update psi
                int u = ant_position[k];
                for (auto edge : edges) {
                    int v = (u==edge.u) ? edge.v : ((u==edge.v) ? edge.u : -1);
                    if (v != -1) {
                        if (solution_by_ant[k]->find(v) != solution_by_ant[k]->end()) {
                            psi_value[k][u] -= 1;
                            psi_value[k][v] -= 1;
                        }
                    }
                }
                solution_by_ant[k]->insert(u);

            }
            // Update tau
            for (int node=0; node<n; node++) {
                // node is in best solution
                if (solution_node.find(node) != solution_node.end())
                    tau_value[node] = (1-rho_val)*tau_value[node] + rho_val*tau0;
                else
                    tau_value[node] = (1-rho_val)*tau_value[node];
            }

            // Update keep moving value
            keep_moving = false;
            for (int k = 0; k < n_ants; ++k) {
                if (not ant_keep_moving[k]) continue;
                int c_sum = 0;
                for (int u = 0; u < n; ++u) {
                    c_sum += c_value(k, u);
                }
                ant_keep_moving[k] = c_sum > 0;
                keep_moving |= ant_keep_moving[k];
            }
        }

        for (int k=0; k<n_ants; ++k) {
            int ant_result = 0;
            for (int node : *solution_by_ant[k]) {
                ant_result += node_weight[node];
            }
            if (ant_result < solution_value) {
                solution_node.clear();
                for (int node : *solution_by_ant[k]) {
                    solution_node.insert(node);
                }
                solution_value = ant_result;
            }
        }
        no_improv_counter++;
        if (solution_value < curr_best_val) {
            curr_best_val = solution_value;
            no_improv_counter = 0;
        }
    }

    return 0;
}

int main() {
    read_instance();
    initialize_values();
    ant_colony();
    for(int node : solution_node) {
        cout << node << endl;
    }
    cout << solution_value << endl;
    return 0;
}
