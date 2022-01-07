#include <RegexUtils.hpp>

namespace ft
{
     Functor::Functor(
         RegexComponentBase 
         const*executable,
         const char* &ptr,
         unsigned int consumed,
         Functor *next,
         const char* prev): executable(executable), ptr(ptr),
                            consumed(consumed), next(next), prev(prev) {}

    

    

    bool    Functor::run()
    {
        if (!this->executable)
            return true;
        return this->executable->match(this->ptr, this->consumed, this->next, this->prev);
    }

    RegexComponentBase::RegexComponentBase(int type) : type(type)
    {
        switch (type)
        {
        case GROUP:
            this->component.chars = new std::set<char>();
            break;
        case INVERSE_GROUP:
            this->component.chars = new std::set<char>();
            break;
        case REPEAT:
            this->component.range = new RepeatedRange();
            break;
        case CONCAT:
            this->component.children = new std::vector<RegexComponentBase *>();
            break;
        case ALTERNATE:
            this->component.children = new std::vector<RegexComponentBase *>();
            break;
        case START_OF_GROUP:
            this->component.group = new std::pair<const char *, const char *>();
            break;
        case END_OF_GROUP:
            // no need to allocate anything it only needs pointer to groupStart
            break; 
        default:
            break;
        }
    } 

    RegexComponentBase::~RegexComponentBase()
    {
        switch (this->type)
        {
        case GROUP:
            delete this->component.chars;
            break;
        case INVERSE_GROUP:
            delete this->component.chars;
            break;
        case REPEAT:
            delete this->component.children;
            break;
        case CONCAT:
            delete this->component.children;
            break;
        case ALTERNATE:
            delete this->component.children;
            break;
        case START_OF_GROUP:
            delete this->component.group;
            break;
        case END_OF_GROUP:
            break;
        default:
            break;
        }
    }


    // Start RegexGroup
    RegexGroup::RegexGroup() : RegexComponentBase(GROUP) {}

    RegexGroup::RegexGroup(char c) : RegexComponentBase(GROUP)
    {
        addChar(c);
    }

    RegexGroup::RegexGroup(char from, char to) : RegexComponentBase(GROUP)
    {
        addRangeChar(from, to);
    }

    void    RegexGroup::addChar(char c)
    {
        this->component.chars->insert(c);
    }

    void    RegexGroup::addRangeChar(char from, char to)
    {
        for (char c = from; c <= to; c++)
            this->component.chars->insert(c);
    }

    bool    RegexGroup::match(const char* &ptr, unsigned long long, Functor*fn, const char*) const
    {
        if (*ptr && this->component.chars->find(*ptr) != this->component.chars->end())
        {
            ptr++;
            bool    tmp = fn->run();
            ptr--;
            return tmp;
        }
        return (false);
    }

    

    void    RegexGroup::addChild(RegexComponentBase *child)
    {
        throw ("RegexGroup::addChild() not implemented");
    }


    // END RegexGroup

    // Start RegexInverseGroup
    RegexInverseGroup::RegexInverseGroup() : RegexComponentBase(GROUP) {}

    RegexInverseGroup::RegexInverseGroup(char c) : RegexComponentBase(GROUP)
    {
        addChar(c);
    }

    RegexInverseGroup::RegexInverseGroup(char from, char to) 
        : RegexComponentBase(GROUP)
    {
        addRangeChar(from, to);
    }

    void    RegexInverseGroup::addChar(char c)
    {
        this->component.chars->insert(c);
    }

    void    RegexInverseGroup::addRangeChar(char from, char to)
    {
        for (char c = from; c <= to; c++)
            this->component.chars->insert(c);
    }

    bool    RegexInverseGroup::match(const char* &ptr, unsigned long long, Functor*fn, const char*) const
    {
       if (*ptr && this->component.chars->find(*ptr) == this->component.chars->end())
        {
            ptr++;
            bool    tmp = fn->run();
            ptr--;
            return tmp;
        }
        return (false);
    }

   

    void    RegexInverseGroup::addChild(RegexComponentBase *child)
    {
        throw ("RegexInverseGroup::addChild() not implemented");
    }
    // END RegexInverseGroup

    // Start RegexConcat
    RegexConcat::RegexConcat() : RegexComponentBase(CONCAT) {}

    RegexConcat::RegexConcat(
        RegexComponentBase *child1) : RegexComponentBase(CONCAT)
    {
        addChild(child1);
    }

    void    RegexConcat::addChild(RegexComponentBase *child)
    {
        this->component.children->push_back(child);
    }

    bool    RegexConcat::match(const char* &ptr, unsigned long long ctx, Functor*fn, const char*) const
    {
        if (ctx == this->component.children->size())
            return fn->run();
        Functor newFn(this, ptr, ctx + 1, fn);
        return this->component.children->at(ctx)->match(ptr, 0, &newFn);
    }

    void    RegexConcat::addChar(char c)
    {
        throw ("RegexConcat::addChar() not implemented");
    }

    void    RegexConcat::addRangeChar(char from, char to)
    {
        throw ("RegexConcat::addRangeChar() not implemented");
    }
    // END RegexConcat

    // Start RegexAlternate
    RegexAlternate::RegexAlternate() : RegexComponentBase(ALTERNATE) {}

    RegexAlternate::RegexAlternate(
        RegexComponentBase *child1) : RegexComponentBase(ALTERNATE)
    {
        addChild(child1);
    }

    void    RegexAlternate::addChild(RegexComponentBase *child)
    {
        this->component.children->push_back(child);
    }

    bool   RegexAlternate::match(const char* &ptr, unsigned long long ctx, Functor*fn, const char*) const
    {
        if (ctx == this->component.children->size())
            return false;
        bool    res = this->component.children->at(ctx)->match(ptr, 0, fn);
        if (res)
            return res;
        return this->match(ptr, ctx + 1, fn);
    }

    void    RegexAlternate::addChar(char c)
    {
        throw ("RegexAlternate::addChar() not implemented");
    }

    void    RegexAlternate::addRangeChar(char from, char to)
    {
        throw ("RegexAlternate::addRangeChar() not implemented");
    }

    // END RegexAlternate

    // Start RegexRepeat

    RegexRepeat::RegexRepeat() : RegexComponentBase(REPEAT) {
        throw ("RegexRepeat::RegexRepeat() not implemented");
    }

    RegexRepeat::RegexRepeat(
        RegexComponentBase *child1,
        unsigned long long min,
        unsigned long long max) : RegexComponentBase(REPEAT)
    {
        this->component.range->toRepeat = child1;
        this->component.range->min = min;
        this->component.range->max = max;
    }

    RegexRepeat::RegexRepeat(RepeatedRange r) : RegexComponentBase(REPEAT)
    {

        this->component.range->toRepeat = r.toRepeat;
        this->component.range->min = r.min;
        this->component.range->max = r.max;
    }

    bool    RegexRepeat::match(const char* &ptr, unsigned long long ctx, Functor*fn, const char* prev) const
    {
        if (ctx > this->component.range->max)
            return false;

        if (prev == ptr)
            return fn->run();
        
        Functor newFn(this, ptr, ctx + 1, fn, ptr);
        bool    res = this->component.range->toRepeat->match(ptr, 0, &newFn);

        if (res)
            return res;
        if (ctx >= this->component.range->min)
            return fn->run();
        return false;
    }

    void    RegexRepeat::addChild(RegexComponentBase *child)
    {
        this->component.children->push_back(child);
    }

    void    RegexRepeat::addChar(char c)
    {
        throw ("RegexRepeat::addChar() not implemented");
    }

    void    RegexRepeat::addRangeChar(char from, char to)
    {
        throw ("RegexRepeat::addRangeChar() not implemented");
    }

    // END RegexRepeat

    // Start RegexStartOfGroup

    RegexStartOfGroup::RegexStartOfGroup() : 
        RegexComponentBase(START_OF_GROUP)
    {
        this->component.group->first = NULL;
        this->component.group->second = NULL;
    }
    
    bool    RegexStartOfGroup::match(const char* &ptr, unsigned long long ctx, Functor*fn, const char*) const
    {
        std::pair<const char *, const char *> tmp = getCapturedGroup();
        this->component.group->first = ptr;
        bool matched = fn->run();
        if (!matched || !this->component.group->second)
        {
            this->component.group->first = tmp.first;
            this->component.group->second = tmp.second;
        }
        return matched;
    }

    void    RegexStartOfGroup::capture(const char *end)
    {
        if (end != this->component.group->first)
            this->component.group->second = end;
    }

    std::pair<const char *, const char *>
    RegexStartOfGroup::getCapturedGroup() const
    {
        return *this->component.group;
    }

    void    RegexStartOfGroup::addChild(RegexComponentBase *child)
    {
        throw ("RegexStartOfGroup::addChild() not implemented");
    }

    void    RegexStartOfGroup::addChar(char c)
    {
        throw ("RegexStartOfGroup::addChar() not implemented");
    }

    void    RegexStartOfGroup::addRangeChar(char from, char to)
    {
        throw ("RegexStartOfGroup::addRangeChar() not implemented");
    }
    // END RegexStartOfGroup

    // Start RegexEndOfGroup
    
    RegexEndOfGroup::RegexEndOfGroup() : RegexComponentBase(END_OF_GROUP) {}

    RegexEndOfGroup::RegexEndOfGroup(RegexStartOfGroup *group) : 
        RegexComponentBase(END_OF_GROUP)
    {
        this->component.groupStart = group;
    }

    bool    RegexEndOfGroup::match(const char* &ptr, unsigned long long ctx, Functor*fn, const char*) const
    {
        this->component.groupStart->capture(ptr);
        bool res = fn->run();
        return res;
    }

    void    RegexEndOfGroup::addChild(RegexComponentBase *child)
    {
        throw ("RegexEndOfGroup::addChild() not implemented");
    }

    void    RegexEndOfGroup::addChar(char c)
    {
        throw ("RegexEndOfGroup::addChar() not implemented");
    }

    void    RegexEndOfGroup::addRangeChar(char from, char to)
    {
        throw ("RegexEndOfGroup::addRangeChar() not implemented");
    }

    // END RegexEndOfGroup


    // Start RegexEnd
    RegexEnd::RegexEnd() : RegexComponentBase(END) {}

    bool    RegexEnd::match(const char* &, unsigned long long, Functor*, const char*) const
    {
        return true;
    }

    void    RegexEnd::addChild(RegexComponentBase *child)
    {
        throw ("RegexEnd::addChild() not implemented");
    }

    void    RegexEnd::addChar(char c)
    {
        throw ("RegexEnd::addChar() not implemented");
    }

    void    RegexEnd::addRangeChar(char from, char to)
    {
        throw ("RegexEnd::addRangeChar() not implemented");
    }

    // END RegexEnd

    // Start RegexEpsilon

    RegexEpsilon::RegexEpsilon() : RegexComponentBase(EPSILON) {}

    bool    RegexEpsilon::match(const char* &, unsigned long long, Functor*fn, const char*) const
    {
        return fn->run();
    }

    void    RegexEpsilon::addChild(RegexComponentBase *child)
    {
        throw ("RegexEpsilon::addChild() not implemented");
    }

    void    RegexEpsilon::addChar(char c)
    {
        throw ("RegexEpsilon::addChar() not implemented");
    }

    void    RegexEpsilon::addRangeChar(char from, char to)
    {
        throw ("RegexEpsilon::addRangeChar() not implemented");
    }

    // END RegexEpsilon


    // Start RegexBackReference

    RegexBackReference::RegexBackReference() : RegexComponentBase(BACK_REFERENCE) {}

    RegexBackReference::RegexBackReference(RegexStartOfGroup *group) : 
        RegexComponentBase(BACK_REFERENCE)
    {
        this->component.groupStart = group;
    }

    bool    RegexBackReference::match(const char* &ptr, unsigned long long ctx, Functor*fn, const char*) const
    {
        std::pair<const char *, const char *> const& group = this->component.groupStart->getCapturedGroup();
        if (group.first == NULL || group.first == group.second)
            return fn->run();
        const char *start = group.first;
        const char *end = group.second;
        const char *p = ptr;
        while (start != end && *ptr && *start == *ptr)
            ++start, ++ptr;
        bool res = false;
        if (start == end)
            return fn->run();
        ptr = p;
        return res;
    }

    void    RegexBackReference::addChild(RegexComponentBase *child)
    {
        throw ("RegexBackReference::addChild() not implemented");
    }

    void    RegexBackReference::addChar(char c)
    {
        throw ("RegexBackReference::addChar() not implemented");
    }

    void    RegexBackReference::addRangeChar(char from, char to)
    {
        throw ("RegexBackReference::addRangeChar() not implemented");
    }

    // END RegexBackReference

    // Start RegexStartOfLine

    RegexStartOfLine::RegexStartOfLine() : RegexComponentBase(START_OF_LINE) {}

    void    RegexStartOfLine::setStart(const char *start)
    {
        this->component.startOfString = start;
    }

    bool    RegexStartOfLine::match(const char* &ptr, unsigned long long ctx, Functor*fn, const char*) const
    {
        if (ptr == this->component.startOfString || *(ptr - 1) == '\n')
            return fn->run();
        return false;
    }

    void    RegexStartOfLine::addChild(RegexComponentBase *child)
    {
        throw ("RegexStartOfLine::addChild() not implemented");
    }

    void    RegexStartOfLine::addChar(char c)
    {
        throw ("RegexStartOfLine::addChar() not implemented");
    }

    void    RegexStartOfLine::addRangeChar(char from, char to)
    {
        throw ("RegexStartOfLine::addRangeChar() not implemented");
    }

    // END RegexStartOfLine

    // Start RegexEndOfLine

    RegexEndOfLine::RegexEndOfLine() : RegexComponentBase(END_OF_LINE) {}

    bool    RegexEndOfLine::match(const char* &ptr, unsigned long long ctx, Functor*fn, const char*) const
    {
        if (*ptr == '\0' || *ptr == '\n')
            return fn->run();
        return false;
    }

    void    RegexEndOfLine::addChild(RegexComponentBase *child)
    {
        throw ("RegexEndOfLine::addChild() not implemented");
    }

    void    RegexEndOfLine::addChar(char c)
    {
        throw ("RegexEndOfLine::addChar() not implemented");
    }

    void    RegexEndOfLine::addRangeChar(char from, char to)
    {
        throw ("RegexEndOfLine::addRangeChar() not implemented");
    }

    // END RegexEndOfLine

}// namespace ft


// a?abc(p*|ft)+0+ abcftpppppp0