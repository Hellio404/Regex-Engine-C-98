#include <Regex.hpp>
namespace ft
{

    unsigned int Regex::max_nfa_stack_size = 1000;

    Regex::Regex(const std::string &pattern) : pattern(pattern)
    {
#define PUSH(state) nfa_stack[stack_index++] = (state)
#define POP() nfa_stack[--stack_index];

        void (Regex::*pending_opperator[256])(std::vector<State *> &, int);
        int last_operator = -1;
        unsigned int stack_index = 0;
        const size_t pattern_size = pattern.size();
        std::vector<State *> nfa_stack;

        nfa_stack.reserve(max_nfa_stack_size);
        for (size_t i = 0; i < pattern_size; i++)
        {
            // doing logic for each character in the pattern
        }

#undef PUSH
#undef POP
    }

    Regex::MaxNfaStackSizeReachedException::MaxNfaStackSizeReachedException()
        : std::exception() {}

    const char *Regex::MaxNfaStackSizeReachedException::what() const throw()
    {
        return ("Max NFA stack size reached Please increase the value of"
                " ft::Regex::max_nfa_stack_size or reduce the pattern"
                " size/complexity");
    }

}