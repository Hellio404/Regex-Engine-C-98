#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include <exception>

namespace ft
{
    class RegexComponentBase;
    struct RegexStartOfGroup;
    struct RegexEndOfGroup;

    // types of regexComponents
    enum
    {
        GROUP,
        INVERSE_GROUP,
        REPEAT,
        CONCAT,
        ALTERNATE,
        START_OF_LINE,
        END_OF_LINE,
        START_OF_GROUP,
        END_OF_GROUP,
        EPSILON,
    };

    struct RepeatedRange
    {
        RegexComponentBase  *toRepeat;
        unsigned int        consumed;
        unsigned int        min;
        unsigned int        max;
    };




    // This is am union for the RegexComponent
    // It is used by GroupChars, Repeat, Concat, and Alternate
    // each once of the uses one  of its members
    union RegexComponentType
    {
        std::set<char>                          *chars;
        std::vector<RegexComponentBase *>       *children;
        RepeatedRange                           *range;
        std::pair<const char *, const char *>   *group;
        RegexStartOfGroup                       *groupStart;
    };

    class Functor
    {
        private:
            RegexComponentBase  *executable;
            const char*         ptr;
            unsigned int        consumed;
            RegexComponentBase  *next;
        public:
            Functor(RegexComponentBase *executable, const char* ptr, unsigned int consumed, RegexComponentBase *next);
            const char *operator()();
    };



    // This is the base class for all regex components
    class RegexComponentBase
    {
    public: // TODO: make them protected if possible
        const static    unsigned int    Infinity = ~0;
        int type;
        RegexComponentType component;

        virtual void    addChar(char) = 0;
        virtual void    addRangeChar(char, char) = 0;
        virtual void    addChild(RegexComponentBase *) = 0;
        virtual const char *match(const char *ptr) const = 0;
        virtual RegexComponentBase* clone(std::map<RegexComponentBase *, 
            RegexComponentBase *> &) = 0;
    protected:
        RegexComponentBase(int type);
        virtual ~RegexComponentBase();
    };

    







    struct RegexGroup : public RegexComponentBase
    {
        RegexGroup();
        RegexGroup(char c);
        RegexGroup(char from, char to);

        void    addChar(char c);
        void    addRangeChar(char from, char to);
        
        RegexComponentBase*     clone(std::map<RegexComponentBase *, 
            RegexComponentBase *> &);
        const char *match(const char *ptr) const;
        private:
            void    addChild(RegexComponentBase *);

    };




    struct RegexInverseGroup : public RegexComponentBase
    {
        RegexInverseGroup();
        RegexInverseGroup(char c);
        RegexInverseGroup(char from, char to);

        void    addChar(char c);
        void    addRangeChar(char from, char to);
        
        RegexComponentBase*     clone(std::map<RegexComponentBase *, 
            RegexComponentBase *> &);
        const char *match(const char *ptr) const;
        private:
            void    addChild(RegexComponentBase *);
    };






    struct RegexConcat : public RegexComponentBase
    {
        RegexConcat();
        RegexConcat(RegexComponentBase *);

        void    addChild(RegexComponentBase *child);

        const char *match(const char *ptr) const;
        
        RegexComponentBase*     clone(std::map<RegexComponentBase *, 
            RegexComponentBase *> &);

        private:
            void    addChar(char);
            void    addRangeChar(char, char);
    };




    struct RegexAlternate : public RegexComponentBase
    {
        RegexAlternate();
        RegexAlternate(RegexComponentBase *);

        void    addChild(RegexComponentBase *child);

        const char *match(const char *ptr) const;
        
        RegexComponentBase*     clone(std::map<RegexComponentBase *, 
            RegexComponentBase *> &);
        private:
            void    addChar(char);
            void    addRangeChar(char, char);
    };





    struct RegexRepeat : public RegexComponentBase
    {
        //               Repeated              Next
        RegexRepeat(RegexComponentBase *, RegexComponentBase *);

        const char *match(const char *ptr) const;
        
        RegexComponentBase*     clone(std::map<RegexComponentBase *, 
            RegexComponentBase *> &);

        private:
            RegexRepeat();
            void    addChild(RegexComponentBase *child);
            void    addChar(char);
            void    addRangeChar(char, char);
    };


    struct RegexStartOfGroup : public RegexComponentBase
    {
        RegexStartOfGroup();

        const char *match(const char *ptr) const;
        void        capture(const char *end);
        std::pair <const char *, const char *> getCapturedGroup() const;

        
        RegexComponentBase*     clone(std::map<RegexComponentBase *, 
            RegexComponentBase *> &);
        private:
            void    addChild(RegexComponentBase *child);
            void    addChar(char);
            void    addRangeChar(char, char);
    };

    struct RegexEndOfGroup : public RegexComponentBase
    {
        RegexEndOfGroup(RegexStartOfGroup *);

        const char *match(const char *ptr) const;

        
        RegexComponentBase*     clone(std::map<RegexComponentBase *, 
            RegexComponentBase *> &);

        private:
            RegexEndOfGroup();
            void    addChild(RegexComponentBase *child);
            void    addChar(char);
            void    addRangeChar(char, char);
    };

    // EpsilonRegex

    struct RegexEpsilon : public RegexComponentBase
    {
        RegexEpsilon();

        const char *match(const char *ptr) const;

        
        RegexComponentBase*     clone(std::map<RegexComponentBase *, 
            RegexComponentBase *> &);
        private:
            void    addChild(RegexComponentBase *child);
            void    addChar(char);
            void    addRangeChar(char, char);
    };


} // namespace ft
