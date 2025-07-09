CXX = c++

SRC = main.cpp
OBJ = $(SRC:.cpp=.o)

NAME = computor

all: $(OBJ)
	$(CXX) $(OBJ) -o $(NAME)
	
clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all