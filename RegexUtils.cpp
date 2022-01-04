#include <RegexUtils.hpp>
namespace ft
{
    int State::id_counter = 0;

    Frag::Frag(int c, State **out, State *out1, State *out2)
        : start(new State(c, out1, out2)), out(out)
    {
        if (out)
            return ;
        if (!out && !out1)
            this->out = &start->out1;
        else if (!out && !out2)
            this->out = &start->out2;
        else // TODO: only for testing
            throw "Invalid Frag constructor";
    }

    State::State(int c, State *out1, State *out2)
        : id(++State::id_counter), c(c), out1(out1), out2(out2) {}

}
