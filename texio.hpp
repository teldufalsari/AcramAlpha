#ifndef ACRAM_TEXIO_HPP
#define ACRAM_TEXIO_HPP

#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <cstdio>
#include <string>

class tex_sentry
{
    pid_t tex_pid;
    FILE* tex_fo;
    int state;
    int stage;

public:
    tex_sentry();
    tex_sentry(const std::string& out_file_name);
    ~tex_sentry();

    void transmit(const std::string& text);
    void end();
    int getState();
};

#endif // ACRAM_TEXIO_HPP
