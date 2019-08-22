//
// Created by nvr on 21/8/19.
//

#ifndef KGER_GRAPH_IO_H
#define KGER_GRAPH_IO_H

#include <string>
#include <bits/shared_ptr.h>
#include "cpp/node.h"

struct GraphSaveOptions {
    std::string output_path;
};

void save(std::shared_ptr<Graph> graph, const GraphSaveOptions &options);

#endif //KGER_GRAPH_IO_H
