#include <RegexParser.hpp>

namespace ft
{
    Regex::InvalidRegexException::InvalidRegexException
        (const char* error): error(error) {}

    const char* Regex::InvalidRegexException::what() const throw()
    {
        return error;
    }
  

    Regex::Regex(const std::string &regx) : 
        regex(regx), current(regex.begin())
    {
        this->root = this->parse();
    }

    bool Regex::match(std::string const& str) {
        return this->match(str.c_str());
    }

    bool    Regex::match(const char *str)
    {
        this->groups.clear();
        RegexEnd end;
        MatchInfo info;
        info.startOfStr = str;
        info.endOfStr = str + strlen(str);
        info.flags = 0;
        Functor fn(&end, str, 0, &info, NULL);
        for (size_t i = 0; str[i]; i++)
        {
            const char* start = str + i;
            if (this->root->match(start, 0, &info, &fn))
            {
                for (size_t j = 0; j < this->inner_groups.size(); j++)
                {
                    if (this->inner_groups[j]->getCapturedGroup().first > this->inner_groups[j]->getCapturedGroup().second)
                        this->groups.push_back("");
                    else 
                    {
                        std::string group(this->inner_groups[j]->getCapturedGroup().first,
                                      this->inner_groups[j]->getCapturedGroup().second);
                        this->groups.push_back(group);
                    }
                    this->inner_groups[j]->component.group->first = NULL;
                    this->inner_groups[j]->component.group->second = NULL;
                }
                return true;
            }
        }
        return false;
    }

    std::vector<std::string> Regex::matchAll(std::string const& str)
    {
        return this->matchAll(str.c_str());
    }

    std::vector<std::string> Regex::matchAll(const char *str)
    {
        std::vector<std::string> result;
        while (this->match(str))
        {
            result.push_back(groups[0]);
            str += groups[0].size();
        }
        return result;
    }



    Regex::~Regex() {}

    char Regex::peek()
    {
        return *current;
    }

    char Regex::eat(char c, const char* error)
    {
        if (peek() != c)
            throw InvalidRegexException(error);
        current++;
        return c;
    }

    char Regex::next()
    {
        char c = peek();
        return eat(c, "Unexpected character");
    }

    bool Regex::hasMoreChars()
    {
        return current != regex.end();
    }

    bool Regex::isRepeatChar(char c)
    {
        return c == '*' || c == '+' || c == '?';
    }

    // alternate here
    RegexComponentBase *Regex::expr()
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
    RegexComponentBase *Regex::term()
    {
        RegexComponentBase *f = factor();
        if (hasMoreChars() && peek() != ')' && peek() != '|')
        {
            RegexComponentBase *t = term();
            return concat(f, t);
        }

        return f;
    }

    RegexComponentBase *Regex::factor()
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

    std::pair<long long, long long> Regex::repeat_range()
    {
        long long min = integer();
        long long max = min;

        if (hasMoreChars() && peek() == ',')
        {
            eat(',', "expected ','");
            if (hasMoreChars() && isdigit(peek()))
                max = integer();
            else
                max = Regex::Infinity;
        }
        return std::make_pair(min, max);
    }

    RegexComponentBase *Regex::atom()
    {
        RegexComponentBase *res;
        if (hasMoreChars() && peek() == '(')
        {
            eat('(', "expected '('");
            res = group();
            eat(')', "expected ')'");
            return res;
        }
        else if (hasMoreChars() && peek() == '[')
        {
            eat('[', "expected '['");
            res = charGroup();
            eat(']', "expected ']'");

            return res;
        }
        return chr();
    }

    RegexComponentBase *Regex::group()
    {
        RegexComponentBase *res;
        if (hasMoreChars() && peek() == '?')
        {
            eat('?', "expected '?'");
            if (next() == ':')
                return expr();
            else
                throw InvalidRegexException("unexpected char after '?'");
        }
        RegexStartOfGroup *group = new RegexStartOfGroup();
        inner_groups.push_back(group);
        res = expr();
        RegexEndOfGroup *end = new RegexEndOfGroup(group);
        res = concat(group, concat(res, end));
        return res;
    }
    
    RegexComponentBase* Regex::charGroup()
    {
        RegexComponentBase *res;
        if (hasMoreChars() && peek() == '^')
        {
            eat('^', "expected '^'");
            res = new RegexInverseGroup();
        }
        else
            res = new RegexGroup();
        if (hasMoreChars() && peek() == ']')
            throw InvalidRegexException("unexpected character ']'");
        return charGroupBody(res);
    }

    RegexComponentBase* Regex::charGroupBody(RegexComponentBase *res)
    {
        RegexComponentBase *r = res;
        if (!hasMoreChars() || peek() == ']')
            return r;
        char c = next();
        if (hasMoreChars() && peek() == '-')
        {
            eat('-', "expected '-'");
            charGroupRange(c, r);
            return charGroupBody(r);
        }
        else
        {
            if (c != '\\')
                res->addChar(c);
            else
            {
                charGroupSkiped(next(), res); // TODO: handle escape
            }
            return charGroupBody(res);
        }
    }
    RegexComponentBase*     Regex::charGroupSkiped(char c, RegexComponentBase*res)
    {
        if (c == 'd' || c == 'D')
            res->addRangeChar('0', '9');
        else if (c == 'w' || c == 'W')
        {
            res->addRangeChar('a', 'z');
            res->addRangeChar('A', 'Z');
            res->addRangeChar('0', '9');
            res->addChar('_');
        }
        else if (c == 's' || c == 'S')
        {
            res->addRangeChar(' ', ' ');
            res->addRangeChar('\t', '\t');
            res->addRangeChar('\n', '\n');
            res->addRangeChar('\r', '\r');
        }
        else if (c == 'n')
            res->addChar('\n');
        else if (c == 'r')
            res->addChar('\r');
        else if (c == 't')
            res->addChar('\t');
        else if (c == 'f')
            res->addChar('\f');
        else if (c == 'v')
            res->addChar('\v');
        else if (c == 'b')
            res->addChar('\b');
        else if (c == 'a')
            res->addChar('\a');
        else if (isalpha(c))
            throw InvalidRegexException("unexpected character after \\");
        else
            res->addChar(c);
        return res;
    }

    RegexComponentBase* Regex::charGroupRange(char c, RegexComponentBase *res)
    {
        if (hasMoreChars() && peek() == '\\')
            throw InvalidRegexException("unexpected character '\\'");        
        else if (hasMoreChars() && peek() != ']')
        {
            char c2 = next();
            if (c2 < c)
                throw InvalidRegexException("invalid range");
            for (char i = c; i <= c2; i++)
                res->addChar(i);
        } else
        {
            res->addChar(c);
            res->addChar('-');
        }
        return res;
    }
    // ^
    
    RegexComponentBase *Regex::chr()
    {
        if (isRepeatChar(peek()) || peek() == '{' || peek() == ')')
            throw InvalidRegexException("Unexpected character");
        if (peek() == '\\')
        {
            eat('\\', "expected '\\'");
            if (!hasMoreChars())
                throw InvalidRegexException("Unexpected end of regex");
            return construct_skiped_char();
        }
        else if (peek() == '.')
        {
            eat('.', "expected '.'");
            return new RegexInverseGroup('\n');
        }
        else if (peek() == '^')
        {
            eat('^', "expected '^'");
            startOfLines.push_back(new RegexStartOfLine());
            return startOfLines.back();
        }
        else if (peek() == '$')
        {
            eat('$', "expected '$'");
            return new RegexEndOfLine();
        }
 
        RegexGroup *res = new RegexGroup();
        res->addChar(next());
        return res;
    }

    RegexComponentBase *Regex::construct_skiped_char()
    {
        if (isdigit(peek()))
        {
            long long n = integer();
            if (n >= static_cast<long long>(inner_groups.size()))
                throw InvalidRegexException("Invalid group reference");
            RegexBackReference *ref = new RegexBackReference(inner_groups[n]);
            return ref;
        }
        else if (peek() == 'd')
        {
            next();
            return new RegexGroup('0', '9');
        }
        else if (peek() == 'D')
        {
            next();
            return new RegexInverseGroup('0', '9');
        }
        else if (peek() == 'w')
        {
            next();
            RegexGroup *r = new RegexGroup ('a', 'z');
            r->addRangeChar('A', 'Z');
            r->addRangeChar('0', '9');
            r->addChar('_');
            return r;
        }
        else if (peek() == 'W')
        {
            next();
            RegexInverseGroup *r = new RegexInverseGroup('a', 'z');
            r->addRangeChar('A', 'Z');
            r->addRangeChar('0', '9');
            r->addChar('_');
            return r;
        }
        else if (peek() == 's')
        {
            next();
            RegexGroup *r = new RegexGroup();
            r->addRangeChar('\t', '\r');
            r->addChar(' ');
            return r;
        }
        else if (peek() == 'S')
        {
            next();
            RegexInverseGroup *r = new RegexInverseGroup();
            r->addRangeChar('\t', '\r');
            r->addChar(' ');
            return r;
        }
        else if (peek() == 'b')
        {
            next();
            return new RegexWordBoundary();
        }
        else if (peek() == 't')
        {
            next();
            return new RegexGroup('\t');
        }
        else if (peek() == 'n')
        {
            next();
            return new RegexGroup('\n');
        }
        else if (peek() == 'r')
        {
            next();
            return new RegexGroup('\r');
        }
        else if (peek() == 'f')
        {
            next();
            return new RegexGroup('\f');
        }
        else if (peek() == 'a')
        {
            next();
            return new RegexGroup('\a');
        }
        else
        {
            char c = peek();
            if (c == '\\' || c == '.' || c == '[' || c == ']' || c == '^' || c == '$' || c == '-' || isRepeatChar(c) || c == '/')
            {
                next();
                return new RegexGroup(c);
            }
            throw InvalidRegexException("Unexpected character after \\");
        }

    }
    
    long long Regex::integer()
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

    RegexComponentBase *Regex::parse()
    {

        RegexStartOfGroup *group = new RegexStartOfGroup();
        RegexEndOfGroup *end = new RegexEndOfGroup(group);
        inner_groups.push_back(group);
        RegexComponentBase *res = expr();
        res = concat(group, concat(res, end));
        if (hasMoreChars())
            throw InvalidRegexException("Unexpected character"
                " at the end of Regex");
        return res;
    }

    RegexComponentBase *
    Regex::concat(RegexComponentBase *a, RegexComponentBase *b)
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
    Regex::alter(RegexComponentBase *a, RegexComponentBase *b)
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

    RegexComponentBase *Regex::repeat(RegexComponentBase *a, char r)
    {
        if (r == '*')
        {
            if (hasMoreChars() && peek() == '?')
            {
                next();
                return new RegexRepeatLazy(a, 0, Regex::Infinity);
            }
            return repeat(a, 0, Regex::Infinity);
        }
        else if (r == '+')
            return repeat(a, 1, Regex::Infinity);
        else if (r == '?')
            return repeat(a, 0, 1);
        else
            throw InvalidRegexException("Unexpected character");
    }

    RegexComponentBase *
    Regex::repeat(RegexComponentBase *a, long long min, long long max)
    {
        if (min > max || min < 0 || max < 0)
            throw InvalidRegexException("Invalid repeat range");
        if (max != Regex::Infinity && max > Regex::MaxRepeat)
            throw InvalidRegexException("Too many repeats (max: 1024)");
        return new RegexRepeat(a, min, max);
    }

}