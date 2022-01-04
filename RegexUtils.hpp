#pragma once
#include <string>

namespace ft
{
    struct State
    {
        static  int id_counter;

        int     id;
        int     c;
        State   *out1;
        State   *out2;
        State(int c, State *out1 = NULL, State *out2 = NULL);
    };

    struct Frag
    {
        State *start;
        State **out; // the address of the out pointer of the start state
        Frag(int c, State **out = NULL, State *out1 = NULL, State *out2 = NULL);
        void    pointOuts(State *st)
        {
            State **out_tmp = this->out;
            State **next;
    
            for (; out_tmp; out_tmp=next)
            {
                next = (State **)*out_tmp;
                *out_tmp = st;
            }
        }

        void    append(State **out)
        {
            State **out_tmp = this->out;

            while (*out_tmp)
                out_tmp = (State **)*out_tmp;
            *out_tmp = (State *)out;
        }
    };

    enum
    {
        SPLIT = 130,
        MATCH,
    };

    enum operator_type
    {
        NO_CONCATENATION = -1,
        NORMAL_OPP,
        ALTERNATION,
        CONCATENATION,
        REPETITION,
    };
}