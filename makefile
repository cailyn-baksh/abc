NAME = abc
SRCS = $(filter-out %.swp,$(wildcard src/*))
OBJS = $(addsuffix .o,$(patsubst src/%,bin/%,$(SRCS)))
INCLUDES = include/
LIBS = boost_program_options

CFLAGS = -std=gnu17 -03 -Wall $(addprefix -I,$(INCLUDES))
CXXFLAGS = -std=gnu++20 -O3 -Wall $(addprefix -I,$(INCLUDES))

build: $(OBJS)
	g++ -Wl,-rpath='$$ORIGIN' -o bin/$(NAME) $^ $(addprefix -l,$(LIBS))

bin/%.c.o: src/%.c
	gcc $(CFLAGS) -c -o $@ $^

bin/%.cpp.o: src/%.cpp
	g++ $(CXXFLAGS) -c -o $@ $^

clean:
	find bin/* \! \( -iname "*.so.*" -o -iname "*.so" \) -type f -delete

.PHONY: build clean FORCE
FORCE:

