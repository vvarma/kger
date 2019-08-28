//
// Created by nvr on 21/8/19.
//

#ifndef KGER_GRAPH_IO_H
#define KGER_GRAPH_IO_H

#include <string>
#include <memory>
#include "graph/node.h"

struct GraphSaveOptions {
    std::string output_path;
};

void save(const std::shared_ptr<Graph> &graph, const GraphSaveOptions &options);

std::shared_ptr<Graph> load(const GraphSaveOptions &options);

#endif //KGER_GRAPH_IO_H
