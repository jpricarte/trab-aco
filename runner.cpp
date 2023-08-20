#include <iostream>
#include <csignal>
#include <fstream>
#include <sstream>
#include <vector>
#include <sys/wait.h>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "usage: ./runner ants instance_dir result_file" << endl;
    }
    fstream results{argv[3], fstream::in};
    if (results.is_open()) {
        while (!results.eof()) {
            string line{};
            getline(results, line);
            stringstream ss{};
            ss << line;
            string iname;
            getline(ss, iname, ' ');

            string command = "./aco " + string(argv[1]) +" " + line + " < " + argv[2] + "/" + iname + " >> results";
            cout << command << endl;
            system(command.c_str());
        }
        results.close();
    }
    return 0;
}