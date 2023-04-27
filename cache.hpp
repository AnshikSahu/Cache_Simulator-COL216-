#ifndef _CACHE_HPP_
#define _CACHE_HPP_

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmath>
#include <ctime>
#include <cstdlib>

using namespace std;

struct Cache{

    struct CacheBlock{
        bool valid;
        bool dirty;
        int tag;
        int lru;
        int address;
        int data;
        CacheBlock(){
            valid = false;
            dirty = false;
            tag = 0;
            lru = 0;
            address = 0;
            data = 0;
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
    struct Cache *next_level;
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
    }

    int read(int address){
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
                return sets[index].blocks[i].data;
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
                    next_level->write(sets[index].blocks[lru_index].address,sets[index].blocks[lru_index].data);
                }
            }
            sets[index].blocks[lru_index].valid = true;
            sets[index].blocks[lru_index].dirty = false;
            sets[index].blocks[lru_index].tag = tag;
            sets[index].blocks[lru_index].lru = 0;
            sets[index].blocks[lru_index].address = address;
            if(next_level != NULL){
                sets[index].blocks[lru_index].data = next_level->read(address);
            }
            return sets[index].blocks[lru_index].data;
        }
        for(int i = 0; i < associativity; i++){
            if(sets[index].blocks[i].valid){
                sets[index].blocks[i].lru++;
            }
        }
        return 0;
    }

    void write(int address, int data){
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
                sets[index].blocks[i].data = data;
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
                    next_level->write(sets[index].blocks[lru_index].address,sets[index].blocks[lru_index].data);
                }
            }
            sets[index].blocks[lru_index].valid = true;
            sets[index].blocks[lru_index].dirty = true;
            sets[index].blocks[lru_index].tag = tag;
            sets[index].blocks[lru_index].lru = 0;
            sets[index].blocks[lru_index].address = address;
            sets[index].blocks[lru_index].data = data;
            if(next_level != NULL){
                sets[index].blocks[lru_index].data = next_level->read(address);
            }
        }
        for(int i = 0; i < associativity; i++){
            if(sets[index].blocks[i].valid){
                sets[index].blocks[i].lru++;
            }
        }
    }
    // void read(int address){
    //     num_accesses++;
    //     num_reads++;
    //     int index = (address >> num_bits_offset) & ((1 << num_bits_index) - 1);
    //     int tag = (address >> (num_bits_offset + num_bits_index)) & ((1 << num_bits_tag) - 1);
    //     bool hit = false;
    //     for(int i = 0; i < associativity; i++){
    //         if(sets[index].blocks[i].valid && sets[index].blocks[i].tag == tag){
    //             hit = true;
    //             num_hits++;
    //             num_read_hits++;
    //             sets[index].blocks[i].lru = 0;
    //             break;
    //         }
    //     }
    //     if(!hit){
    //         num_misses++;
    //         num_read_misses++;
    //         int lru = 0;
    //         int lru_index = 0;
    //         for(int i = 0; i < associativity; i++){
    //             if(sets[index].blocks[i].lru > lru){
    //                 lru = sets[index].blocks[i].lru;
    //                 lru_index = i;
    //             }
    //         }
    //         if(sets[index].blocks[lru_index].dirty){
    //             num_write_backs++;
    //             if(next_level != NULL){
    //                 next_level->write(sets[index].blocks[lru_index].address,sets[index].blocks[lru_index].data);
    //             }
    //         }
    //         sets[index].blocks[lru_index].valid = true;
    //         sets[index].blocks[lru_index].dirty = false;
    //         sets[index].blocks[lru_index].tag = tag;
    //         sets[index].blocks[lru_index].lru = 0;
    //         sets[index].blocks[lru_index].address = address;
    //         if(next_level != NULL){
    //             sets[index].blocks[lru_index].data = next_level->read(address);
    //         }
    //     }
    //     for(int i = 0; i < associativity; i++){
    //         if(sets[index].blocks[i].valid){
    //             sets[index].blocks[i].lru++;
    //         }
    //     }
    // }

    // void write(int address){
    //     num_accesses++;
    //     num_writes++;
    //     int index = (address >> num_bits_offset) & ((1 << num_bits_index) - 1);
    //     int tag = (address >> (num_bits_offset + num_bits_index)) & ((1 << num_bits_tag) - 1);
    //     bool hit = false;
    //     for(int i = 0; i < associativity; i++){
    //         if(sets[index].blocks[i].valid && sets[index].blocks[i].tag == tag){
    //             hit = true;
    //             num_hits++;
    //             num_write_hits++;
    //             sets[index].blocks[i].lru = 0;
    //             sets[index].blocks[i].dirty = true;
    //             break;
    //         }
    //     }
    //     if(!hit){
    //         num_misses++;
    //         num_write_misses++;
    //         int lru = 0;
    //         int lru_index = 0;
    //         for(int i = 0; i < associativity; i++){
    //             if(sets[index].blocks[i].lru > lru){
    //                 lru = sets[index].blocks[i].lru;
    //                 lru_index = i;
    //             }
    //         }
    //         if(sets[index].blocks[lru_index].dirty){
    //             num_write_backs++;
    //             if(next_level != NULL){
    //                 next_level->write(sets[index].blocks[lru_index].address,sets[index].blocks[lru_index].data);
    //             }
    //         }
    //         sets[index].blocks[lru_index].valid = true;
    //         sets[index].blocks[lru_index].dirty = true;
    //         sets[index].blocks[lru_index].tag = tag;
    //         sets[index].blocks[lru_index].lru = 0;
    //         sets[index].blocks[lru_index].address = address;
    //         if(next_level != NULL){
    //             sets[index].blocks[lru_index].data = next_level->read(address);
    //         }
    //     }
    //     for(int i = 0; i < associativity; i++){
    //         if(sets[index].blocks[i].valid){
    //             sets[index].blocks[i].lru++;
    //         }
    //     }
    // }

};
#endif