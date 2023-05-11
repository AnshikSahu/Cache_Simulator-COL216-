all: simulator.hpp Cache.cpp Cache_inclusive.cpp simulator_inclusive.hpp
	g++ Cache.cpp -o cache_simulate_non_inclusive
	g++ Cache_inclusive.cpp -o cache_simulate
clean:
	rm -f cache_simulate_non_inclusive
	rm -f cache_simulate
compile: simulator.hpp Cache.cpp Cache_inclusive.cpp simulator_inclusive.hpp
	g++ Cache.cpp -o cache_simulate_non_inclusive
	g++ Cache_inclusive.cpp -o cache_simulate
run_non_inclusive: cache_simulate_non_inclusive
	./cache_simulate_non_inclusive $(ARGS)
run_inclusive: cache_simulate_inclusive
	./cache_simulate $(ARGS)
cache_simulate_non_inclusive: simulator.hpp Cache.cpp
	g++ Cache.cpp -o cache_simulate_inclusive
cache_simulate: simulator_inclusive.hpp Cache_inclusive.cpp
	g++ Cache_inclusive.cpp -o cache_simulate
run_all: cache_simulate varriations.sh
	./varriations.sh