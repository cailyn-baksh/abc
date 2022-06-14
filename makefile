NAME = abc
VERSION = v0.1.0
SRCS = $(filter-out %.swp,$(wildcard src/*))
OBJS = $(addsuffix .o,$(patsubst src/%,bin/%,$(SRCS)))
INCLUDES = include/
LIBS = boost_program_options

CFLAGS = -std=gnu17 -03 -Wall $(addprefix -I,$(INCLUDES)) -DNAME=\"$(NAME)\" -DVERSION=\"$(VERSION)\"
CXXFLAGS = -std=gnu++20 -O3 -Wall $(addprefix -I,$(INCLUDES)) -DNAME=\"$(NAME)\" -DVERSION=\"$(VERSION)\"

build: $(OBJS)
	g++ -Wl,-rpath='$$ORIGIN' -o bin/$(NAME) $^ $(addprefix -l,$(LIBS))

bin/%.c.o: src/%.c | bin
	gcc $(CFLAGS) -c -o $@ $^

bin/%.cpp.o: src/%.cpp | bin
	g++ $(CXXFLAGS) -c -o $@ $^

bin:
	mkdir -p $@

clean:
	find bin/* \! \( -iname "*.so.*" -o -iname "*.so" \) -type f -delete

.PHONY: build clean FORCE
FORCE:
