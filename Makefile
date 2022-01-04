NAME    = regex
CC      = g++-11
FLAGS   = -Wall -Wextra -Werror -Wno-unused-variable -Wno-unused-parameter -g
HEADS   = Regex.hpp 
SRCS = Regex.cpp RegexUtils.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)
$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(FLAGS_DEBUG) -I. -o $(NAME) $(OBJS)

%.o: %.cpp $(HEADS)
	$(CC) $(FLAGS) $(FLAGS_DEBUG) -I. -c -o $@ $<
clean:
	rm -rf $(OBJS)
fclean: clean
	rm -rf $(NAME)
re: fclean all

