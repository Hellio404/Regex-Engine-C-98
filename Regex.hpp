#pragma once

#include <RegexUtils.hpp>
#include <string>
#include <vector>
#include <limits>
#include <exception>

namespace ft
{

class Regex
{
    std::string regex;
    std::string::const_iterator current;
    RegexComponentBase* root;
    static const long long Infinity = __LONG_LONG_MAX__;
    static const long long MaxRepeat = 1024;
    std::vector <RegexStartOfLine *> startOfLines;
    std::vector <RegexStartOfGroup *> inner_groups;


public:
    std::vector <std::string> groups;
    
    Regex(const std::string &regex);
    ~Regex();
    bool                        match(std::string const&);
    bool                        match(const char *);
    std::vector<std::string >   matchAll(std::string const&);
    std::vector<std::string >   matchAll(const char*);
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


    RegexComponentBase*     expr();
    RegexComponentBase*     term();
    RegexComponentBase*     factor();
    RegexComponentBase*     group();
    RegexComponentBase*     atom();
    RegexComponentBase*     chr();
    RegexComponentBase*     charGroup();
    RegexComponentBase*     charGroupBody(RegexComponentBase*);
    RegexComponentBase*     charGroupSkiped(char, RegexComponentBase*);
    RegexComponentBase*     charGroupRange(char, RegexComponentBase*);

    RegexComponentBase*     repeat(RegexComponentBase *, long long, long long, bool = true);
    RegexComponentBase*     repeat(RegexComponentBase *, char);
    RegexComponentBase*     concat(RegexComponentBase *, RegexComponentBase *);
    RegexComponentBase*     alter(RegexComponentBase *, RegexComponentBase *);

    RegexComponentBase*     construct_skiped_char();

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

/*

ab(c*|d)e\1 abccccceccccc


*/