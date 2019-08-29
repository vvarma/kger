//
// Created by nvr on 30/7/19.
//

#ifndef KGER_OWLPARSER_H
#define KGER_OWLPARSER_H

#include <libxml/xmlreader.h>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "graph/unique_id.h"
#include "graph/node.h"


struct OntologyBuilder {
    std::map<has_unique_id::uid, std::shared_ptr<NamedEntityNode>> nodes;
    std::map<std::string_view, has_unique_id::uid> node_cache;

    void processChildSecondPass(xmlDocPtr doc, xmlNodePtr cur, std::shared_ptr<NamedEntityNode> current_node = {});

    void processChildFirstPass(xmlDocPtr doc, xmlNodePtr cur);

    void processTree(const std::string &filename);

};

#endif //KGER_OWLPARSER_H
