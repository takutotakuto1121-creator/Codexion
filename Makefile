NAME	= codexion
CC 		= cc
CFLAGS 	= -Wall -Wextra -Werror -pthread -I includes
SRCS 	= src/coder.c\
		  src/main.c\
		  src/monitor.c\
		  src/parse.c\
		  utils/utils1.c
OBJS 	= $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re


