#include <Regex.hpp>
namespace ft
{
    CustomLongLong operator+(long long lhs, const CustomLongLong &rhs)
    {
        CustomLongLong res(rhs + lhs);

        if (res.value == (-__LONG_LONG_MAX__ - 1))
            return CustomLongLong(__LONG_LONG_MAX__);
        return CustomLongLong(-res.value);
    }

    CustomLongLong operator-(long long lhs, const CustomLongLong &rhs)
    {
        return rhs - lhs;
    }

    CustomLongLong operator*(long long lhs, const CustomLongLong &rhs)
    {
        return CustomLongLong(rhs * lhs);
    }

    CustomLongLong operator/(long long lhs, const CustomLongLong &rhs)
    {
        return CustomLongLong(lhs / rhs.value);
    }

    CustomLongLong::CustomLongLong(long long value) : value(value) {}
    CustomLongLong::CustomLongLong(const CustomLongLong &other) : value(other.value) {}
    CustomLongLong &CustomLongLong::operator=(const CustomLongLong &other)
    {
        value = other.value;
        return *this;
    }

    bool CustomLongLong::operator==(const CustomLongLong &other) const
    {
        return value == other.value;
    }

    bool CustomLongLong::operator!=(const CustomLongLong &other) const
    {
        return value != other.value;
    }

    bool CustomLongLong::operator<(const CustomLongLong &other) const
    {
        return value < other.value;
    }

    bool CustomLongLong::operator>(const CustomLongLong &other) const
    {
        return value > other.value;
    }

    bool CustomLongLong::operator<=(const CustomLongLong &other) const
    {
        return value <= other.value;
    }

    bool CustomLongLong::operator>=(const CustomLongLong &other) const
    {
        return value >= other.value;
    }

    CustomLongLong CustomLongLong::operator+(const CustomLongLong &other) const
    {
        return CustomLongLong(*this + other.value);
    }

    CustomLongLong CustomLongLong::operator+(long long other) const
    {
        if (this->value >= 0 && __LONG_LONG_MAX__ - this->value < other)
            return CustomLongLong(__LONG_LONG_MAX__);
        if (this->value < 0 && (-__LONG_LONG_MAX__ - 1) - this->value > other)
            return CustomLongLong(-__LONG_LONG_MAX__ - 1);
        else
            return CustomLongLong(this->value + other);
    }


    CustomLongLong CustomLongLong::operator-(const CustomLongLong &other) const
    {
        return CustomLongLong(*this - other.value);
    }

    CustomLongLong CustomLongLong::operator-(long long other) const
    {
        // check if this->value - other will overflow or underflow if it does, return the max or min value
        if (this->value < 0 && other == (-__LONG_LONG_MAX__ - 1))
            return CustomLongLong((-__LONG_LONG_MAX__ - 1));
        if (other == (-__LONG_LONG_MAX__ - 1))
            return CustomLongLong(this->value - other); 
        return CustomLongLong(*this + -other);
    }

    CustomLongLong CustomLongLong::operator*(const CustomLongLong &other) const
    {
        return CustomLongLong(*this * other.value);
    }

    CustomLongLong CustomLongLong::operator*(long long other) const
    {
        bool is_negative = false;
        if (other == (-__LONG_LONG_MAX__ - 1) && this->value  != 0)
            return (*this < 0) ? CustomLongLong(-__LONG_LONG_MAX__ - 1) : CustomLongLong(__LONG_LONG_MAX__);
        else if (this->value == (-__LONG_LONG_MAX__ - 1) && other != 0)
            return (other < 0) ? CustomLongLong(__LONG_LONG_MAX__) : CustomLongLong(-__LONG_LONG_MAX__ - 1);
        is_negative = (other < 0) ^ (this->value < 0);
        if (other < 0)
            other = -other;
        long long val = this->value;
        if (this->value < 0)
            val = -val;
        if (other != 0 && this->value > __LONG_LONG_MAX__ / other)
            return is_negative ? CustomLongLong(-__LONG_LONG_MAX__ - 1) : CustomLongLong(__LONG_LONG_MAX__);
        return CustomLongLong(val * other);
    }
    
    CustomLongLong CustomLongLong::operator/(const CustomLongLong &other) const
    {
        return CustomLongLong(*this / other.value);
    }

    CustomLongLong CustomLongLong::operator/(long long other) const
    {
        return CustomLongLong(this->value / other);
    }

    Regex::InvalidRegexException::InvalidRegexException
        (const char* error): error(error) {
            
        }

    const char* Regex::InvalidRegexException::what() const throw()
    {
        return error;
    }
    
    Regex::ret_t::ret_t(CustomLongLong min, CustomLongLong max, RegexComponentBase* c):
         min(min), max(max), c(c) {}

    Regex::Regex(const std::string &regx, unsigned int flags) : 
        regex(regx), flags(flags), current(regex.begin()), allowed_repeat(true)
    {
        this->root = this->parse();
    }

    RegexComponentBase*
    Regex::parse()
    {

        RegexStartOfGroup *group = new RegexStartOfGroup();
        RegexEndOfGroup *end = new RegexEndOfGroup(group);
        inner_groups.push_back(group);
        ret_t   res = expr();
        res = concat(ret_t(0, 0, group), concat(res, ret_t(0, 0, end)));
        if (hasMoreChars())
            throw InvalidRegexException("Unexpected character"
                " at the end of Regex");
        return res.c;
    }

    bool Regex::match(std::string const& str, result_t &r) 
    {
        return this->match(str.c_str(), r);
    }

    bool    Regex::match(const char *str, result_t &r)
    {
        RegexEnd end;
        MatchInfo info;
        info.startOfStr = str;
        info.endOfStr = str + std::strlen(str);
        info.flags = flags;
        Functor fn(&end, str, 0, &info, NULL);
        for (size_t i = 0; str[i]; i++)
        {
            const char* start = str + i;
            if (this->root->match(start, 0, &info, &fn))
            {
                for (size_t j = 0; j < this->inner_groups.size(); j++)
                {
                    
                    if (this->inner_groups[j]->getCapturedGroup().first > this->inner_groups[j]->getCapturedGroup().second)
                        r.groups.push_back("");
                    else 
                    {
                        std::string group(this->inner_groups[j]->getCapturedGroup().first,
                                      this->inner_groups[j]->getCapturedGroup().second);
                        r.groups.push_back(group);
                    }
                    if (j == 0)
                        r.str = r.groups.back();
                    this->inner_groups[j]->component.group->first = NULL;
                    this->inner_groups[j]->component.group->second = NULL;
                }
                return true;
            }
        }
        return false;
    }

    std::vector<Regex::result_t> Regex::matchAll(std::string const& str)
    {
        return this->matchAll(str.c_str());
    }

    std::vector<Regex::result_t> Regex::matchAll(const char *str)
    {
        std::vector<result_t> result(1);
        while (this->match(str, result.back()))
        {
           result.resize(result.size() + 1);
        }
        result.pop_back();
        return result;
    }

    bool    Regex::test(const char *str)
    {
        result_t r;
        return this->match(str, r);
    }

    bool    Regex::test(std::string const& str)
    {
        return this->test(str.c_str());
    }

    Regex::~Regex() {
        delete this->root;
    }

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
    Regex::ret_t
    Regex::expr()
    {
        ret_t t = term();
        if (hasMoreChars() && peek() == '|')
        {
            eat('|', "expected '|'");
            ret_t e = expr();
            return alter(t, e);
        }
        return t;
    }

    // concat here
    Regex::ret_t
    Regex::term()
    {
        ret_t f = factor();
        if (hasMoreChars() && peek() != ')' && peek() != '|')
        {
            ret_t t = term();
            return concat(f, t);
        }

        return f;
    }

    Regex::ret_t
    Regex::factor()
    {
        ret_t a = atom();
        // disable repeat for (?<=...) and (?<!...)
        if (hasMoreChars() && isRepeatChar(peek()))
        {
            if (!allowed_repeat)
                throw InvalidRegexException("Unexpected repeat inside lookup group");
            char r = next();
            return repeat(a, r);
        }
        else if (hasMoreChars() && peek() == '{')
        {
            if (!allowed_repeat)
                throw InvalidRegexException("Unexpected repeat inside lookup group");
            eat('{', "expected '{'");
            std::pair<long long, long long> r = repeat_range();
            eat('}', "expected '}'");
            return repeat(a, r.first, r.second);
        }
        return a;
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

    Regex::ret_t
    Regex::atom()
    {
        if (hasMoreChars() && peek() == '(')
        {
            eat('(', "expected '('");
            ret_t const& grp = group();
            eat(')', "expected ')'");
            return grp;
        }
        else if (hasMoreChars() && peek() == '[')
        {
            eat('[', "expected '['");
            RegexComponentBase *res = charGroup();
            eat(']', "expected ']'");
            return ret_t(1, 1 , res);
        }
        return ret_t(1, 1, chr());
    }

    Regex::ret_t
    Regex::expr_without_repeat()
    {
        allowed_repeat = false;
        ret_t ret = expr();
        allowed_repeat = true;
        return ret;
    }

    Regex::ret_t
    Regex::group()
    {
        if (hasMoreChars() && peek() == '?')
        {
            eat('?', "expected '?'");
            char c = next();
            if (c == ':')
                return expr();
            else if (c == '<')
            {
                c = next();
                if (c != '=' && c != '!')
                    throw InvalidRegexException("unexpected char after '?<'");
                ret_t ret = expr_without_repeat();
                if (c == '=')
                {
                    RegexComponentBase *res = new RegexPositiveLookBehind();
                    res->component.range->child = ret.c;
                    res->component.range->min = ret.min.value;
                    res->component.range->max = ret.max.value;
                    return ret_t(0, 0, res);
                }
                else if (c == '!')
                {
                    RegexComponentBase *res = new RegexNegativeLookBehind();
                    res->component.range->child = ret.c;
                    res->component.range->min = ret.min.value;
                    res->component.range->max = ret.max.value;
                    return ret_t(0, 0, res);
                }
            }
            else if (c == '=')
            {
                ret_t ret = expr_without_repeat();
                RegexComponentBase *res = new RegexPositiveLookAhead();
                res->component.range->child = ret.c;
                res->component.range->min = ret.min.value;
                res->component.range->max = ret.max.value;
                return ret_t(0, 0, res);
            }
            else if (c == '!')
            {
                ret_t ret = expr_without_repeat();
                RegexComponentBase *res = new RegexNegativeLookAhead();
                res->component.range->child = ret.c;
                res->component.range->min = ret.min.value;
                res->component.range->max = ret.max.value;
                return ret_t(0, 0, res);
            }
            else
                throw InvalidRegexException("unexpected char after '?'");
        }
        RegexStartOfGroup *group = new RegexStartOfGroup();
        inner_groups.push_back(group);
        RegexEndOfGroup *end = new RegexEndOfGroup(group);
        ret_t const& res = expr();
        return concat(ret_t(0, 0, group), concat(res, ret_t(0, 0, end)));
    }
    
    RegexComponentBase*
    Regex::charGroup()
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

    RegexComponentBase*
    Regex::charGroupBody(RegexComponentBase *res)
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
                charGroupSkiped(next(), res);
            }
            return charGroupBody(res);
        }
    }

    RegexComponentBase*
    Regex::charGroupSkiped(char c, RegexComponentBase*res)
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

    RegexComponentBase* 
    Regex::charGroupRange(char c, RegexComponentBase *res)
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
    
    RegexComponentBase*
    Regex::chr()
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
            return new RegexStartOfLine();
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

    RegexComponentBase*
    Regex::construct_skiped_char()
    {
        // TODO: disable backrefs inside (?<=...) and (?<!...)
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
        else if (peek() == 'B')
        {
            next();
            return new RegexNonWordBoundary();
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
            next();
            return new RegexGroup(c);
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
        return std::atol(num.c_str());
    }

    

    Regex::ret_t
    Regex::concat(ret_t a, ret_t b)
    {
        if (a.c->type == RegexComponentBase::CONCAT)
        {
            a.c->addChild(b.c);
            return ret_t(a.min + b.min, a.max + b.max, a.c);
        }
        else if (b.c->type == RegexComponentBase::CONCAT)
        {
            b.c->component.children->insert(b.c->component.children->begin(), a.c);
            return ret_t(a.min + b.min, a.max + b.max, b.c);
        }
        else
        {
            RegexConcat *res = new RegexConcat();
            res->addChild(a.c);
            res->addChild(b.c);
            return ret_t(a.min + b.min, a.max + b.max, res);
        }
    }

    Regex::ret_t
    Regex::alter(ret_t a, ret_t b)
    {

        if (a.c->type == RegexComponentBase::ALTERNATE)
        {
           
            a.c->addChild(b.c);
            return ret_t(std::min(a.min, b.min), std::max(a.max, b.max), a.c);
        }
        else if (b.c->type == RegexComponentBase::ALTERNATE)
        {
            b.c->component.children->insert(b.c->component.children->begin(), a.c);
            return  ret_t(std::min(a.min, b.min), std::max(a.max, b.max), b.c);
        }
        else
        {
            RegexAlternate *res = new RegexAlternate();
            res->addChild(a.c);
            res->addChild(b.c);
            return  ret_t(std::min(a.min, b.min), std::max(a.max, b.max), res);
        }
    }

    Regex::ret_t
    Regex::repeat(ret_t a, char r)
    {
        if (r == '*')
            return repeat(a, 0, Regex::Infinity);
        else if (r == '+')
            return repeat(a, 1, Regex::Infinity);
        else if (r == '?')
            return repeat(a, 0, 1);
        else
            throw InvalidRegexException("Unexpected character");
    }

    Regex::ret_t
    Regex::repeat(ret_t a, long long min, long long max, bool checkLazy)
    {
        if (min > max || min < 0 || max < 0)
            throw InvalidRegexException("Invalid repeat range");
        if (max != Regex::Infinity && max > Regex::MaxRepeat)
            throw InvalidRegexException("Too many repeats (max: 1024)");
        if (checkLazy && hasMoreChars() && peek() == '?')
        {
            next();
            return ret_t(
                std::min(min * a.min, CustomLongLong(Regex::Infinity)), 
                std::min(max * a.max, CustomLongLong(Regex::Infinity)),
                new RegexRepeatLazy(a.c, min, max)
            );
        }
        return ret_t(
                std::min(min * a.min, CustomLongLong(Regex::Infinity)), 
                std::min(max * a.max, CustomLongLong(Regex::Infinity)),
                new RegexRepeat(a.c, min, max)
        );
    }

}