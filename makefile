all: simulator.hpp Cache.cpp Cache_inclusive.cpp simulator_inclusive.hpp
	g++ Cache.cpp -o cache_simulate
	g++ Cache_inclusive.cpp -o cache_simulate_inclusive
clean:
	rm -f cache_simulate
	rm -f cache_simulate_inclusive
compile: simulator.hpp Cache.cpp Cache_inclusive.cpp simulator_inclusive.hpp
	g++ Cache.cpp -o cache_simulate
	g++ Cache_inclusive.cpp -o cache_simulate_inclusive
run: cache_simulate
	./cache_simulate $(ARGS)
run_inclusive: cache_simulate_inclusive
	./cache_simulate_inclusive $(ARGS)
cache_simulate: simulator.hpp Cache.cpp
	g++ Cache.cpp -o cache_simulate
cache_simulate_inclusive: simulator_inclusive.hpp Cache_inclusive.cpp
	g++ Cache_inclusive.cpp -o cache_simulate_inclusive
run_all: cache_simulate varriations.sh
	./varriations.sh
