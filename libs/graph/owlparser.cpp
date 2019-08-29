//
// Created by nvr on 28/7/19.
//

#include "owlparser.h"


void OntologyBuilder::processChildSecondPass(xmlDocPtr doc, xmlNodePtr cur, std::shared_ptr<NamedEntityNode> current_node) {
    if (xmlStrEqual(cur->name, (const xmlChar *) "Class") == 1) {
        std::string resource;
        auto prop = cur->properties;
        while (prop != nullptr) {
            if (xmlStrEqual(prop->name, (const xmlChar *) "about") == 1) {
                if (prop->children != nullptr) {
                    resource = std::string(reinterpret_cast<const char *>(prop->children->content));
                }
                break;
            }
            prop = prop->next;
        }
        if (!resource.empty()) {
            auto it = node_cache.find(resource);
            if (it != node_cache.end()) {
                current_node = nodes[it->second];
            }
        }
    } else if (current_node && xmlStrEqual(cur->name, (const xmlChar *) "label") == 1) {
        auto prop = cur->properties;
        bool isEnglish = false;
        while (prop != nullptr) {
            if (xmlStrEqual(prop->name, (const xmlChar *) "lang") == 1) {
                if (prop->children != nullptr) {
                    isEnglish = xmlStrEqual(prop->children->content, (const xmlChar *) "en") == 1;
                    if (isEnglish)
                        break;
                }
            }
            prop = prop->next;
        }
        if (isEnglish) {
            auto child = cur->children;
            while (child != nullptr) {
                if (child->content != nullptr) {
                    current_node->label = std::string(reinterpret_cast<const char *>(child->content));
                    break;
                }
                child = child->next;
            }
        }
    } else if (current_node && xmlStrEqual(cur->name, (const xmlChar *) "subClassOf") == 1) {
        std::string resource;
        auto prop = cur->properties;
        while (prop != nullptr) {
            if (xmlStrEqual(prop->name, (const xmlChar *) "resource") == 1) {
                if (prop->children != nullptr) {
                    resource = std::string(reinterpret_cast<const char *>(prop->children->content));
                }
                break;
            }
            prop = prop->next;
        }
        if (!resource.empty()) {
            auto parent = node_cache.find(resource);
            if (parent != node_cache.end()) {
                current_node->parents.push_back(parent->second);
            }
        }
    }
    auto child = cur->children;
    while (child != nullptr) {
        processChildSecondPass(doc, child, current_node);
        child = child->next;
    }

}

void OntologyBuilder::processChildFirstPass(xmlDocPtr doc, xmlNodePtr cur) {
    if (xmlStrEqual(cur->name, (const xmlChar *) "Class") == 1) {
        std::shared_ptr<NamedEntityNode> current_node;
        auto prop = cur->properties;
        while (prop != nullptr) {
            if (xmlStrEqual(prop->name, (const xmlChar *) "about") == 1) {
                if (prop->children != nullptr) {
                    current_node = std::make_shared<NamedEntityNode>(
                            std::string(reinterpret_cast<const char *>(prop->children->content)));
                }
                break;
            }
            prop = prop->next;
        }
        if (current_node) {
            nodes[current_node->unique_id()] = current_node;
            node_cache[current_node->resource] = current_node->unique_id();
        }
    }
    auto child = cur->children;
    while (child != nullptr) {
        processChildFirstPass(doc, child);
        child = child->next;
    }
}


void OntologyBuilder::processTree(const std::string &filename) {
    auto doc = xmlParseFile(filename.c_str());
    if (doc == nullptr) {
        return;
    }
    auto cur = xmlDocGetRootElement(doc);
    if (cur != nullptr) {
        processChildFirstPass(doc, cur);
        processChildSecondPass(doc, cur);
    }
    xmlFree(doc);
}

