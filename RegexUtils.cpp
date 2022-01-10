#include <RegexUtils.hpp>

namespace ft
{
     Functor::Functor(
         RegexComponentBase 
         const*executable,
         const char* &ptr,
         unsigned int consumed,
         MatchInfo* info,
         Functor *next,
         const char* prev): executable(executable), ptr(ptr),
                            consumed(consumed), info(info), next(next), prev(prev) {}

    

    char    invert_case(char c)
    {
        if (c >= 'A' && c <= 'Z')
            return c + ('a' - 'A');
        else if (c >= 'a' && c <= 'z')
            return c - ('a' - 'A');
        else
            return c;
    }

    bool    Functor::run()
    {
        if (!this->executable)
            return true;
        return this->executable->match(this->ptr, this->consumed, this->info, this->next, this->prev);
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
        case LOOK_BEHIND:
            this->component.range = new RepeatedRange();
            break;
        case LOOK_AHEAD:
            this->component.range = new RepeatedRange();
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
            delete this->component.range;
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
        case LOOK_BEHIND:
            delete this->component.range;
            break;
        case LOOK_AHEAD:
            delete this->component.range;
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

    bool    RegexGroup::match(const char* &ptr, unsigned long long, MatchInfo *info, Functor*fn, const char*) const
    {
        if (ptr != info->endOfStr
            && (this->component.chars->find(*ptr) 
                != this->component.chars->end()
            || (info->flags & RegexComponentBase::iCase
            && this->component.chars->find(ft::invert_case(*ptr)) 
                != this->component.chars->end()))
        )
        {
            ptr++;
            bool    tmp = fn->run();
            ptr--;
            return tmp;
        }
        return (false);
    }

    

    void    RegexGroup::addChild(RegexComponentBase *)
    {
        throw ("RegexGroup::addChild() not implemented");
    }

    RegexGroup::~RegexGroup() {}

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

    bool    RegexInverseGroup::match(const char* &ptr, unsigned long long, MatchInfo *info, Functor*fn, const char*) const
    {
       if (ptr != info->endOfStr
            && (this->component.chars->find(*ptr) 
                == this->component.chars->end()
            || (info->flags & RegexComponentBase::iCase
            && this->component.chars->find(ft::invert_case(*ptr)) 
                == this->component.chars->end()))
        )
        {
            ptr++;
            bool    tmp = fn->run();
            ptr--;
            return tmp;
        }
        return (false);
    }

   

    void    RegexInverseGroup::addChild(RegexComponentBase *)
    {
        throw ("RegexInverseGroup::addChild() not implemented");
    }

    RegexInverseGroup::~RegexInverseGroup() {}
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

    bool    RegexConcat::match(const char* &ptr, unsigned long long ctx, MatchInfo *info, Functor*fn, const char*) const
    {
        if (ctx == this->component.children->size())
            return fn->run();
        Functor newFn(this, ptr, ctx + 1, info, fn);
        return this->component.children->at(ctx)->match(ptr, 0, info, &newFn);
    }

    void    RegexConcat::addChar(char)
    {
        throw ("RegexConcat::addChar() not implemented");
    }

    void    RegexConcat::addRangeChar(char, char)
    {
        throw ("RegexConcat::addRangeChar() not implemented");
    }

    RegexConcat::~RegexConcat() 
    {
        for (size_t i = 0; i < this->component.children->size(); i++)
            delete this->component.children->at(i);
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

    bool   RegexAlternate::match(const char* &ptr, unsigned long long ctx, MatchInfo *info, Functor*fn, const char*) const
    {
        if (ctx == this->component.children->size())
            return false;
        bool    res = this->component.children->at(ctx)->match(ptr, 0, info, fn);
        if (res)
            return res;
        return this->match(ptr, ctx + 1, info, fn);
    }

    void    RegexAlternate::addChar(char)
    {
        throw ("RegexAlternate::addChar() not implemented");
    }

    void    RegexAlternate::addRangeChar(char, char)
    {
        throw ("RegexAlternate::addRangeChar() not implemented");
    }

    RegexAlternate::~RegexAlternate()
    {
        for (size_t i = 0; i < this->component.children->size(); i++)
            delete this->component.children->at(i);
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
        this->component.range->child = child1;
        this->component.range->min = min;
        this->component.range->max = max;
    }

    RegexRepeat::RegexRepeat(RepeatedRange r) : RegexComponentBase(REPEAT)
    {

        this->component.range->child = r.child;
        this->component.range->min = r.min;
        this->component.range->max = r.max;
    }

    bool    RegexRepeat::match(const char* &ptr, unsigned long long ctx, MatchInfo *info, Functor*fn, const char* prev) const
    {
        if (ctx > this->component.range->max)
            return false;

        if (prev == ptr)
            return fn->run();
        
        Functor newFn(this, ptr, ctx + 1, info, fn, ptr);
        bool    res = this->component.range->child->match(ptr, 0, info, &newFn);

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

    void    RegexRepeat::addChar(char)
    {
        throw ("RegexRepeat::addChar() not implemented");
    }

    void    RegexRepeat::addRangeChar(char, char)
    {
        throw ("RegexRepeat::addRangeChar() not implemented");
    }

    RegexRepeat::~RegexRepeat()
    {
        delete this->component.range->child;
    }

    // END RegexRepeat

    // Start RegexRepeatLazy

    RegexRepeatLazy::RegexRepeatLazy() : RegexComponentBase(REPEAT) {
        throw ("RegexRepeatLazy::RegexRepeatLazy() not implemented");
    }

    RegexRepeatLazy::RegexRepeatLazy(
        RegexComponentBase *child1,
        unsigned long long min,
        unsigned long long max) : RegexComponentBase(REPEAT)
    {
        this->component.range->child = child1;
        this->component.range->min = min;
        this->component.range->max = max;
    }

    RegexRepeatLazy::RegexRepeatLazy(RepeatedRange r) : RegexComponentBase(REPEAT)
    {

        this->component.range->child = r.child;
        this->component.range->min = r.min;
        this->component.range->max = r.max;
    }

    bool    RegexRepeatLazy::match(const char* &ptr, unsigned long long ctx, MatchInfo *info, Functor*fn, const char*) const
    {
        if (ctx > this->component.range->max)
            return false;

        bool matched = false;
        if (ctx >= this->component.range->min)
            matched = fn->run();
        if (!matched && ctx < this->component.range->max)
        {
            Functor newFn(this, ptr, ctx + 1, info, fn, ptr);
            return this->component.range->child->match(ptr, 0, info, &newFn);
        }
        return matched;
    }

    void    RegexRepeatLazy::addChild(RegexComponentBase *child)
    {
        this->component.children->push_back(child);
    }

    void    RegexRepeatLazy::addChar(char)
    {
        throw ("RegexRepeatLazy::addChar() not implemented");
    }

    void    RegexRepeatLazy::addRangeChar(char, char)
    {
        throw ("RegexRepeatLazy::addRangeChar() not implemented");
    }

    RegexRepeatLazy::~RegexRepeatLazy()
    {
        delete this->component.range->child;
    }

    // END RegexRepeatLazy

    // Start RegexStartOfGroup

    RegexStartOfGroup::RegexStartOfGroup() : 
        RegexComponentBase(START_OF_GROUP)
    {
        this->component.group->first = NULL;
        this->component.group->second = NULL;
    }
    
    bool    RegexStartOfGroup::match(const char* &ptr, unsigned long long, MatchInfo *, Functor*fn, const char*) const
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

    void    RegexStartOfGroup::addChild(RegexComponentBase *)
    {
        throw ("RegexStartOfGroup::addChild() not implemented");
    }

    void    RegexStartOfGroup::addChar(char)
    {
        throw ("RegexStartOfGroup::addChar() not implemented");
    }

    void    RegexStartOfGroup::addRangeChar(char, char)
    {
        throw ("RegexStartOfGroup::addRangeChar() not implemented");
    }

    RegexStartOfGroup::~RegexStartOfGroup() {}

    // END RegexStartOfGroup

    // Start RegexEndOfGroup
    
    RegexEndOfGroup::RegexEndOfGroup() : RegexComponentBase(END_OF_GROUP) {}

    RegexEndOfGroup::RegexEndOfGroup(RegexStartOfGroup *group) : 
        RegexComponentBase(END_OF_GROUP)
    {
        this->component.groupStart = group;
    }
    bool    RegexEndOfGroup::match(const char* &ptr, unsigned long long, MatchInfo *, Functor*fn, const char*) const
    {
        std::pair<const char *, const char *> tmp = this->component.groupStart->getCapturedGroup();
        this->component.groupStart->capture(ptr);
        bool matched = fn->run();
        /*
        ^<([a-z]+)([^<]*)(?:>(.*)<\/\1>|\s+\/>)$
        <head><meta charset="UTF-8">
        <meta http-equiv="X-UA-Compatible"
        content="IE=edge"><meta name="viewport"
        content="width=device-width, initial-scale=1.0">
        <title>Document</title></head>
        */
        if (!matched)
        {
            this->component.groupStart->component.group->first = tmp.first;
            this->component.groupStart->component.group->second = tmp.second;
        }
        return matched;
    }

    void    RegexEndOfGroup::addChild(RegexComponentBase *)
    {
        throw ("RegexEndOfGroup::addChild() not implemented");
    }

    void    RegexEndOfGroup::addChar(char)
    {
        throw ("RegexEndOfGroup::addChar() not implemented");
    }

    void    RegexEndOfGroup::addRangeChar(char, char)
    {
        throw ("RegexEndOfGroup::addRangeChar() not implemented");
    }

    RegexEndOfGroup::~RegexEndOfGroup() {}

    // END RegexEndOfGroup


    // Start RegexEnd
    RegexEnd::RegexEnd() : RegexComponentBase(END) {}

    bool    RegexEnd::match(const char* &, unsigned long long, MatchInfo *, Functor*, const char*) const
    {
        return true;
    }

    void    RegexEnd::addChild(RegexComponentBase *)
    {
        throw ("RegexEnd::addChild() not implemented");
    }

    void    RegexEnd::addChar(char)
    {
        throw ("RegexEnd::addChar() not implemented");
    }

    void    RegexEnd::addRangeChar(char , char )
    {
        throw ("RegexEnd::addRangeChar() not implemented");
    }

    RegexEnd::~RegexEnd() {}

    // END RegexEnd

    // Start RegexBackReference

    RegexBackReference::RegexBackReference() : RegexComponentBase(BACK_REFERENCE) {}

    RegexBackReference::RegexBackReference(RegexStartOfGroup *group) : 
        RegexComponentBase(BACK_REFERENCE)
    {
        this->component.groupStart = group;
    }

    bool    RegexBackReference::match(const char* &ptr, unsigned long long , MatchInfo *, Functor*fn, const char*) const
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

    void    RegexBackReference::addChild(RegexComponentBase *)
    {
        throw ("RegexBackReference::addChild() not implemented");
    }

    void    RegexBackReference::addChar(char)
    {
        throw ("RegexBackReference::addChar() not implemented");
    }

    void    RegexBackReference::addRangeChar(char, char)
    {
        throw ("RegexBackReference::addRangeChar() not implemented");
    }

    RegexBackReference::~RegexBackReference() {}

    // END RegexBackReference

    // Start RegexStartOfLine

    RegexStartOfLine::RegexStartOfLine() : RegexComponentBase(START_OF_LINE) {}

    void    RegexStartOfLine::setStart(const char *start)
    {
        this->component.startOfString = start;
    }

    bool    RegexStartOfLine::match(const char* &ptr, unsigned long long, MatchInfo *info, Functor*fn, const char*) const
    {
        if (ptr == info->startOfStr || *(ptr - 1) == '\n')
            return fn->run();
        return false;
    }

    void    RegexStartOfLine::addChild(RegexComponentBase *)
    {
        throw ("RegexStartOfLine::addChild() not implemented");
    }

    void    RegexStartOfLine::addChar(char)
    {
        throw ("RegexStartOfLine::addChar() not implemented");
    }

    void    RegexStartOfLine::addRangeChar(char, char)
    {
        throw ("RegexStartOfLine::addRangeChar() not implemented");
    }

    RegexStartOfLine::~RegexStartOfLine() {}

    // END RegexStartOfLine

    // Start RegexEndOfLine

    RegexEndOfLine::RegexEndOfLine() : RegexComponentBase(END_OF_LINE) {}

    bool    RegexEndOfLine::match(const char* &ptr, unsigned long long, MatchInfo *info, Functor*fn, const char*) const
    {
        if (ptr == info->endOfStr || *ptr == '\n')
            return fn->run();
        return false;
    }

    void    RegexEndOfLine::addChild(RegexComponentBase *)
    {
        throw ("RegexEndOfLine::addChild() not implemented");
    }

    void    RegexEndOfLine::addChar(char)
    {
        throw ("RegexEndOfLine::addChar() not implemented");
    }

    void    RegexEndOfLine::addRangeChar(char, char)
    {
        throw ("RegexEndOfLine::addRangeChar() not implemented");
    }

    RegexEndOfLine::~RegexEndOfLine() {}

    // END RegexEndOfLine

    // Start RegexWordBoundary

    RegexWordBoundary::RegexWordBoundary() : RegexComponentBase(WORD_BOUNDARY) {}

    bool    RegexWordBoundary::match(const char* &ptr, unsigned long long, MatchInfo *info, Functor*fn, const char*) const
    {
        if ((ptr == info->startOfStr &&  (isalnum(*ptr) || *ptr == '_'))
            || (ptr == info->endOfStr 
                &&  (isalnum(*(ptr - 1)) || *(ptr - 1) == '_'))
            || (ptr != info->startOfStr && ptr != info->endOfStr 
                && (isalnum(*(ptr - 1)) || *(ptr - 1) == '_') 
                    ^ (isalnum(*ptr) || *ptr == '_')))
            return fn->run();
        return false;
    }

    void    RegexWordBoundary::addChild(RegexComponentBase *)
    {
        throw ("RegexWordBoundary::addChild() not implemented");
    }

    void    RegexWordBoundary::addChar(char)
    {
        throw ("RegexWordBoundary::addChar() not implemented");
    }

    void    RegexWordBoundary::addRangeChar(char, char)
    {
        throw ("RegexWordBoundary::addRangeChar() not implemented");
    }

    RegexWordBoundary::~RegexWordBoundary() {}

    // END RegexWordBoundary

    // Start RegexNonWordBoundary

    RegexNonWordBoundary::RegexNonWordBoundary() : RegexComponentBase(WORD_BOUNDARY) {}

    bool    RegexNonWordBoundary::match(const char* &ptr, unsigned long long, MatchInfo *info, Functor*fn, const char*) const
    {
        if (!((ptr == info->startOfStr &&  (isalnum(*ptr) || *ptr == '_'))
            || (ptr == info->endOfStr 
                &&  (isalnum(*(ptr - 1)) || *(ptr - 1) == '_'))
            || (ptr != info->startOfStr && ptr != info->endOfStr 
                && (isalnum(*(ptr - 1)) || *(ptr - 1) == '_') 
                    ^ (isalnum(*ptr) || *ptr == '_'))))
            return fn->run();
        return false;
    }

    void    RegexNonWordBoundary::addChild(RegexComponentBase *)
    {
        throw ("RegexNonWordBoundary::addChild() not implemented");
    }

    void    RegexNonWordBoundary::addChar(char)
    {
        throw ("RegexNonWordBoundary::addChar() not implemented");
    }

    void    RegexNonWordBoundary::addRangeChar(char, char)
    {
        throw ("RegexNonWordBoundary::addRangeChar() not implemented");
    }

    RegexNonWordBoundary::~RegexNonWordBoundary() {}

    // END RegexNonWordBoundary

    // Start RegexPositiveLookBehind

    RegexPositiveLookBehind::RegexPositiveLookBehind() : 
        RegexComponentBase(LOOK_BEHIND) {}

    bool    RegexPositiveLookBehind::match(const char* &ptr, unsigned long long ctx, MatchInfo *info, Functor*fn, const char*prev) const
    {
        if (this->component.range->min + ctx > this->component.range->max)
            return false;
        
        if (prev != NULL && prev != ptr)
            return false;
        if (prev == ptr)
            return fn->run();
        
        if (info->startOfStr + ctx + this->component.range->min > ptr)
            return false;

        Functor newFn(this, ptr, ctx, info, fn, ptr);
        ptr -= this->component.range->min + ctx;
        if (this->component.range->child->match(ptr, ctx, info, &newFn))
            return true;
        ptr += this->component.range->min + ctx;
        
        return this->match(ptr, ctx + 1, info, fn);
    }

    void    RegexPositiveLookBehind::addChild(RegexComponentBase *)
    {
        throw ("RegexPositiveLookBehind::addChild() not implemented");
    }

    void    RegexPositiveLookBehind::addChar(char)
    {
        throw ("RegexPositiveLookBehind::addChar() not implemented");
    }

    void    RegexPositiveLookBehind::addRangeChar(char, char)
    {
        throw ("RegexPositiveLookBehind::addRangeChar() not implemented");
    }

    RegexPositiveLookBehind::~RegexPositiveLookBehind() 
    {
        delete this->component.range->child;
    }

    // END RegexPositiveLookBehind

    // Start RegexNegativeLookBehind

    RegexNegativeLookBehind::RegexNegativeLookBehind() : 
        RegexComponentBase(LOOK_BEHIND) {}

    bool    RegexNegativeLookBehind::match(const char* &ptr, unsigned long long ctx, MatchInfo *info, Functor*fn, const char*prev) const
    {
        if (prev == ptr)
            return true;
        else if (prev != NULL && prev != ptr)
            return false;

        while (1)
        {
            if (info->startOfStr + ctx + this->component.range->min > ptr)
                return fn->run();
            if (this->component.range->min + ctx > this->component.range->max)
                return fn->run();
            Functor newFn(this, ptr, ctx, info, fn, ptr);
            ptr -= this->component.range->min + ctx;
            if (this->component.range->child->match(ptr, ctx, info, &newFn))
            {
                ptr += this->component.range->min + ctx;
                return false;
            }
            ptr += this->component.range->min + ctx;
            ++ctx;
        }
    }

    void    RegexNegativeLookBehind::addChild(RegexComponentBase *)
    {
        throw ("RegexNegativeLookBehind::addChild() not implemented");
    }

    void    RegexNegativeLookBehind::addChar(char)
    {
        throw ("RegexNegativeLookBehind::addChar() not implemented");
    }

    void    RegexNegativeLookBehind::addRangeChar(char, char)
    {
        throw ("RegexNegativeLookBehind::addRangeChar() not implemented");
    }

    RegexNegativeLookBehind::~RegexNegativeLookBehind() 
    {
        delete this->component.range->child;
    }

    // END RegexNegativeLookBehind

    // Start RegexPositiveLookAhead

    RegexPositiveLookAhead::RegexPositiveLookAhead() : 
        RegexComponentBase(LOOK_AHEAD) {}

    bool    RegexPositiveLookAhead::match(const char* &ptr, unsigned long long ctx, MatchInfo *info, Functor*fn, const char* prev) const
    {
        if (prev != NULL)
        {
            ptr = prev;
            return fn->run();
        }
  

        Functor newFn(this, ptr, ctx, info, fn, ptr);
        return  (this->component.range->child->match(ptr, ctx, info, &newFn));
         
    }

    void    RegexPositiveLookAhead::addChild(RegexComponentBase *)
    {
        throw ("RegexPositiveLookAhead::addChild() not implemented");
    }

    void    RegexPositiveLookAhead::addChar(char)
    {
        throw ("RegexPositiveLookAhead::addChar() not implemented");
    }

    void    RegexPositiveLookAhead::addRangeChar(char, char)
    {
        throw ("RegexPositiveLookAhead::addRangeChar() not implemented");
    }

    RegexPositiveLookAhead::~RegexPositiveLookAhead() 
    {
        delete this->component.range->child;
    }

    // END RegexPositiveLookAhead

    // Start RegexNegativeLookAhead

    RegexNegativeLookAhead::RegexNegativeLookAhead() : 
        RegexComponentBase(LOOK_AHEAD) {}

    bool    RegexNegativeLookAhead::match(const char* &ptr, unsigned long long ctx, MatchInfo *info, Functor*fn, const char* prev) const
    {
        if (prev != NULL)
        {
            return true;
        }
        prev = ptr;
        Functor newFn(this, ptr, ctx, info, fn, ptr);
        if (!(this->component.range->child->match(ptr, ctx, info, &newFn)))
            return fn->run();
        ptr = prev;
        return false;
    }

    void    RegexNegativeLookAhead::addChild(RegexComponentBase *)
    {
        throw ("RegexNegativeLookAhead::addChild() not implemented");
    }

    void    RegexNegativeLookAhead::addChar(char)
    {
        throw ("RegexNegativeLookAhead::addChar() not implemented");
    }

    void    RegexNegativeLookAhead::addRangeChar(char, char)
    {
        throw ("RegexNegativeLookAhead::addRangeChar() not implemented");
    }

    RegexNegativeLookAhead::~RegexNegativeLookAhead() 
    {
        delete this->component.range->child;
    }

    // END RegexNegativeLookAhead

}// namespace ft


// a?abc(p*|ft)+0+ abcftpppppp0