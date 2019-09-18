#include <chrono>
#include <clingo.hh>
#include <fstream>
#include <iostream>

std::string createFacts(int number)
{
    std::string ret;
    for (int i = 0; i < number; i++) {
        ret.append("test" + std::to_string(i) + ".\n");
    }
    return ret;
}

std::string createExternals(int number)
{
    std::string ret;
    for (int i = 0; i < number; i++) {
        ret.append("#external test" + std::to_string(number) + ".\n");
    }
    return ret;
}

int main()
{
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

    /*std::ofstream myfile;
    myfile.open("facts.csv");
    std::string facts = createFacts(100);
    for (int j = 1; j <= 10; j++) {
        myfile << std::to_string(j * 10) + ",";
        for (int i = 0; i < 1000; i++) {
            auto start = std::chrono::high_resolution_clock::now();
            for (int k = 0; k < j * 10; k++) {
                Clingo::Control clingo = Clingo::Control({}, logger, 20);
                clingo.ground({{facts.c_str(), {}}}, nullptr);
                clingo.solve();
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = end - start;
            auto ms = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
            myfile << std::to_string(ms) + ",";
        }
        myfile << std::endl;
    }
    myfile.close();*/

    std::ofstream myfile;
    myfile.open("externals.csv");
    std::string externals = createExternals(100);
    for (int j = 1; j <= 10; j++) {
        myfile << std::to_string(j * 10) + ",";
        for (int i = 0; i < 1000; i++) {
            Clingo::Control clingo = Clingo::Control({}, logger, 20);
            clingo.ground({{externals.c_str(), {}}}, nullptr);
            clingo.solve();
            auto start = std::chrono::high_resolution_clock::now();
            for (int k = 0; k < j * 10; k++) {
                std::string ext = "test" + std::to_string(i);
                clingo.assign_external(Clingo::parse_term(ext.c_str()), Clingo::TruthValue::True),
                clingo.solve();
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = end - start;
            auto ms = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
            myfile << std::to_string(ms) + ",";
        }
        myfile << std::endl;
    }
    myfile.close();

    /*std::ofstream myfile2;
    myfile2.open("externals.csv");
    std::string externals = createExternals(100);
    for (int i = 1; i <= 1000; i++) {
        std::cout << i * 100 << std::endl;
        myfile2 << std::to_string(i * 100) + ",";
        for (int j = 0; j < 1000; j++) {
            Clingo::Control clingo = Clingo::Control({}, logger, 20);
            auto start = std::chrono::high_resolution_clock::now();
            clingo.ground({{externals.c_str(), {}}}, nullptr);
            clingo.solve();
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = end - start;
            auto ms = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
            myfile2 << std::to_string(ms) + ",";
        }
        myfile2 << "\n";
    }
    myfile2.close();*/
    return 0;
}