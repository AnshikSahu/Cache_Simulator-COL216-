#ifndef _SIMULATOR_HPP_
#define _SIMULATOR_HPP_

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <bitset>

using namespace std;


struct Cache{

    struct CacheBlock{
        bool valid;
        bool dirty;
        long long int tag;
        int lru;
        long long int address;
        CacheBlock(){
            valid = false;
            dirty = false;
            tag = 0;
            lru = 1;
            address = 0;
        }
    };

    struct CacheSet{
        vector<CacheBlock> blocks;
        CacheSet(int associativity){
            for(int i = 0; i < associativity; i++){
                CacheBlock block;
                blocks.push_back(block);
            }
        }
    };
    
    int size;
    int associativity;
    int block_size;
    int num_sets;
    int num_blocks;
    int num_bits_offset;
    int num_bits_index;
    int num_bits_tag;
    int num_accesses;
    int num_hits;
    int num_misses;
    int num_write_backs;
    int num_reads;
    int num_writes;
    int num_read_hits;
    int num_write_hits;
    int num_read_misses;
    int num_write_misses;
    float miss_rate;
    float read_miss_rate;
    float write_miss_rate;
    int level;

    vector<CacheSet> sets;
    struct Cache *next_level;
    struct Cache *prev_level;
    
    Cache(int size, int associativity, int block_size, int level){
        this->size = size;
        this->associativity = associativity;
        this->block_size = block_size;
        this->num_sets = size/(associativity*block_size);
        this->num_blocks = size/block_size;
        this->num_bits_offset = log2(block_size);
        this->num_bits_index = log2(num_sets);
        this->num_bits_tag = 48 - num_bits_offset - num_bits_index;
        this->num_accesses = 0;
        this->num_hits = 0;
        this->num_misses = 0;
        this->num_write_backs = 0;
        this->num_reads = 0;
        this->num_writes = 0;
        this->num_read_hits = 0;
        this->num_write_hits = 0;
        this->num_read_misses = 0;
        this->num_write_misses = 0;
        this->next_level = NULL;
        this->prev_level = NULL;
        this->miss_rate = 0;
        this->read_miss_rate = 0;
        this->write_miss_rate = 0;
        this->level = level;
        
        for(int i = 0; i < num_sets; i++){
            CacheSet set(associativity);
            sets.push_back(set);
        }
    }

    void read(long long int address){
        num_accesses++;
        num_reads++;
        int index = (address >> num_bits_offset) & ((1 << num_bits_index) - 1);
        long long int tag = (address >> (num_bits_offset + num_bits_index));
        bool hit = false;
        for(int i = 0; i < associativity; i++){
            if(sets[index].blocks[i].valid && sets[index].blocks[i].tag == tag){
                hit = true;
                num_hits++;
                num_read_hits++;
                sets[index].blocks[i].lru = 0;
                break;
            }
        }
        if(!hit){
            num_misses++;
            num_read_misses++;
            int lru = 0;
            int lru_index = 0;
            for(int i = 0; i < associativity; i++){
                if(sets[index].blocks[i].lru > lru){
                    lru = sets[index].blocks[i].lru;
                    lru_index = i;
                }
            }
            if(sets[index].blocks[lru_index].dirty){
                num_write_backs++;
                if(next_level != NULL){
                    next_level->write(sets[index].blocks[lru_index].address);
                }
            }
            if(next_level != NULL){
                next_level->read(address);
            }

            sets[index].blocks[lru_index].valid = true;
            sets[index].blocks[lru_index].dirty = false;
            sets[index].blocks[lru_index].tag = tag;
            sets[index].blocks[lru_index].lru = 0;
            sets[index].blocks[lru_index].address = address;
        }
        for(int i = 0; i < associativity; i++){
            sets[index].blocks[i].lru++;
        }
    }

    void write(long long int address){
        num_accesses++;
        num_writes++;
        int index = (address >> num_bits_offset) & ((1 << num_bits_index) - 1);
        long long int tag = (address >> (num_bits_offset + num_bits_index));
        bool hit = false;
        for(int i = 0; i < associativity; i++){
            if(sets[index].blocks[i].valid && sets[index].blocks[i].tag == tag){
                hit = true;
                num_hits++;
                num_write_hits++;
                sets[index].blocks[i].lru = 0;
                sets[index].blocks[i].dirty = true;
                break;
            }
        }
        if(!hit){
            num_misses++;
            num_write_misses++;
            int lru = 0;
            int lru_index = 0;
            for(int i = 0; i < associativity; i++){
                if(sets[index].blocks[i].lru > lru){
                    lru = sets[index].blocks[i].lru;
                    lru_index = i;
                }
            }
            if(sets[index].blocks[lru_index].dirty){
                num_write_backs++;
                if(next_level != NULL){
                    ((struct Cache*)next_level)->write(sets[index].blocks[lru_index].address);
                }
            }
            if(next_level != NULL){
                ((struct Cache*)next_level)->read(address);
            }
            sets[index].blocks[lru_index].valid = true;
            sets[index].blocks[lru_index].dirty = true;
            sets[index].blocks[lru_index].tag = tag;
            sets[index].blocks[lru_index].lru = 0;
            sets[index].blocks[lru_index].address = address;
        }
        for(int i = 0; i < associativity; i++){
            sets[index].blocks[i].lru++;
        }
    }


    void compute_stats(){
        this->miss_rate = (float)num_misses/(float)num_accesses;
        this->read_miss_rate = (float)num_read_misses/(float)num_reads;
        this->write_miss_rate = (float)num_write_misses/(float)num_writes;
    }

    void print_stats(){
        if(level == 1){
            // cout << "Number of accesses = " << num_accesses << endl;
            // cout << "Number of hits = " << num_hits << endl;
            // cout << "Number of misses = " << num_misses << endl;
            cout << "i. number of L1 reads = " << num_reads << endl;
            cout << "ii. number of L1 read misses = " << num_read_misses << endl;
            cout << "iii. number of L1 writes = " << num_writes << endl;
            // cout << "Number of read hits = " << num_read_hits << endl;
            // cout << "Number of write hits = " << num_write_hits << endl;
            cout << "iv. number of write misses = " << num_write_misses << endl;
            cout << "v. L1 miss rate = " << miss_rate << endl;
            cout << "vi. number of write backs = " << num_write_backs << endl;
        }
        else{
            cout << "vii. number of L2 reads = " << num_reads << endl;
            cout << "viii. number of L2 read misses = " << num_read_misses << endl;
            cout << "ix. number of L2 writes = " << num_writes << endl;
            cout << "x. number of L2 write misses = " << num_write_misses << endl;
            cout << "xi. L2 miss rate = " << miss_rate << endl;
            cout << "xii. number of L2 write backs = " << num_write_backs << endl;
        }
    }

};


struct Heirarchy{
    int num_levels;
    vector<struct Cache*> levels;
    int block_size;
    int access_time;

    Heirarchy(int block_size, int num_levels, vector<int> sizes, vector<int> associativity){
        this->block_size = block_size;
        this->num_levels = num_levels;
        for(int i = 0; i < num_levels; i++){
            Cache *cache = new Cache(sizes[i], associativity[i], block_size, i+1);
            if(i>0){
                cache->prev_level = levels[i - 1];
                levels[i - 1]->next_level = cache;
            }
            levels.push_back(cache);
        } 
        this->access_time = 0;       
    }

    void read(int address){
        levels[0]->read(address);
    }

    void write(int address){
        levels[0]->write(address);
    }

    void compute_stats(){
        for(int i=0; i < num_levels; i++){
            levels[i]->compute_stats();
        }
        access_time = 1*levels[0]->num_accesses + 20*levels[1]->num_accesses + 200*(levels[1]->num_misses+levels[1]->num_write_backs);
    }

    void print_stats(){
        cout<<"===== Simulation results ====="<<endl;
        for(int i=0; i < num_levels; i++){
            levels[i]->print_stats();
        }
        cout << "Access time = " << access_time << endl;
    }
};

#endif