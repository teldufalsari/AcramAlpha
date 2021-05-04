#include "common.hpp"
#include "parser.hpp"
#include "texio.hpp"

int process_file(std::ifstream& input_fs, std::uintmax_t input_size, tex_sentry& output_pipe)
{
    std::string input(input_size, '\0');
    getline(input_fs, input);
    expr_parser parser(input);
    expr_tree function = parser.read();
    if (parser.status() != OK) {
        std::cout << "Acram: " << parser.strerror() << std::endl;
        return parser.status();
    }
    else {
        auto deriv = function.derivative();
        deriv.simplify();
        output_pipe.transmit("\\begin{dmath*}\n f(x)=" + function.toTex() + "\\end{dmath*}\n");
        output_pipe.transmit("\\begin{dmath*}\n f'(x)=" + deriv.toTex() + "\\end{dmath*}\n");
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
        std::cout << "Acram: no real files were provided, leaving" << std::endl;
        return ERR_NO_FILE;
    }

    tex_sentry tex(argv[argc - 1]);
    if (tex.getState()) {
        std::cout << "Acram: couln't run LaTeX executable" << std::endl;
        return tex.getState();
    }
    tex.transmit("\\documentclass{article}\n\\usepackage[russian]{babel}\n\\usepackage{amsmath}\n\\usepackage{breqn}\n\\begin{document}");
    for (std::size_t C = 0; C < pathv.size(); C++) { // isn't it beautiful?
        std::ifstream input_fs(pathv[C]);
        if (!input_fs.is_open()) {
            std::cout << "Acram: can't open file " << pathv[C] << std::endl;
            continue;
        }
        std::cout << "Acram: processing file " << pathv[C] << std::endl;
        if (process_file(input_fs, fs::file_size(pathv[C]), tex))
            std::cout << "Couldn't create pdf for file " << pathv[C] << std::endl;
    }
    tex.transmit("\\end{document}");
    tex.end();
    if (tex.getState()) {
        std::cout << "TeX error" << std::endl;
        return tex.getState();
    }
    return 0;
}
