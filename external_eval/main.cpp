#include <iostream>
#include <clingo.hh>
#include <chrono>
#include <fstream>

std::string createFacts(int number) {
    std::string ret;
    for(int i  = 0; i < number; i++) {
        ret.append("test" + std::to_string(i) + ".\n");
    }
    return ret;
}

std::string createExternals(int number) {
    std::string ret;
    for(int i  = 0; i < number; i++) {
        ret.append("#external test" + std::to_string(number) + ".\n");
    }
    return ret;
}

int main() {
    Clingo::Logger logger = [](Clingo::WarningCode warningCode, char const* message) {
        switch (warningCode) {
            case Clingo::WarningCode::AtomUndefined:
            case Clingo::WarningCode::FileIncluded:
            case Clingo::WarningCode::GlobalVariable:
            case Clingo::WarningCode::VariableUnbounded:
            case Clingo::WarningCode::Other:
            case Clingo::WarningCode::OperationUndefined:
            case Clingo::WarningCode::RuntimeError:
                break;
            default:
                std::cerr << message << std::endl;
        }
    };

    std::ofstream myfile;
    myfile.open ("facts.csv");
    for(int i = 1; i <= 100; i++) {
        std::cout << i * 100 << std::endl;
        myfile << std::to_string(i * 100) + ",";
        for(int j = 0; j < 1000; j++) {
            Clingo::Control clingo = Clingo::Control({}, logger, 20);
            std::string facts = createFacts(i * 100);
            auto start = std::chrono::high_resolution_clock::now();
            clingo.ground({{facts.c_str(),{}}}, nullptr);
            clingo.solve();
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = end - start;
            auto ms = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
            myfile << std::to_string(ms) + ",";
        }
        myfile << "\n";
    }
    myfile.close();

    std::ofstream myfile2;
    myfile2.open ("externals.csv");
    for(int i = 1; i <= 1000; i++) {
        std::cout << i * 100 << std::endl;
        myfile2 << std::to_string(i * 100) + ",";
        for(int j = 0; j < 1000; j++) {
            Clingo::Control clingo = Clingo::Control({}, logger, 20);
            std::string externals = createExternals(i * 100);
            auto start = std::chrono::high_resolution_clock::now();
            clingo.ground({{externals.c_str(),{}}}, nullptr);
            clingo.solve();
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = end - start;
            auto ms = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
            myfile2 << std::to_string(ms) + ",";
        }
        myfile2 << "\n";
    }
    myfile2.close();
    return 0;
}