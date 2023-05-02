SHELL = /bin/sh
NAME = resize

# Compile using g++ / opencv4
CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -O2 -g3 -MMD
LDFLAGS = `pkg-config --libs opencv4` -lboost_program_options
INCLUDES = -Iincludes/ -I/usr/include/opencv4

OBJS_DIR = objs
DEPS_DIR = deps
SRCS_DIR = srcs

SRCS = main.cpp \
		interpret_options.cpp

OBJS = $(addprefix $(OBJS_DIR)/, $(SRCS:.cpp=.o))
DEPS = $(addprefix $(DEPS_DIR)/, $(SRCS:.cpp=.d))

# SRCS := $(addprefix $(SRCS_DIR)/, $(SRCS))

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS) $(INCLUDES)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp Makefile
	@mkdir -p $(OBJS_DIR)
	@mkdir -p $(DEPS_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ -MMD -MF $(DEPS_DIR)/$*.d $(INCLUDES)

clean:
	rm -rf $(OBJS_DIR) $(DEPS_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

arg-test: $(NAME)
	python3 tests/test_arguments.py

-include $(DEPS)

.PHONY: all clean fclean re arg-test