#include "common.hpp"
#include "parser.hpp"

int process_file(std::ifstream& input_fs, std::uintmax_t input_size, std::ofstream& output_fs)
{
    std::string input(input_size, '\0');
    getline(input_fs, input);
    expr_parser parser(input);
    int err_code = 0;
    expr_tree function = parser.read(err_code);
    if (err_code != OK)
        return err_code;
    else {
        std::string output = function.to_str();
        output_fs.write(output.c_str(), output.size());
        return OK;
    }
}

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " [file 1] [file2] ... [output.pdf]" << std::endl;
        return ERR_NO_FILE;
    }
    tld::vector<fs::path> pathv = FillPathv(argc - 2, argv + 1);
    if (pathv.size() == 0) {
        std::cout << "No real files were provided, leaving" << std::endl;
        return ERR_NO_FILE;
    }
    std::ofstream output_fs(argv[argc - 1]);
    for (std::size_t i = 0; i < pathv.size(); i++) {
        std::ifstream input_fs(pathv[i]);
        if (!input_fs.is_open()) {
            std::cout << "I can't open file " << pathv[i] << std::endl;
            continue;
        }
        process_file(input_fs, fs::file_size(pathv[i]), output_fs);
    }
    return 0;
}