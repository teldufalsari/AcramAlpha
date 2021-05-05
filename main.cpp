#include "common.hpp"
#include "parser.hpp"
#include "texio.hpp"
#include <stdexcept>

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

int ProcessFunction(const std::string& func_str, std::string& output_ss)
{
    expr_parser parser(func_str);
    expr_tree function = parser.read();
    if (parser.status() != OK) {
        std::cout << "Acram: " << parser.strerror() << std::endl;
        return parser.status();
    }
    function.checkSemantics();
    if (function.status() != OK) {
        std::cout << "Acram: " << function.strerror() << std::endl;
        return function.status();
    }
    auto derivative = function.derivative();
    derivative.simplify();
    output_ss += "\\begin{dmath*}\n" + function.getName() + '(' + function.getVar() + ")=" + function.toTex() + "\\end{dmath*}\n";
    output_ss += "\\begin{dmath*}\n" + derivative.getName() + '(' + derivative.getVar() + ")=" + derivative.toTex() + "\\end{dmath*}\n";
    return OK;
}

void WriteTex(const std::string& code, const fs::path& output_filename)
{
    std::cout << "Acram: LaTeX executable not found" << std::endl;
    std::ofstream output_fs(output_filename);
    if (!output_fs.is_open())
        throw std::runtime_error("Acram: couldn't open file \"" + output_filename.string() + "\" to write TeX output");
    std::cout << "Acram: writing TeX output to \"" + output_filename.string() + "\"..." << std::endl;
    output_fs.write(code.c_str(), code.size());
    if (!output_fs.good())
        throw std::runtime_error("Acram: errors occured during writing. Data may be incomplete");
    std::cout << "Acram: output written successfully to \"" + output_filename.string() + ".tex\"" <<std::endl;
}

bool LatexExists()
{
    return fs::exists("/bin/pdflatex") || fs::exists("/usr/bin/pdflatex");
}

void LatexToPdf(const std::string& code, const fs::path& output_filename)
{
    std::cout << "Acram: Converting output to pdf..." << std::endl;
    tex_sentry tex(output_filename);
    if (tex.getState())
        throw std::runtime_error("Acram: couldn't run LaTeX executable");
    tex.transmit(code);
    if (tex.getState())
        throw std::runtime_error("Acram: failed to transmit data to LaTeX executable");
    tex.end();
    if (tex.getState())
        throw std::runtime_error("Acram: LaTeX exited with bad status");
    std::cout << "Acram: output written successfully to \"" + output_filename.string() + ".pdf\"" << std::endl;
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
        std::cout << "Acram: again: print Y to start or Q to exit:\n]=> ";
    }
    return proceed;
}

int ConsoleMode(const fs::path& output_filename)
{
    std::string input_buf, output(Header());
    std::cout << "Acram Alpha, symbolic differentiator by @teldufalsari" << std::endl;
    while (1) {
        std::cout << "Acram: print Y to start or Q to exit:\n]=> ";
        bool proceed = Ask();
        if (proceed == false) {
            output += "\\end{document}\n";
            try {
                if (LatexExists())
                    LatexToPdf(output, output_filename);
                else
                    WriteTex(output, output_filename);
            } catch (std::runtime_error& ex) {
                std::cout << ex.what() << std::endl;;
                return 1;
            }
            return 0;
        }
        std::cout << "Acram: write your function in the format \"f(x)=...\"\n]=> ";
        std::getline(std::cin, input_buf, '\n');
        ProcessFunction(input_buf, output);
    }
}

int FileMode(const tld::vector<fs::path>& inputs, const fs::path& output_filename)
{
    std::string input_buf, output(Header());
    std::cout << "Acram Alpha, symbolic differentiator by @teldufalsari" << std::endl;
    for (std::size_t i = 0; i < inputs.size(); i++) {
        std::ifstream input_fs(inputs[i]);
        if (!input_fs.is_open()) {
            std::cout << "Acram: can't open file " << inputs[i] << std::endl;
            continue;
        }
        std::cout << "Acram: processing file " << inputs[i] << std::endl;
        std::getline(input_fs, input_buf);
        ProcessFunction(input_buf,output);
    }
    output += "\\end{document}\n";
    try {
        if (LatexExists())
            LatexToPdf(output, output_filename);
        else
            WriteTex(output, output_filename.string() + ".tex");
    } catch (std::runtime_error& ex) {
        std::cout << ex.what() << std::endl;;
        return 1;
    }
    return 0;
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
    fs::path output_filename(argv[argc - 1]);
    return FileMode(pathv, output_filename);
}
