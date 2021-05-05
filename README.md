# Acram Alpha
Symbolic differentiator for GNU/Linux.

## What is it?
This is a symbolic differentiator: software that takes
mathematical function in common notation and calculates its derivative.
If `pdflatex` is installed, the output will be converted to pdf.
If not, LaTeX source file will be created instead.

## Usage
Acram Alpha can operate in two input modes: console and file
### Console mode:
Print `acram` to enter console input mode. Follow the instructions:
if there are any syntax or semantic error, they will be reported and
input will be discarded. On exit, all functions entered will be outputted
to `pdflatex` or saved as LaTeX source. The file name will be either
"Acram_out.pdf" or "Acram_out.tex

Print `acram [filename]` to specify output file name instead
of "Acram_out.*"

### File mode:
Print `acram  file_1 [file_2 ...] output_file` to run Acram Alpha
in file input mode. Each file should contain mathematical function
on a single line. If there are any errors, they will be reported and
file with errors will be discarded. Output is saved to "output_file.pdf"
of "output_file.tex" respectively.

## Features
### Supported functions:
 * arithmetic operators
 * trigonometric and inverse trigonometric functions
 * power function
 * natural logarithm and exponential function
 * square root as a distinct from power func`tion

### Bugs and issues:
 * functions like f(x)^(g(x)) are not supported though treated as if g(x) is constant
 * calculatons with decimal fraction are not supported. You'd better not use them at all
 * if `pdflatex` is installed but for some reason would not start, the program silently fails
