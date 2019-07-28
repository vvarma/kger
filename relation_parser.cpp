//
// Created by nvr on 18/7/19.
//

#include <fstream>
#include <vector>
#include <iostream>
#include "relation_parser.h"

vector<string_view> parse_line(string_view line) {
    vector<string_view> finds;
    if (line.empty())
        return finds;
    if (line.at(0) == '#')
        return finds;

    while (!line.empty()) {
        char end_char = 0;
        if (line.at(0) == '"') {
            end_char = '"';
        } else if (line.at(0) == '<') {
            end_char = '>';
        } else {
            line.remove_prefix(1);
            continue;
        }
        line.remove_prefix(1);
        size_t end_at = line.find_first_of(end_char);
        if (end_at == std::string_view::npos) {
            break;
        }
        finds.push_back(line.substr(0, end_at));
        line.remove_prefix(end_at);
    }
    return finds;
}

void relation_parser::parse(const string &filename,
                            const function<void(vector<string_view>)> &collector) {
    std::ifstream ifile;
    ifile.open(filename);
    std::string line;
    int doc_count = 0;
    while (std::getline(ifile, line)) {
        collector(parse_line(line));
        if (++doc_count % 10000 == 0) {
            cout << "processing. done:" << doc_count << endl;
        }
    }
}
