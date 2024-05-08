#ifndef SERALANG_PACKAGE_STMT_H
#define SERALANG_PACKAGE_STMT_H

#include <string>
#include "../ast_node.h"

namespace parser::ast {

    class package_stmt : public ast_node {
    public:
        package_stmt(std::string package_name) : package_name(std::move(package_name)) {}

        std::string package_name;
    };

}

#endif //SERALANG_PACKAGE_STMT_H
