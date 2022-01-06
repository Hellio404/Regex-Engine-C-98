#pragma once

#include <RegexUtils.hpp>
#include <string>
#include <vector>
#include <limits>
#include <exception>

namespace ft
{

class RegexParser
{
    std::string regex;
    std::string::const_iterator current;
    RegexComponentBase* root;
    static const long long Infinity = __LONG_LONG_MAX__;
    static const long long MaxRepeat = 1024;
public:
    std::vector <RegexStartOfGroup *> groups;
    RegexParser(const std::string &regex);
    ~RegexParser();
    bool                    match(std::string const&);

private:
    char                    peek();
    char                    eat(char, const char*);
    char                    next();
    bool                    hasMoreChars();
    bool                    isRepeatChar(char);

    long long               integer();
    std::pair<long long, long long> repeat_range();


    RegexComponentBase*     expr();
    RegexComponentBase*     term();
    RegexComponentBase*     factor();
    RegexComponentBase*     atom();
    RegexComponentBase*     chr();


    RegexComponentBase*     repeat(RegexComponentBase *, long long, long long);
    RegexComponentBase*     repeat(RegexComponentBase *, char);
    RegexComponentBase*     concat(RegexComponentBase *, RegexComponentBase *);
    RegexComponentBase*     alter(RegexComponentBase *, RegexComponentBase *);

    RegexComponentBase*     construct_skiped_char(RegexGroup *, char);

    RegexComponentBase*     parse();

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