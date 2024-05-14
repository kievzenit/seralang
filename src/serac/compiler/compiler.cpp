#include "compiler.h"

void compiler::compiler::compile() {
    for (const auto& entry : std::filesystem::recursive_directory_iterator(
            options_.current_directory,
            std::filesystem::directory_options::follow_directory_symlink)) {
        if (!entry.is_regular_file() && !entry.is_symlink()) {
            continue;
        }

        if (!entry.path().has_extension() || entry.path().extension() != ".sr") {
            continue;
        }

        files_.push_back(entry.path());
    }

    for (const auto& file_name : files_) {
        std::ifstream file(file_name);
        if (!file.is_open()) {
            auto error_message = std::format(
                    "Unable to open file: {}, exiting with error.\n",
                    file_name);
            utils::log_error(error_message);
        }

        lexer::lexer lexer(file.rdbuf());
        parser::parser parser(lexer);

        translation_asts_.push_back(parser.parse());
    }

    for (auto& package : split_asts_by_package()) {
        emitter::emitter emitter(std::move(package.second));
        auto package_ir = emitter.emit();

        translator::translator translator(std::move(package_ir));
        auto module = translator.translate();

        if (options_.run_in_jit) {
            run_module_in_jit(std::move(module));
            continue;
        }

        compile_module(std::move(module));
    }
}

std::map<std::string, std::vector<std::unique_ptr<parser::ast::translation_ast>>>
compiler::compiler::split_asts_by_package() {
    std::map<std::string, std::vector<std::unique_ptr<parser::ast::translation_ast>>> packages;

    for (auto& ast : translation_asts_) {
        packages[ast->package->package_name].push_back(std::move(ast));
    }

    return packages;
}

void compiler::compiler::compile_module(std::unique_ptr<llvm::Module> module) {
    using namespace llvm;

    if (options_.output_type == emit_type::none) {
        return;
    }

    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();

    auto target_triple = sys::getDefaultTargetTriple();
    module->setTargetTriple(target_triple);

    std::string error;
    const auto target = TargetRegistry::lookupTarget(target_triple, error);

    if (!target) {
        utils::log_error(error);
        __builtin_unreachable();
    }

    const auto cpu = "generic";
    const auto features = "";
    auto options = TargetOptions();

    auto target_machine = target->createTargetMachine(
            target_triple,
            cpu,
            features,
            options,
            Reloc::PIC_);

    auto output_file_name = module->getModuleIdentifier() + generate_extension();
    std::error_code error_code;
    raw_fd_ostream dest(output_file_name, error_code, sys::fs::OF_None);

    if (error_code) {
        utils::log_error("Cannot open file: " + output_file_name);
    }

    if (options_.output_type == emit_type::llvm_ir) {
        module->print(dest, nullptr);
        return;
    }

    LoopAnalysisManager LAM;
    FunctionAnalysisManager FAM;
    CGSCCAnalysisManager CGAM;
    ModuleAnalysisManager MAM;

    PassBuilder PB;

    PB.registerModuleAnalyses(MAM);
    PB.registerCGSCCAnalyses(CGAM);
    PB.registerFunctionAnalyses(FAM);
    PB.registerLoopAnalyses(LAM);
    PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

    ModulePassManager MPM = PB.buildPerModuleDefaultPipeline(OptimizationLevel::O0);
    MPM.addPass(createModuleToFunctionPassAdaptor(JumpThreadingPass()));

    MPM.run(*module, MAM);

    legacy::PassManager pass;
    auto file_type = options_.output_type == emit_type::object_file ?
            CodeGenFileType::ObjectFile : CodeGenFileType::AssemblyFile;

    if (target_machine->addPassesToEmitFile(pass, dest, nullptr, file_type)) {
        utils::log_error("Target machine cannot emit file of this type.");
    }

    pass.run(*module);
    dest.flush();
}

void compiler::compiler::run_module_in_jit(std::unique_ptr<llvm::Module> module) {
    auto main_func = module->getFunction("main");

    llvm::EngineBuilder engine_builder(std::move(module));
    llvm::ExecutionEngine* execution_engine = engine_builder.create();

    auto result = execution_engine->runFunctionAsMain(main_func, {}, {});

    std::cout << "Program returned code: " << result << std::endl;
}

std::string compiler::compiler::generate_extension() const {
    switch (options_.output_type) {
        case emit_type::object_file:
            return ".o";
        case emit_type::llvm_ir:
            return ".ll";
        case emit_type::assembler:
            return ".s";
        default:
            __builtin_unreachable();
    }
}
