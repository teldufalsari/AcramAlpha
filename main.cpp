#include "common.hpp"
#include "parser.hpp"
#include "texio.hpp"

std::string Header()
{
    return
    "\\documentclass{article}\n"
    "\\usepackage[russian]{babel}\n"
    "\\usepackage{amsmath}\n"
    "\\usepackage{breqn}\n"
    "\\DeclareMathOperator{\\arccot}{arccot}\n"
    "\\begin{document}\n"
    "\\begin{center}\n"
    "{\\Large " + Splash() + "}\n"
    "\\end{center}\n";
}

int ProcessFile(std::ifstream& input_fs, std::uintmax_t input_size, tex_sentry& output_pipe)
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
        function.checkSemantics();
        if (function.status() != T_OK) {
            std::cout << "Acram: " << function.strerror() << std::endl;
            return function.status();
        }
        auto deriv = function.derivative();
        deriv.simplify();
        output_pipe.transmit("\\begin{dmath*}\n" + function.getName() + '(' + function.getVar() + ")=" + function.toTex() + "\\end{dmath*}\n");
        output_pipe.transmit("\\begin{dmath*}\n" + deriv.getName() + '(' + deriv.getVar() + ")=" + deriv.toTex() + "\\end{dmath*}\n");
        return OK;
    }
}

tex_sentry TexInit(const fs::path& output_filename)
{
    tex_sentry tex(output_filename);
    if (tex.getState())
        std::cout << "Acram: couln't run LaTeX executable" << std::endl;
    else
        tex.transmit(Header());
    return tex;
}

bool Ask()
{
    bool proceed = true;
    char key = '\0';
    while (1) {
        std::cin.get(key);
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (key == 'y' || key == 'Y') {
            proceed = true;
            break;
        }
        if (key == 'q' || key == 'Q') {
            proceed = false;
            break;
        }
        std::cout << "Acram: again: print Y to start or Q to exit:";
    }
    return proceed;
}

int ConsoleMode(const fs::path& output_filename)
{
    tex_sentry tex = TexInit(output_filename);
    std::string input;
    if (tex.getState()) {
        std::cout << "Acram: failed to transmit data to LaTeX executable" << std::endl;
        return tex.getState();
    }
    while (1) {
        std::cout << "Acram Alpha, v2.721\n" "Acram: print Y to start or Q to exit:";
        bool proceed = Ask();
        if (proceed == false) {
            tex.transmit("\\end{document}\n");
            tex.end();
            return 0;
        }
        std::cout << "Acram: write your function in the format \"f(x)=...\"\n";
        std::getline(std::cin, input, '\n');
        expr_parser parser(input);
        expr_tree function = parser.read();
        if (parser.status() != OK) {
            std::cout << "Acram: " << parser.strerror() << std::endl;
            continue;
        }
        else {
            function.checkSemantics();
            if (function.status() != T_OK) {
                std::cout << "Acram: " << function.strerror() << std::endl;
                continue;
            }
        }
        auto deriv = function.derivative();
        deriv.simplify();
        tex.transmit("\\begin{dmath*}\n" + function.getName() + '(' + function.getVar() + ")=" + function.toTex() + "\\end{dmath*}\n");
        tex.transmit("\\begin{dmath*}\n" + deriv.getName() + '(' + deriv.getVar() + ")=" + deriv.toTex() + "\\end{dmath*}\n");
    }
}

int main(int argc, char* argv[])
{
    if (argc == 1) {
        return ConsoleMode("Acram_out");
    } else if (argc == 2) {
        return ConsoleMode(argv[1]);
    }
    tld::vector<fs::path> pathv = FillPathv(argc - 2, argv + 1);
    if (pathv.size() == 0) {
        std::cout << "Acram: no real files were provided, leaving" << std::endl;
        return ERR_NO_FILE;
    }

    tex_sentry tex = TexInit(argv[argc - 1]);
    if (tex.getState()) {
        std::cout << "Acram: failed to transmit data to LaTeX executable" << std::endl;
        return tex.getState();
    }
    tex.transmit(Header());
    for (std::size_t C = 0; C < pathv.size(); C++) { // isn't it beautiful?
        std::ifstream input_fs(pathv[C]);
        if (!input_fs.is_open()) {
            std::cout << "Acram: can't open file " << pathv[C] << std::endl;
            continue;
        }
        std::cout << "Acram: processing file " << pathv[C] << std::endl;
        if (ProcessFile(input_fs, fs::file_size(pathv[C]), tex))
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
