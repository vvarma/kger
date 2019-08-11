//
// Created by nvr on 18/7/19.
//

#ifndef KGER_RELATION_PARSER_H
#define KGER_RELATION_PARSER_H


#include <string>
#include <functional>
#include <vector>


class relation_parser {
public:
    void parse(const std::string &filename, const std::function<void(std::vector<std::string_view>)>& collector);

};


#endif //KGER_RELATION_PARSER_H
