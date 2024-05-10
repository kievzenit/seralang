#include "test_utils.h"

int test_source(const std::string& source) {
        std::istringstream string_stream(source);
        lexer::lexer lexer(string_stream.rdbuf());
        parser::parser parser(lexer);

        auto translation_ast = parser.parse();
        std::vector<std::unique_ptr<parser::ast::translation_ast>> translation_asts;
        translation_asts.push_back(std::move(translation_ast));

        emitter::emitter emitter(std::move(translation_asts));
        auto package_ir = emitter.emit();

        translator::translator translator(std::move(package_ir));
        auto module = translator.translate();

        auto main_func = module->getFunction("main");

        llvm::EngineBuilder engine_builder(std::move(module));
        llvm::ExecutionEngine* execution_engine = engine_builder.create();

        return execution_engine->runFunctionAsMain(main_func, {}, {});
}