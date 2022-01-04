#pragma once
#include <string>
#include <exception>
#include <vector>
#include <RegexUtils.hpp>
#include <iostream>
namespace ft
{
    class Regex
    {
        Regex();
    public:
        Regex(const std::string &pattern);
        ~Regex();

        void check_concatenation(int, std::vector<char> &);

        static unsigned int max_nfa_stack_size;

        std::string pattern;
    private:
      
        void execute_higher_priority_operator(std::vector<char> &, int,
            std::vector<Frag> &, unsigned int &, size_t);
        void dispatch_operator(int operator_type,
            std::vector<Frag> &, unsigned int &, size_t);
        int get_opp_code(char);

        void alternation(std::vector<Frag> &, unsigned int &, size_t);
        void concatenation(std::vector<Frag> &, unsigned int &, size_t);
        void repetition_star(std::vector<Frag> &, unsigned int &, size_t);

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