#pragma once

#include <exception>
#include <string>

class MsgException : public std::exception {
public:

    MsgException(const std::string& message, const std::string& file, int line)
        : std::exception(),
          _message("Exception thrown in file " + file + " : " + std::to_string(line) + "\n\t" +
                   message + "\n")
    {
    }

    const char* what() const noexcept override { return _message.c_str(); }

private:

    std::string _message;
};

// clang-format on

#define THROW_EXCEPTION(msg) throw MsgException(msg, __FILE__, __LINE__)