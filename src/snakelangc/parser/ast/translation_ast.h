#ifndef SNAKELANG_TRANSLATION_AST_H
#define SNAKELANG_TRANSLATION_AST_H

#include <utility>
#include <vector>
#include "ast_node.h"
#include "package_stmt.h"
#include "top_stmt.h"

namespace parser::ast {

    class translation_ast : public ast_node {
    public:
        translation_ast(std::unique_ptr<package_stmt> package_stmt, std::vector<std::unique_ptr<top_stmt>> stmts) :
                package(std::move(package_stmt)), stmts(std::move(stmts)) {}

        std::unique_ptr<package_stmt> package;
        std::vector<std::unique_ptr<top_stmt>> stmts;
    };

}

#endif //SNAKELANG_TRANSLATION_AST_H
