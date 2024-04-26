#ifndef SNAKELANG_COMPILER_H
#define SNAKELANG_COMPILER_H

#include <utility>
#include <filesystem>
#include <thread>

#include <llvm/Support/TargetSelect.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/IR/LegacyPassManager.h>

#include "../parser/parser.h"
#include "../emitter/emitter.h"
#include "../translator/translator.h"

#include "compile_options.h"

namespace compiler {

    class compiler {
    public:
        explicit compiler(compile_options options) : options_(std::move(options)) {}
        compiler() {
            options_ = compile_options(std::filesystem::current_path(), emit_type::object_file);
        }

        void compile();

        std::map<std::string, std::vector<std::unique_ptr<parser::ast::translation_ast>>> split_asts_by_package();
    private:
        compile_options options_;

        std::vector<std::string> files_;
        std::vector<std::unique_ptr<parser::ast::translation_ast>> translation_asts_;

        void compile_module(std::unique_ptr<llvm::Module> module);
        std::string generate_extension() const;
    };

}

#endif //SNAKELANG_COMPILER_H
