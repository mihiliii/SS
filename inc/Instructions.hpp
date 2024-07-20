#ifndef Instructions_hpp_
#define Instructions_hpp_

#include <unordered_map>
#include <functional>

class Instructions {
public:

    friend class Assembler;

    static void halt();

private:

    static std::unordered_map<std::string, std::function<void()>> instruction_map;
    
};

#endif