//
// Created by nvr on 21/7/19.
//

#include <cxxopts.hpp>
#include <string>
#include <vector>
#include <fstream>
#include "relation_parser.h"

using namespace std;

int main(int c, char *argv[]) {
    cxxopts::Options options("extractor", "Preprocessor to extract lexical nodes for sentence piece");
    options.add_options()
            ("ttl_list", "ttl files to process", cxxopts::value<vector<string>>())
            ("out_file", "location to write results", cxxopts::value<string>());
    auto results = options.parse(c, argv);

    relation_parser rp;
    ofstream ofile;
    ofile.open(results["out_file"].as<string>());
    if (!ofile.is_open()) {
        cout << "Could not open file for writing.";
        return 1;
    }
    auto lambda = [&ofile](vector<string_view> finds) -> void {
        if (finds.size() != 3) {
            return;
        }
        for (auto f:finds) {
            if (f.find("http") == 0)
                continue;
            ofile << f << endl;

        }
    };
    for (const auto& filename:results["ttl_list"].as<vector<string>>()) {
        rp.parse(filename, lambda);
    }
    ofile.close();
}
