#include "simulator.hpp"
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

// int main(int argc,char *argv[]){
//     int block_size = stoi(argv[1]);
//     int L1_size = stoi(argv[1]);
//     int L1_assoc = stoi(argv[1]);
//     int L2_size = stoi(argv[1]);
//     int L2_assoc = stoi(argv[1]);
int main(){

    int block_size = 64;
    int L1_size = 1024;
    int L1_assoc = 2;
    int L2_size = 65536;
    int L2_assoc = 8;
    Heirarchy* heirarchy=(Heirarchy*)(new Heirarchy(block_size, 2, {L1_size,L2_size}, {L1_assoc,L2_assoc}));
    std::ifstream file("memory_trace_files/trace5.txt");
	if (file.is_open())
    {
        std::string line;
        char c;
        char* num;
        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            if (iss >> c >> num)
            {
                string s(num);
                s="0x"+s;
                if(c=='r')
                {
                    heirarchy->read(stoi(s));
                }
                else if(c=='w')
                {
                    heirarchy->write(stoi(s));
                }
            }
            else{
                cout<<"Error in reading file"<<endl;
                cout<<line<<endl;
            }
        }
        file.close();
    }
	else
	{
		std::cerr << "File could not be opened. Terminating...\n";
		return 0;
	}
	heirarchy->print_stats();
	return 0;
}