CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99

slopfuck: slopfuck.c keywords.h praise.h bookends.h multipliers.h
	$(CC) $(CFLAGS) -o $@ slopfuck.c

docker:
	docker build -t slopfuck .

test: docker
	python3 tests/run_tests.py

clean:
	rm -f slopfuck

.PHONY: clean docker test
