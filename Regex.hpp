#pragma once
#include <string>
#include <exception>
#include <vector>
namespace ft
{
    struct State{
        int id;
        int c;
        State *out1;
        State *out2;
    };

    class Regex
    {
    public:
        Regex(const std::string &pattern);
        ~Regex();

        static unsigned int max_nfa_stack_size;


    private:
        std::string pattern;

    // Exception classes
    public:
        class MaxNfaStackSizeReachedException : public std::exception
        {
        public:
            MaxNfaStackSizeReachedException();
            virtual const char *what() const throw();
        };
    };
}