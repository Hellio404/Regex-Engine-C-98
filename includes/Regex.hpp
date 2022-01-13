#pragma once

#include "../RegexUtils.hpp"
#include <string>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <limits>
#include <exception>
#include <iostream>

namespace ft
{

struct CustomLongLong
{
    long long value;

    CustomLongLong(long long value);
    CustomLongLong(const CustomLongLong &other);
    CustomLongLong &operator=(const CustomLongLong &other);

    bool operator==(const CustomLongLong &other) const;
    bool operator!=(const CustomLongLong &other) const;
    bool operator<(const CustomLongLong &other) const;
    bool operator>(const CustomLongLong &other) const;
    bool operator<=(const CustomLongLong &other) const;
    bool operator>=(const CustomLongLong &other) const;
    CustomLongLong operator+(const CustomLongLong &other) const;
    CustomLongLong operator+(long long other) const;
    CustomLongLong operator-(const CustomLongLong &other) const;
    CustomLongLong operator-(long long other) const;
    CustomLongLong operator*(const CustomLongLong &other) const;
    CustomLongLong operator*(long long other) const;
    CustomLongLong operator/(const CustomLongLong &other) const;
    CustomLongLong operator/(long long other) const;
};




class Regex
{
    std::string     regex;
    unsigned int    flags;
    std::string::const_iterator current;
    RegexComponentBase* root;
    static const long long Infinity = __LONG_LONG_MAX__;
    static const long long MaxRepeat = 1024;
    std::vector <RegexStartOfGroup *>   inner_groups;
    bool                                allowed_repeat;
    struct ret_t
    {
        CustomLongLong min;
        CustomLongLong max;
        RegexComponentBase* c;
        ret_t(CustomLongLong , CustomLongLong , RegexComponentBase* );
    };

public:
    struct  result_t
    {
        std::string str;
        std::vector<std::string> groups;
    };
    
    Regex(const std::string &regex, unsigned int = 0);
    ~Regex();
    bool                        match(std::string const&, result_t &);
    bool                        match(const char *, result_t &);
    std::vector<result_t>       matchAll(std::string const&);
    std::vector<result_t>       matchAll(const char*);
    bool                        test(std::string const&);
    bool                        test(const char*);
    enum 
    {
        iCase = 4,
    };
    
private:
    char                    peek();
    char                    eat(char, const char*);
    char                    next();
    bool                    hasMoreChars();
    bool                    isRepeatChar(char);

    long long               integer();
    std::pair<long long, long long> repeat_range();


    ret_t                   expr();
    ret_t                   term();
    ret_t                   factor();
    ret_t                   group();
    ret_t                   atom();
    RegexComponentBase*     chr();
    RegexComponentBase*     charGroup();
    RegexComponentBase*     charGroupBody(RegexComponentBase*);
    RegexComponentBase*     charGroupSkiped(char, RegexComponentBase*);
    RegexComponentBase*     charGroupRange(char, RegexComponentBase*);

    ret_t                   repeat(ret_t, long long, long long, bool = true);
    ret_t                   repeat(ret_t, char);
    ret_t                   concat(ret_t, ret_t);
    ret_t                   alter(ret_t, ret_t);

    RegexComponentBase*     construct_skiped_char();
    ret_t                   expr_without_repeat();

    RegexComponentBase      *parse();

public:
    class InvalidRegexException : public std::exception
    {
        const char* error;
    public:
        InvalidRegexException(const char* error);
        const char* what() const throw();
    };
};

} // namespace ft

/*

ab(c*|d)e\1 abccccceccccc


*/