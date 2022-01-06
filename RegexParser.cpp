#include <RegexParser.hpp>

namespace ft
{
    RegexParser::InvalidRegexException::InvalidRegexException
        (const char* error): error(error) {}

    const char* RegexParser::InvalidRegexException::what() const throw()
    {
        return error;
    }
  

    RegexParser::RegexParser(const std::string &regx) : 
        regex(regx), current(regex.begin())
    {
        this->root = this->parse();
    }

    bool RegexParser::match(std::string const& str) {
        const char* chr = str.c_str();
        RegexEnd end;
        Functor fn(&end, chr, 0, NULL);
        return this->root->match(chr, 0, &fn);
    }


    RegexParser::~RegexParser() {}

    char RegexParser::peek()
    {
        return *current;
    }

    char RegexParser::eat(char c, const char* error)
    {
        if (peek() != c)
            throw (error);
        current++;
        return c;
    }

    char RegexParser::next()
    {
        char c = peek();
        return eat(c, "Unexpected character");
    }

    bool RegexParser::hasMoreChars()
    {
        return current != regex.end();
    }

    bool RegexParser::isRepeatChar(char c)
    {
        return c == '*' || c == '+' || c == '?';
    }

    // alternate here
    RegexComponentBase *RegexParser::expr()
    {
        RegexComponentBase *t = term();
        if (hasMoreChars() && peek() == '|')
        {
            eat('|', "expected '|'");
            RegexComponentBase *e = expr();
            return alter(t, e);
        }
        return t;
    }

    // concat here
    RegexComponentBase *RegexParser::term()
    {
        RegexComponentBase *f = factor();
        if (hasMoreChars() && peek() != ')' && peek() != '|')
        {
            RegexComponentBase *t = term();
            return concat(f, t);
        }

        return f;
    }

    RegexComponentBase *RegexParser::factor()
    {
        RegexComponentBase *a = atom();
        RegexComponentBase *res = a;

        if (hasMoreChars() && isRepeatChar(peek()))
        {
            char r = next();
            res = repeat(a, r);
        }
        else if (hasMoreChars() && peek() == '{')
        {
            eat('{', "expected '{'");
            std::pair<long long, long long> r = repeat_range();
            res = repeat(a, r.first, r.second);
            eat('}', "expected '}'");
        }
        return res;
    }

    std::pair<long long, long long> RegexParser::repeat_range()
    {
        long long min = integer();
        long long max = min;

        if (hasMoreChars() && peek() == ',')
        {
            eat(',', "expected ','");
            if (hasMoreChars() && isdigit(peek()))
                max = integer();
            else
                max = RegexParser::Infinity;
        }
        return std::make_pair(min, max);
    }

    RegexComponentBase *RegexParser::atom()
    {
        RegexComponentBase *res;
        if (peek() == '(')
        {
            eat('(', "expected '('");
            RegexStartOfGroup *group = new RegexStartOfGroup();
            groups.push_back(group);
            res = expr();
            RegexEndOfGroup *end = new RegexEndOfGroup(group);
            res = concat(group, concat(res, end));
            eat(')', "expected ')'");
            return res;
        }
        return chr();
    }

    RegexComponentBase *RegexParser::chr()
    {
        RegexGroup *res = new RegexGroup();
        if (isRepeatChar(peek()) || peek() == '{' || peek() == ')')
            throw InvalidRegexException("Unexpected character");
        if (peek() == '\\')
        {
            eat('\\', "expected '\\'");
            if (!hasMoreChars())
                throw InvalidRegexException("Unexpected end of regex");
            return construct_skiped_char(res, next());
        }
        res->addChar(next());
        return res;
    }

    // TODO: implement
    RegexComponentBase *RegexParser::construct_skiped_char(RegexGroup *res, char c)
    {
        res->addChar(c);
        return res;
    }

    long long RegexParser::integer()
    {
        std::string num;
        if (!hasMoreChars())
            throw InvalidRegexException("Unexpected end of regex");
        if (!isdigit(peek()))
            throw InvalidRegexException("Expected digit");

        while (hasMoreChars() && isdigit(peek()))
            num += next();
        return std::atoll(num.c_str());
    }

    RegexComponentBase *RegexParser::parse()
    {

        RegexStartOfGroup *group = new RegexStartOfGroup();
        RegexEndOfGroup *end = new RegexEndOfGroup(group);
        groups.push_back(group);
        RegexComponentBase *res = expr();
        res = concat(group, concat(res, end));
        if (hasMoreChars())
            throw InvalidRegexException("Unexpected character"
                " at the end of Regex");
        return res;
    }

    RegexComponentBase *
    RegexParser::concat(RegexComponentBase *a, RegexComponentBase *b)
    {
        if (a->type == RegexComponentBase::CONCAT)
        {
            a->addChild(b);
            return a;
        }
        else if (b->type == RegexComponentBase::CONCAT)
        {
            b->component.children->insert(b->component.children->begin(), a);
            return b;
        }
        else
        {
            RegexConcat *res = new RegexConcat();
            res->addChild(a);
            res->addChild(b);
            return res;
        }
    }

    RegexComponentBase *
    RegexParser::alter(RegexComponentBase *a, RegexComponentBase *b)
    {

        if (a->type == RegexComponentBase::ALTERNATE)
        {
            
            a->addChild(b);
            return a;
        }
        else if (b->type == RegexComponentBase::ALTERNATE)
        {
            b->component.children->insert(b->component.children->begin(), a);
            return b;
        }
        else
        {
            RegexAlternate *res = new RegexAlternate();
            res->addChild(a);
            res->addChild(b);
            return res;
        }
    }

    RegexComponentBase *RegexParser::repeat(RegexComponentBase *a, char r)
    {
        if (r == '*')
            return repeat(a, 0, RegexParser::Infinity);
        else if (r == '+')
            return repeat(a, 1, RegexParser::Infinity);
        else if (r == '?')
            return repeat(a, 0, 1);
        else
            throw InvalidRegexException("Unexpected character");
    }

    RegexComponentBase *RegexParser::repeat(RegexComponentBase *a, long long min, long long max)
    {
        if (min > max || min < 0 || max < 0)
            throw InvalidRegexException("Invalid repeat range");
        if (max != RegexParser::Infinity && max > RegexParser::MaxRepeat)
            throw InvalidRegexException("Too many repeats (max: 1024)");
        return new RegexRepeat(a, min, max);
    }

}