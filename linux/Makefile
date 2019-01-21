##
## EPITECH PROJECT, 2018
## Makefile
## File description:
## juniqu_v
##

NAME		= zia

TEST 		= unitTest

PACKAGE		= package

RM		= rm -f

CXX		= g++

CXXFLAGS	+= -g3 -Wall -Wextra -Werror -Wfatal-errors -MD -std=c++17 -Iincludes/ -Iincludes/server -Iincludes/server/api -Iincludes/server/error -I./includes/server/NetStack -Iincludes/modules -g3

LDFLAGS		+= -ldl -lssl -lcrypto

SRCS		= $(foreach dir,$(shell find sources/server -type d),$(wildcard $(dir)/*.cpp)) \
					sources/main.cpp

SRCS_TEST =  $(foreach dir,$(shell find sources/server -type d),$(wildcard $(dir)/*.cpp)) \
						 sources/modules/SSLModule.cpp \
					   sources/unitTest/unitTest.cpp

OBJS_TEST = $(SRCS_TEST:.cpp=.o)
OBJS		= $(SRCS:.cpp=.o)
DEPS		= $(SRCS:.cpp=.d)

all: $(NAME)

$(NAME): $(OBJS)
	@echo "Building $(NAME)..."
	@$(CXX) -o $(NAME) $(OBJS) $(CXXFLAGS) $(LDFLAGS)
	@make -C sources/modules
	@cp $(NAME) sources/modules/*.so $(PACKAGE)
	@make -C $(PACKAGE)

clean:
	@echo "Cleaning temporary files..."
	@$(RM) $(OBJS)
	@$(RM) $(DEPS)
	@$(RM) $(OBJS_TEST)
	cd sources/modules && make clean

fclean: clean
	@echo "Cleaning executable..."
	@$(RM) $(TEST)
	@$(RM) $(NAME) $(PACKAGE)/$(NAME) $(PACKAGE)/*.so
	@make -C $(PACKAGE) clean	
	@make -C sources/modules fclean

re: fclean all

test: $(OBJS_TEST)
	@$(CXX) -o $(TEST) $(OBJS_TEST) $(CXXFLAGS) $(LDFLAGS)

%.o: %.cpp
	@echo "=>" $<
	@$(CXX) -c $< -o $@ $(CXXFLAGS)

-include $(DEPS)

.PHONY:	all clean fclean re
