#ifndef _SIMULATOR_HPP_
#define _SIMULATOR_HPP_

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <cmath>

using namespace std;

struct Cache{

    struct CacheBlock{
        bool valid;
        bool dirty;
        int tag;
        int lru;
        int address;
        CacheBlock(){
            valid = false;
            dirty = false;
            tag = 0;
            lru = 0;
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

    vector<CacheSet> sets;
    void *next_level;
    struct Cache *prev_level;
    
    Cache(int size, int associativity, int block_size){
        this->size = size;
        this->associativity = associativity;
        this->block_size = block_size;
        this->num_sets = size/(associativity*block_size);
        this->num_blocks = size/block_size;
        this->num_bits_offset = log2(block_size);
        this->num_bits_index = log2(num_sets);
        this->num_bits_tag = 32 - num_bits_offset - num_bits_index;
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
        
        for(int i = 0; i < num_sets; i++){
            CacheSet set(associativity);
            sets.push_back(set);
        }
    }

    void read(int address){
        num_accesses++;
        num_reads++;
        int index = (address >> num_bits_offset) & ((1 << num_bits_index) - 1);
        int tag = (address >> (num_bits_offset + num_bits_index)) & ((1 << num_bits_tag) - 1);
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
                    ((struct Cache*)next_level)->write(sets[index].blocks[lru_index].address);
                }
            }
            if(next_level != NULL){
                    ((struct Cache*)next_level)->read(address);
                }
            sets[index].blocks[lru_index].valid = true;
            sets[index].blocks[lru_index].dirty = false;
            sets[index].blocks[lru_index].tag = tag;
            sets[index].blocks[lru_index].lru = 0;
            sets[index].blocks[lru_index].address = address;
        }
        for(int i = 0; i < associativity; i++){
            if(sets[index].blocks[i].valid){
                sets[index].blocks[i].lru++;
            }
        }
    }

    void write(int address){
        num_accesses++;
        num_writes++;
        int index = (address >> num_bits_offset) & ((1 << num_bits_index) - 1);
        int tag = (address >> (num_bits_offset + num_bits_index)) & ((1 << num_bits_tag) - 1);
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
            sets[index].blocks[lru_index].valid = true;
            sets[index].blocks[lru_index].dirty = true;
            sets[index].blocks[lru_index].tag = tag;
            sets[index].blocks[lru_index].lru = 0;
            sets[index].blocks[lru_index].address = address;
        }
        for(int i = 0; i < associativity; i++){
            if(sets[index].blocks[i].valid){
                sets[index].blocks[i].lru++;
            }
        }
    }

    void print_stats(){
        cout << "Number of accesses = " << num_accesses << endl;
        cout << "Number of hits = " << num_hits << endl;
        cout << "Number of misses = " << num_misses << endl;
        cout << "Number of write backs = " << num_write_backs << endl;
        cout << "Number of reads = " << num_reads << endl;
        cout << "Number of writes = " << num_writes << endl;
        cout << "Number of read hits = " << num_read_hits << endl;
        cout << "Number of write hits = " << num_write_hits << endl;
        cout << "Number of read misses = " << num_read_misses << endl;
        cout << "Number of write misses = " << num_write_misses << endl;
    }

};

struct Dram{
    int size;
    struct Cache *prev_level;

    Dram(int size){
        this->size = size;
        this->prev_level = NULL;
    }

    void read(int address){
        return ;
    }

    void write(int address){
        return ;
    }
};

struct Heirarchy{
    int num_levels;
    vector<struct Cache*> levels;
    struct Dram *DRAM;

    Heirarchy(int L1_size, int L1_associativity, int L1_block_size, int L2_size, int L2_associativity, int L2_block_size, int DRAM_size, int num_levels){
        DRAM = new Dram(DRAM_size);
        this->num_levels = num_levels;
        for(int i = 0; i < num_levels; i++){
            if(i == 0){
                levels.push_back(new Cache(L1_size, L1_associativity, L1_block_size));
                levels[i]->next_level = DRAM;
            }
            else if(i == 1){
                levels.push_back(new Cache(L2_size, L2_associativity, L2_block_size));
                levels[i]->next_level = levels[i - 1];
                levels[i-1]->prev_level = levels[i];
            }
        }        
    }

    void read(int address){
        levels[num_levels - 1]->read(address);
    }

    void write(int address){
        levels[num_levels - 1]->write(address);
    }

    void print_stats(){
        for(int i=0; i < num_levels; i++){
            cout << "L" << i + 1 << " Cache:\n";
            levels[i]->print_stats();
        }
    }
};

#endif
