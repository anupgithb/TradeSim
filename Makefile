# Makefile for TradeSimulator (headless, custom Boost & fmt & OpenSSL)

# Compiler
CXX       := g++
CXXFLAGS  := -Wall -std=c++17 \
              -I. \
              -Icore \
              -Inetwork \
              -Imodels \
              -Iutils \
              -IC:/boost/include \
              -IC:/openssl/include

# Library flags
LIBDIRS   := -LC:/boost/lib \
              -LC:/openssl/lib
LDLIBS    := -lboost_system \
              -lws2_32 \
	      -lmswsock \
              -lfmt \
              -lssl \
              -lcrypto

# Source directories
SRCDIRS   := . core network models utils
SRC       := $(wildcard main.cpp $(foreach d,$(SRCDIRS),$(d)/*.cpp))
OBJ       := $(SRC:.cpp=.o)

# Target executable
TARGET    := TradeSim.exe

.PHONY: all clean

all: $(TARGET)

# Link step
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBDIRS) $(LDLIBS)

# Compile step
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

