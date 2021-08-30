/*!
 * @file demo.hpp
 * @author  (Aleksander Rybin)
 * @brief Demo program for heterogeneous tree library
 * @date 2021-08-29
 */

// ---------------------
// includes: STL
#include <filesystem>
#include <fstream>
#include <string>

// ---------------------
// includes: boost
#include <boost/program_options.hpp>

// ---------------------
// includes: local
#include "tree_lib/tree.hpp"

int main(int argc, char* argv[]) {
    using TreeType = tree_lib::Tree<int, double, std::string>;

    namespace po = boost::program_options;

    std::filesystem::path inputPath;
    std::filesystem::path outputPath;

    po::options_description options("Allowed programm options");

    // clang-format off
    options.add_options()
        ("help,h", "produce help message")
        ("input,i", po::value<std::filesystem::path>(&inputPath)->required(), "path to input file with tree")
        ("output,o", po::value<std::filesystem::path>(&outputPath)->required(), "path to output file to serialize tree");
    // clang-format on

    po::variables_map vm;

    try {
        po::store(po::parse_command_line(argc, argv, options), vm);
        po::notify(vm);
    } catch (const po::error& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    if (vm.count("help")) {
        std::cout << options << std::endl;
        return EXIT_SUCCESS;
    }

    if (!std::filesystem::exists(inputPath) || std::filesystem::is_directory(inputPath)) {
        std::cout << "no such file " << inputPath << std::endl;
        return EXIT_FAILURE;
    }

    if (std::filesystem::is_directory(outputPath)) {
        std::cout << "no such file " << outputPath << std::endl;
        return EXIT_FAILURE;
    }

    TreeType tree;

    std::ifstream input(inputPath);
    std::ofstream output(outputPath);

    tree.Load(input);
    tree.Print(std::cout);
    tree.Dump(output);
    output << std::endl << std::endl;
    output << tree.Print();

    return EXIT_SUCCESS;
}
