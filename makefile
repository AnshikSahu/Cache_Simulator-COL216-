all: simulator.hpp Cache.cpp
	g++ Cache.cpp -o cache_simulate
clean:
	rm -f cache_simulate
compile:
	g++ Cache.cpp -o cache_simulate
run: cache_simulate
	./cache_simulate $(ARGS)
cache_simulate: simulator.hpp Cache.cpp
	g++ Cache.cpp -o cache_simulate