#include <Regex.hpp>
namespace ft
{
#define PUSH(frag) nfa_stack[stack_index++] = (frag)
#define POP() nfa_stack[--stack_index];

   
    
    unsigned int Regex::max_nfa_stack_size = 1000;
   
    Regex::Regex(const std::string &pattern) : pattern(pattern)
    {

        std::vector<char> pending_opperator;

        int last_operator = -1;
        unsigned int stack_index = 0;
        const size_t pattern_size = pattern.size();
        std::vector<Frag> nfa_stack;

        nfa_stack.reserve(max_nfa_stack_size);
        for (size_t i = 0; i < pattern_size; i++)
        {
            switch (pattern[i])
            {
            default:
                check_concatenation(last_operator, pending_opperator);
                last_operator = NORMAL_OPP;
                PUSH(Frag(pattern[i]));
                break;
            case '|':
                execute_higher_priority_operator(pending_opperator,
                    ALTERNATION, nfa_stack, stack_index, i);
                last_operator = ALTERNATION;
                pending_opperator.push_back('|');
                break;
            case '*':
                last_operator = REPETITION;
                repetition_star(nfa_stack, stack_index, i);
                break;
            }

        }
        execute_higher_priority_operator(pending_opperator,
            0, nfa_stack, stack_index, pattern_size);
        State *start = nfa_stack[0].start;
        
    }
    
    void Regex::concatenation(std::vector<Frag> &nfa_stack,
        unsigned int &stack_index, size_t i)
    {
        Frag &f2 = POP();
        Frag &f1 = POP();
        
        f1.pointOuts(f2.start);
        f1.out = f2.out;
        PUSH(f1);
    }

    void Regex::alternation(std::vector<Frag> &nfa_stack,
        unsigned int &stack_index, size_t i)
    {
        Frag &f2 = POP();
        Frag &f1 = POP();

        f1.append(f2.out);
        Frag split(SPLIT, f1.out, f1.start, f2.start);
        PUSH(split);
    }

   

    void Regex::repetition_star(std::vector<Frag> &nfa_stack,
        unsigned int &stack_index, size_t i)
    {
        Frag    &f = POP();
        Frag    split(SPLIT, NULL, f.start, NULL);
        f.pointOuts(split.start);
        PUSH(split);
    }

    int Regex::get_opp_code(char c)
    {
        switch (c)
        {
        case 'c':
            return CONCATENATION;
        case '|':
            return ALTERNATION;
        case '*':
            return REPETITION;
        default:
            return NORMAL_OPP;
        }
    }

    void    Regex::execute_higher_priority_operator(
        std::vector<char> &pending_opperator,
        int operator_type,
        std::vector<Frag> &nfa_stack,
        unsigned int &stack_index,
        size_t i)
    {
        while (!pending_opperator.empty() &&
            pending_opperator.back() >= operator_type)
        {
            dispatch_operator(get_opp_code(pending_opperator.back()),
                nfa_stack, stack_index, i);
            pending_opperator.pop_back();
        }
    }

    void    Regex::dispatch_operator(
        int operator_type,
        std::vector<Frag> &nfa_stack,
        unsigned int &stack_index,
        size_t i)
    {
        switch (operator_type)
        {
        case ALTERNATION:
            alternation(nfa_stack, stack_index, i);
            break;
        case CONCATENATION:
            concatenation(nfa_stack, stack_index, i);
            break;
        }
    }

    void Regex::check_concatenation(
        int last_operator, std::vector<char> &pending_opperator)
    {
        if (last_operator != ALTERNATION && last_operator != NO_CONCATENATION)
            pending_opperator.push_back('c');
    }

    Regex::~Regex(){}

#undef PUSH
#undef POP

    Regex::MaxNfaStackSizeReachedException::MaxNfaStackSizeReachedException()
        : std::exception()
    {
    }

    const char *Regex::MaxNfaStackSizeReachedException::what() const throw()
    {
        return ("Max NFA stack size reached Please increase the value of"
                " ft::Regex::max_nfa_stack_size or reduce the pattern"
                " size/complexity");
    }

}


int main()
{
    ft::Regex r("abc|de|f*");
    std::cout << "Hello World!\n";
    return 0;
}
