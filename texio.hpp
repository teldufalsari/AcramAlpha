#ifndef ACRAM_TEXIO_HPP
#define ACRAM_TEXIO_HPP

#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <cstdio>
#include <string>
#include <iostream>
/** 
 * @file texio.hpp
 * @brief pipe i/o handling class
 */

/// Sentry object for redirecting LaTeX commands directly to pdflatex
class tex_sentry
{
    // pdflatex process id
    pid_t tex_pid;
    // file stream associated with pdflatex standard input
    FILE* tex_fo;
    // state of object
    int state;

public:
    tex_sentry() = delete;

    /**
     * @brief Runs pdflatex with specified output file name
     * @param out_file_name file name without an extension
     */
    tex_sentry(const std::string& out_file_name);

    tex_sentry(const tex_sentry& that) = delete;
    tex_sentry(tex_sentry&& that) = delete;
    tex_sentry& operator =(const tex_sentry& that) = delete;
    tex_sentry& operator =(tex_sentry&& that) = delete;

    /// Closes stream associated with pipe if hasn't been closed yet
    ~tex_sentry();

    /**
     * @brief Write commands to pdflatex standard input
     * @param text string that contains LaTeX commands
     * @details If state is not OK, transmitting does nothing
     */
    void transmit(const std::string& text);
    
    /// End pdflatex session and try to save pdf
    void end();

    /// Obtain sentry object state
    int getState();
};

#endif // ACRAM_TEXIO_HPP
