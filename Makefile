NAME = mirror

FLAGS = -Wall -Wextra -Werror

FILES = main.cpp

LIBS = -lboost_system

all: $(NAME)

$(NAME):
	g++ -g -o $(NAME) $(FILES) $(FLAGS) $(LIBS)

clean:
	rm -f *.o

fclean: clean
	rm -f $(NAME)

re: fclean all
