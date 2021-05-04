#include "texio.hpp"

tex_sentry::tex_sentry() : tex_pid(0), tex_fo(nullptr), state(0)
{
    int pipefd[2] = {};
    if (pipe(pipefd) < 0) {
        state = errno;
        return;
    }
    tex_pid = fork();
    if (tex_pid < 0) { // Fork was not sucessful
        state = errno;
        return;
    } else if (tex_pid == 0) { // Child section
        close(pipefd[1]);
        int musorka = open("/dev/null", O_WRONLY);
        int tex_outputfd = dup2(musorka, STDOUT_FILENO);
        if (tex_outputfd < 0) {
            close(musorka);
            exit(1);
        }
        close(musorka);
        int tex_inputfd = dup2(pipefd[0], STDIN_FILENO);
        if (tex_inputfd < 0) {
            close(pipefd[0]);
            exit(1);
        }
        close(pipefd[0]);
        execlp("pdflatex", "pdflatex", "-jobname", "acram_out", nullptr);
        exit(1);
    } // End of child section
    close(pipefd[0]);
    tex_fo = fdopen(pipefd[1], "w");
    if (tex_fo == nullptr)
        state = errno;
}

tex_sentry::tex_sentry(const std::string& out_file_name) : tex_pid(0), tex_fo(nullptr), state(0)
{
    int pipefd[2] = {};
    if (pipe(pipefd) < 0) {
        state = errno;
        return;
    }
    tex_pid = fork();
    if (tex_pid < 0) {
        state = errno;
        return;
    } else if (tex_pid == 0) {
        close(pipefd[1]);
        int musorka = open("/dev/null", O_WRONLY);
        int tex_outputfd = dup2(musorka, STDOUT_FILENO);
        if (tex_outputfd < 0) {
            close(musorka);
            exit(1);
        }
        close(musorka);
        int tex_inputfd = dup2(pipefd[0], STDIN_FILENO);
        if (tex_inputfd < 0) {
            close(pipefd[0]);
            exit(1);
        }
        close(pipefd[0]);
        execlp("pdflatex", "pdflatex", "-jobname", out_file_name.c_str(), nullptr);
        exit(1);
    }
    close(pipefd[0]);
    tex_fo = fdopen(pipefd[1], "w");
    if (tex_fo == nullptr)
        state = errno;
}

tex_sentry::~tex_sentry()
{
    if (tex_fo != nullptr)
        fclose(tex_fo);
}

void tex_sentry::transmit(const std::string& text)
{
    clearerr(tex_fo);
    std::size_t written = fwrite(text.data(), sizeof(char), text.size(), tex_fo);
    if (written < text.size())
        state = ferror(tex_fo);
}

void tex_sentry::end()
{
    int success = fclose(tex_fo);
    if (success < 0)
        state = errno;
    tex_fo = nullptr;
    int tex_status = 0;
    waitpid(tex_pid, &tex_status, 0);
    if (!WIFEXITED(tex_status) || WEXITSTATUS(tex_status))
        state = WEXITSTATUS(tex_status);
}

int tex_sentry::getState()
{
    return state;
}
