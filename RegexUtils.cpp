#include <RegexUtils.hpp>

namespace ft
{

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
            this->component.children = new std::vector<RegexComponentBase *>();
            this->component.children->reserve(2);
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

    const char *RegexGroup::match(const char *ptr) const
    {
        if (this->component.chars->find(*ptr) != this->component.chars->end())
            return ptr + 1;
        return (NULL);
    }

    RegexComponentBase* RegexGroup::clone(std::map<RegexComponentBase *,
        RegexComponentBase *> &cloned)
    {
        if (cloned.find(this) != cloned.end())
            return (cloned[this]);
        RegexGroup *newGroup = new RegexGroup();
        cloned[this] = newGroup;
        newGroup->component.chars->insert(this->component.chars->begin(),
            this->component.chars->end());
        return (newGroup);
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

    const char *RegexInverseGroup::match(const char *ptr) const
    {
        if (this->component.chars->find(*ptr) == this->component.chars->end())
            return ptr + 1;
        return (NULL);
    }

    RegexComponentBase *RegexInverseGroup::clone(std::map<
        RegexComponentBase *, RegexComponentBase *> &cloned)
    {
        if (cloned.find(this) != cloned.end())
            return (cloned[this]);
        RegexInverseGroup *newGroup = new RegexInverseGroup();
        cloned[this] = newGroup;
        newGroup->component.chars->insert(this->component.chars->begin(),
            this->component.chars->end());
        return (newGroup);
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

    const char *RegexConcat::match(const char *ptr) const
    {
        std::vector<RegexComponentBase *>::const_iterator it;
        std::vector<RegexComponentBase *>::const_iterator end;

        it = this->component.children->begin();
        end = this->component.children->end();
        for (; it != end; it++)
        {
            ptr = (*it)->match(ptr);
            if (ptr == NULL)
                return (NULL);
        }
        return (ptr);
    }

    RegexComponentBase *RegexConcat::clone(std::map<RegexComponentBase *,
        RegexComponentBase *> &cloned)
    {
        if (cloned.find(this) != cloned.end())
            return (cloned[this]);
        RegexConcat *newConcat = new RegexConcat();
        cloned[this] = newConcat;
        std::vector<RegexComponentBase *>::const_iterator it;
        std::vector<RegexComponentBase *>::const_iterator end;

        it = this->component.children->begin();
        end = this->component.children->end();
        for (; it != end; it++)
        {
            newConcat->addChild((*it)->clone(cloned));
        }
        return (newConcat);
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

    const char *RegexAlternate::match(const char *ptr) const
    {
        std::vector<RegexComponentBase *>::const_iterator it;
        std::vector<RegexComponentBase *>::const_iterator end;
        const char *tmp;
        it = this->component.children->begin();
        end = this->component.children->end();
        for (; it != end; it++)
        {
            tmp = (*it)->match(ptr);
            if (tmp != NULL)
                return (tmp);
            
        }
        return (NULL);
    }

    RegexComponentBase *RegexAlternate::clone(std::map<
        RegexComponentBase *, RegexComponentBase *> &cloned)
    {
        if (cloned.find(this) != cloned.end())
            return (cloned[this]);
        RegexAlternate *newAlternate = new RegexAlternate();
        cloned[this] = newAlternate;
        std::vector<RegexComponentBase *>::const_iterator it;
        std::vector<RegexComponentBase *>::const_iterator end;

        it = this->component.children->begin();
        end = this->component.children->end();
        for (; it != end; it++)
        {
            newAlternate->addChild((*it)->clone(cloned));
        }
        return (newAlternate);
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

    RegexRepeat::RegexRepeat() : RegexComponentBase(REPEAT) {}

    RegexRepeat::RegexRepeat( RegexComponentBase *toRepeat,
        RegexComponentBase *next) : RegexComponentBase(REPEAT)
    {
        this->component.children->push_back(toRepeat);
        this->component.children->push_back(next);
    }

    const char *RegexRepeat::match(const char *ptr) const
    {
        const char *tmp = this->component.children->at(0)->match(ptr);
        if (tmp == NULL)
            return ptr;
        const char *tmp2 = this->match(tmp);
        if (tmp2 != tmp)
            return tmp2;
        // this so we can  capture the groups again as the last recursive call
        // did miss them up
        tmp = this->component.children->at(0)->match(ptr);
    
        tmp2 = this->component.children->at(1)->match(tmp);
        if (tmp2 == NULL)
            return ptr;
        return (tmp2);
    }

    RegexComponentBase *RegexRepeat::clone(std::map<
        RegexComponentBase *, RegexComponentBase *> &cloned)
    {
        if (cloned.find(this) != cloned.end())
            return (cloned[this]);
        RegexRepeat *newRepeat = new RegexRepeat();
        cloned[this] = newRepeat;

        std::vector<RegexComponentBase *>::const_iterator it;
        std::vector<RegexComponentBase *>::const_iterator end;

        it = this->component.children->begin();
        end = this->component.children->end();
        for (; it != end; it++)
        {
            newRepeat->addChild((*it)->clone(cloned));
        }
        return (newRepeat);
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
    
    const char *RegexStartOfGroup::match(const char *ptr) const
    {
        this->component.group->first = ptr;
        this->component.group->second = ptr;
        return ptr;
    }

    void    RegexStartOfGroup::capture(const char *end)
    {
        this->component.group->second = end;
    }

    std::pair<const char *, const char *>
    RegexStartOfGroup::getCapturedGroup() const
    {
        return *this->component.group;
    }

    RegexComponentBase *RegexStartOfGroup::clone(std::map<
        RegexComponentBase *, RegexComponentBase *> &cloned)
    {
        if (cloned.find(this) != cloned.end())
            return (cloned[this]);
        RegexStartOfGroup *newGroup = new RegexStartOfGroup();
        cloned[this] = newGroup;
        return (newGroup);
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

    const char *RegexEndOfGroup::match(const char *ptr) const
    {
        this->component.groupStart->capture(ptr);
        return ptr;
    }

    RegexComponentBase *RegexEndOfGroup::clone(std::map<
        RegexComponentBase *, RegexComponentBase *> &cloned)
    {
        if (cloned.find(this) != cloned.end())
            return (cloned[this]);
        RegexEndOfGroup *newGroup = new RegexEndOfGroup();
        cloned[this] = newGroup;
        newGroup->component.groupStart = 
            dynamic_cast<RegexStartOfGroup *>(this->component.groupStart->clone(cloned));
        return (newGroup);
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

    // Start RegexEpsilon

    RegexEpsilon::RegexEpsilon() : RegexComponentBase(EPSILON) {}

    const char *RegexEpsilon::match(const char *ptr) const
    {
        return ptr;
    }

    RegexComponentBase *RegexEpsilon::clone(std::map<
        RegexComponentBase *, RegexComponentBase *> &cloned)
    {
        if (cloned.find(this) != cloned.end())
            return (cloned[this]);
        RegexEpsilon *newEpsilon = new RegexEpsilon();
        cloned[this] = newEpsilon;
        return (newEpsilon);
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

}// namespace ft