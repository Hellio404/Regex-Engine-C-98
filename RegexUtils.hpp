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
   

    struct RepeatedRange
    {
        RegexComponentBase  *toRepeat;
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


    class Functor;
    
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
        virtual bool    match(const char *&, unsigned long long, Functor*) const = 0;
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
            END,
        };
    protected:
        RegexComponentBase(int type);
        virtual ~RegexComponentBase();
    };

    

    class Functor
    {
        private:
            const RegexComponentBase    *executable;
            const char*                 &ptr;
            unsigned int                consumed;
            Functor                     *next;
        public:
            Functor(RegexComponentBase const *executable, const char* &ptr, unsigned int consumed, Functor *next);
            bool    operator()();
    };

   





    struct RegexGroup : public RegexComponentBase
    {
        RegexGroup();
        RegexGroup(char c);
        RegexGroup(char from, char to);

        void    addChar(char c);
        void    addRangeChar(char from, char to);
        bool    match(const char *&, unsigned long long, Functor*) const;
        
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
        bool    match(const char *&, unsigned long long, Functor*) const;
        private:
            void    addChild(RegexComponentBase *);
    };






    struct RegexConcat : public RegexComponentBase
    {
        RegexConcat();
        RegexConcat(RegexComponentBase *);

        void    addChild(RegexComponentBase *child);

        bool    match(const char *&, unsigned long long, Functor*) const;

        private:
            void    addChar(char);
            void    addRangeChar(char, char);
    };




    struct RegexAlternate : public RegexComponentBase
    {
        RegexAlternate();
        RegexAlternate(RegexComponentBase *);

        void    addChild(RegexComponentBase *child);

        bool    match(const char *&, unsigned long long, Functor*) const;
        private:
            void    addChar(char);
            void    addRangeChar(char, char);
    };





    struct RegexRepeat : public RegexComponentBase
    {
        RegexRepeat(RepeatedRange);
        RegexRepeat(RegexComponentBase *child1, unsigned long long min,
            unsigned long long max);
        bool    match(const char *&, unsigned long long, Functor*) const;

        private:
            RegexRepeat();
            void    addChild(RegexComponentBase *child);
            void    addChar(char);
            void    addRangeChar(char, char);
    };


    struct RegexStartOfGroup : public RegexComponentBase
    {
        RegexStartOfGroup();

        bool    match(const char *&, unsigned long long, Functor*) const;
        void    capture(const char *end);
    
        std::pair <const char *, const char *> getCapturedGroup() const;

        private:
            void    addChild(RegexComponentBase *child);
            void    addChar(char);
            void    addRangeChar(char, char);
    };

    struct RegexEndOfGroup : public RegexComponentBase
    {
        RegexEndOfGroup(RegexStartOfGroup *);

        bool    match(const char *&, unsigned long long, Functor*) const;

        private:
            RegexEndOfGroup();
            void    addChild(RegexComponentBase *child);
            void    addChar(char);
            void    addRangeChar(char, char);
    };

    struct RegexEnd : public RegexComponentBase
    {
        RegexEnd();

        bool    match(const char *&, unsigned long long, Functor*) const;

        private:
            void    addChild(RegexComponentBase *child);
            void    addChar(char);
            void    addRangeChar(char, char);
    };

    

} // namespace ft
