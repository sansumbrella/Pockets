# For this to all work from the command-line, we will also need
# to build and link pockets, puptent, and cobweb here, acquiring
# the Cinder and boost dependencies from up a few levels

ALL_TESTS:=$(foreach file, $(wildcard *.cpp), $(file:.cpp=.tests) )

test: build run

%.tests: %.cpp
	clang++ $< -o $@

build:$(ALL_TESTS)

# run tests and continue when there are errors
run:
	-./PocketsTests.tests
	-./CobWebTests.tests
	-./PupTentTests.tests

clean:
	rm *.tests

.PHONY: test
.PHONY: run
.PHONY: clean
