/*
================================================================================
  Copyright (c) 2023, Pandemos
  All rights reserved.
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  * Neither the name of the organization nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
================================================================================
*/

#pragma once

#include <thread>
#include <mutex>
#include <list>
#include <string>
#include <cstdio>
#include <sstream>
#if defined(_WIN32) || defined(_WIN64)
#include <io.h>
#include <stdio.h>
#else
#include <unistd.h>
#endif

namespace platform {

    using utilities::string;

    class testing : public properties {
    public:
        virtual void init(int argc, char **argv) = 0;
        virtual int run() = 0;

        // Launches a thread that captures stdout and calls run()
        void runWithOutputCapture() {
            std::thread([this]() {
                fflush(stdout);
                int pipefd[2];
#if defined(_WIN32) || defined(_WIN64)
                if (_pipe(pipefd, 4096, _O_TEXT) == -1) return;
                int stdout_fd = _dup(_fileno(stdout));
                _dup2(pipefd[1], _fileno(stdout));
                _close(pipefd[1]);
                FILE* read_fp = _fdopen(pipefd[0], "r");
#else
                if (pipe(pipefd) == -1) return;
                int stdout_fd = dup(fileno(stdout));
                dup2(pipefd[1], fileno(stdout));
                close(pipefd[1]);
                FILE* read_fp = fdopen(pipefd[0], "r");
#endif
                std::thread runThread([this]() { this->run(); });
                char buffer[256];
                std::string line;
                while (fgets(buffer, sizeof(buffer), read_fp)) {
                    line = buffer;
                    std::lock_guard<std::mutex> lock(outputMutex);
                    outputLines.push_back(line);
                }
                fclose(read_fp);
                fflush(stdout);
#if defined(_WIN32) || defined(_WIN64)
                _dup2(stdout_fd, _fileno(stdout));
                _close(stdout_fd);
#else
                dup2(stdout_fd, fileno(stdout));
                close(stdout_fd);
#endif
                runThread.join();
            }).detach();
        }

        // Pops a line from the output list, thread-safe
        bool popOutputLine(std::string& out) {
            std::lock_guard<std::mutex> lock(outputMutex);
            if (!outputLines.empty()) {
                out = std::move(outputLines.front());
                outputLines.pop_front();
                return true;
            }
            return false;
        }

        // Getter for test data path
        virtual std::string getTestDataPath() const {
            return testDataPath;
        }
        // Setter for test data path
        virtual void setTestDataPath(const std::string& path) {
            testDataPath = path;
        }

    protected:
        // System path to test data
        std::string testDataPath;
        // Captured output lines
        std::list<std::string> outputLines;
        // Mutex for protecting outputLines
        std::mutex outputMutex;
    };

}
