CXX=g++
CXXFLAGS=-std=c++23 -Wall -Wextra -Wpedantic -Wno-interference-size
LDFLAGS=
# release, asan, msan, tsan
BUILD_TYPE?=release

NAME=result.out

SRCDIR=./src
BINDIR=./bin

TARGET=$(BINDIR)/$(NAME)

SRCS=$(wildcard $(SRCDIR)/*.cpp)
OBJS=$(patsubst $(SRCDIR)/%.cpp,$(BINDIR)/%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)

ifeq ($(BUILD_TYPE),release)
	CXXFLAGS += -O3
	LDFLAGS += 
else ifeq ($(BUILD_TYPE),asan)
	CXXFLAGS += -O1 -g -fsanitize=address -fno-omit-frame-pointer
	LDFLAGS += -fsanitize=address
else ifeq ($(BUILD_TYPE),msan)
	CXXFLAGS += -O1 -g  -fsanitize=memory -fno-omit-frame-pointer
	LDFLAGS += -fsanitize=memory
else ifeq ($(BUILD_TYPE),tsan)
	CXXFLAGS += -O1 -g -fsanitize=thread
	LDFLAGS += -fsanitize=thread
endif



all: $(BINDIR) $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS)  $(CXXFLAGS) -o $(BINDIR)/$(NAME)

$(BINDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(LDFLAGS)  $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BINDIR):
	-mkdir -p $(BINDIR)


clean:
	rm -rf $(BINDIR)
.PHONY: clean


-include $(DEPS)