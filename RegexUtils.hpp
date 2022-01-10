#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <exception>
namespace ft
{
    class Regex;

    class RegexComponentBase;
    struct RegexStartOfGroup;
    struct RegexEndOfGroup;

    // types of regexComponents
   struct MatchInfo
   {
        const char          *startOfStr;
        const char          *endOfStr;
        unsigned long long  flags;
   };

    struct RepeatedRange
    {
        RegexComponentBase  *child;
        unsigned long long  min;
        unsigned long long  max;
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
        const char *                            startOfString;
    };

    
    class Functor;
    
    // This is the base class for all regex components
    class RegexComponentBase
    {
    public: // TODO: make them protected if possible
        const static unsigned int   Infinity = ~0;
        int                         type;
        RegexComponentType          component;
        

        virtual void    addChar(char) = 0;
        virtual void    addRangeChar(char, char) = 0;
        virtual void    addChild(RegexComponentBase *) = 0;
        virtual bool    match(const char *&, unsigned long long, MatchInfo *, Functor*, const char* = NULL) const = 0;
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
            BACK_REFERENCE,
            WORD_BOUNDARY,
            LOOK_BEHIND,
            LOOK_AHEAD,
        };

        enum 
        {
            iCase = 4,
        };
        virtual ~RegexComponentBase();
    protected:
        RegexComponentBase(int type);
    };

    

    class Functor
    {
        private:
            const RegexComponentBase    *executable;
            const char*                 &ptr;
            unsigned int                consumed;
            MatchInfo                   *info; 
            Functor                     *next;
            const char*                 prev;

        public:
            Functor(RegexComponentBase const *executable, const char* &ptr, unsigned int consumed, MatchInfo* info, Functor *next, const char *prev = NULL);
            bool    run();
    };

   





    struct RegexGroup : public RegexComponentBase
    {
        RegexGroup();
        RegexGroup(char c);
        RegexGroup(char from, char to);

        void    addChar(char c);
        void    addRangeChar(char from, char to);
        bool    match(const char *&, unsigned long long, MatchInfo *, Functor*, const char* = NULL) const;
        ~RegexGroup();
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
        bool    match(const char *&, unsigned long long, MatchInfo *, Functor*, const char* = NULL) const;
        ~RegexInverseGroup();
        private:
            void    addChild(RegexComponentBase *);
    };






    struct RegexConcat : public RegexComponentBase
    {
        RegexConcat();
        RegexConcat(RegexComponentBase *);

        void    addChild(RegexComponentBase *child);

        bool    match(const char *&, unsigned long long, MatchInfo *, Functor*, const char* = NULL) const;
        ~RegexConcat();
        private:
            void    addChar(char);
            void    addRangeChar(char, char);
    };




    struct RegexAlternate : public RegexComponentBase
    {
        RegexAlternate();
        RegexAlternate(RegexComponentBase *);

        void    addChild(RegexComponentBase *child);
        ~RegexAlternate();
        bool    match(const char *&, unsigned long long, MatchInfo *, Functor*, const char* = NULL) const;
        private:
            void    addChar(char);
            void    addRangeChar(char, char);
    };





    struct RegexRepeat : public RegexComponentBase
    {
        RegexRepeat(RepeatedRange);
        RegexRepeat(RegexComponentBase *child1, unsigned long long min,
            unsigned long long max);
        bool    match(const char *&, unsigned long long, MatchInfo *, Functor*, const char* = NULL) const;
        ~RegexRepeat();
        private:
            RegexRepeat();
            void    addChild(RegexComponentBase *child);
            void    addChar(char);
            void    addRangeChar(char, char);
    };

    struct RegexRepeatLazy : public RegexComponentBase
    {
        RegexRepeatLazy(RepeatedRange);
        RegexRepeatLazy(RegexComponentBase *child1, unsigned long long min,
            unsigned long long max);
        bool    match(const char *&, unsigned long long, MatchInfo *, Functor*, const char* = NULL) const;
        ~RegexRepeatLazy();
        private:
            RegexRepeatLazy();
            void    addChild(RegexComponentBase *child);
            void    addChar(char);
            void    addRangeChar(char, char);
    };


    struct RegexStartOfGroup : public RegexComponentBase
    {
        RegexStartOfGroup();

        bool    match(const char *&, unsigned long long, MatchInfo *, Functor*, const char* = NULL) const;
        void    capture(const char *end);
    
        std::pair <const char *, const char *> getCapturedGroup() const;
        ~RegexStartOfGroup();
        private:
            void    addChild(RegexComponentBase *child);
            void    addChar(char);
            void    addRangeChar(char, char);
    };

    struct RegexEndOfGroup : public RegexComponentBase
    {
        RegexEndOfGroup(RegexStartOfGroup *);

        bool    match(const char *&, unsigned long long, MatchInfo *, Functor*, const char* = NULL) const;
        ~RegexEndOfGroup();
        private:
            RegexEndOfGroup();
            void    addChild(RegexComponentBase *child);
            void    addChar(char);
            void    addRangeChar(char, char);
    };

    struct RegexEnd : public RegexComponentBase
    {
        RegexEnd();

        bool    match(const char *&, unsigned long long, MatchInfo *, Functor*, const char* = NULL) const;
        ~RegexEnd();
        private:
            void    addChild(RegexComponentBase *child);
            void    addChar(char);
            void    addRangeChar(char, char);
    };


    struct RegexBackReference : public RegexComponentBase
    {
        RegexBackReference(RegexStartOfGroup *);

        bool    match(const char *&, unsigned long long, MatchInfo *, Functor*, const char* = NULL) const;
        ~RegexBackReference();
        private:
            RegexBackReference();
            void    addChild(RegexComponentBase *child);
            void    addChar(char);
            void    addRangeChar(char, char);
    };

    struct RegexStartOfLine : public RegexComponentBase
    {
        RegexStartOfLine();

        bool    match(const char *&, unsigned long long, MatchInfo *, Functor*, const char* = NULL) const;
        void    setStart(const char *start);
        ~RegexStartOfLine();
        private:
            void    addChild(RegexComponentBase *child);
            void    addChar(char);
            void    addRangeChar(char, char);
    };

    struct RegexEndOfLine : public RegexComponentBase
    {
        RegexEndOfLine();

        bool    match(const char *&, unsigned long long, MatchInfo *, Functor*, const char* = NULL) const;
        ~RegexEndOfLine();
        private:
            void    addChild(RegexComponentBase *child);
            void    addChar(char);
            void    addRangeChar(char, char);
    };

    struct RegexWordBoundary : public RegexComponentBase
    {
        RegexWordBoundary();

        bool    match(const char *&, unsigned long long, MatchInfo *, Functor*, const char* = NULL) const;
        ~RegexWordBoundary();
        private:
            void    addChild(RegexComponentBase *child);
            void    addChar(char);
            void    addRangeChar(char, char);
    };

    struct RegexNonWordBoundary : public RegexComponentBase
    {
        RegexNonWordBoundary();
        ~RegexNonWordBoundary();
        bool    match(const char *&, unsigned long long, MatchInfo *, Functor*, const char* = NULL) const;

        private:
            void    addChild(RegexComponentBase *child);
            void    addChar(char);
            void    addRangeChar(char, char);
    };

    struct RegexPositiveLookBehind : public RegexComponentBase
    {
        RegexPositiveLookBehind();
        ~RegexPositiveLookBehind();
        bool    match(const char *&, unsigned long long, MatchInfo *, Functor*, const char* = NULL) const;

        private:
            void    addChild(RegexComponentBase *child);
            void    addChar(char);
            void    addRangeChar(char, char);
    };

    struct RegexNegativeLookBehind : public RegexComponentBase
    {
        RegexNegativeLookBehind();
        ~RegexNegativeLookBehind();
        bool    match(const char *&, unsigned long long, MatchInfo *, Functor*, const char* = NULL) const;

        private:
            void    addChild(RegexComponentBase *child);
            void    addChar(char);
            void    addRangeChar(char, char);
    };

    struct RegexPositiveLookAhead : public RegexComponentBase
    {
        RegexPositiveLookAhead();
        ~RegexPositiveLookAhead();
        bool    match(const char *&, unsigned long long, MatchInfo *, Functor*, const char* = NULL) const;

        private:
            void    addChild(RegexComponentBase *child);
            void    addChar(char);
            void    addRangeChar(char, char);
    };

    struct RegexNegativeLookAhead : public RegexComponentBase
    {
        RegexNegativeLookAhead();
        ~RegexNegativeLookAhead();
        bool    match(const char *&, unsigned long long, MatchInfo *, Functor*, const char* = NULL) const;

        private:
            void    addChild(RegexComponentBase *child);
            void    addChar(char);
            void    addRangeChar(char, char);
    };

} // namespace ft
