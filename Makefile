NAME    = regex
LIBNAME = lib$(NAME).a
CC      = clang++
FLAGS   = -Wall -Wextra -Werror  -std=c++98 
SRCS = Regex.cpp RegexUtils.cpp
SRCS_TEST = tests/main.cpp
OBJS = $(SRCS:.cpp=.o)
OBJS_TEST = $(SRCS_TEST:.cpp=.o)

all: $(LIBNAME)

$(LIBNAME): $(OBJS)
	ar -rs $(LIBNAME) $(OBJS)

test: $(OBJS) $(OBJS_TEST)
	$(CC) $(FLAGS) $(FLAGS_DEBUG) -I. -Iincludes -o $(NAME) $(OBJS_TEST) $(OBJS) 

%.o: %.cpp
	$(CC) $(FLAGS) $(FLAGS_DEBUG) -I. -Iincludes -c -o $@ $<
clean:
	rm -rf $(OBJS) $(OBJS_TEST)
fclean: clean
	rm -rf $(NAME) $(LIBNAME)
re: fclean all

