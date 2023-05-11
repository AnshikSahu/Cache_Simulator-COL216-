#include "simulator_inclusive.hpp"
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

int main(int argc,char *argv[]){
    int block_size = stoi(argv[1]);
    int L1_size = stoi(argv[2]);
    int L1_assoc = stoi(argv[3]);
    int L2_size = stoi(argv[4]);
    int L2_assoc = stoi(argv[5]);
    Heirarchy* heirarchy=(Heirarchy*)(new Heirarchy(block_size, 2, {L1_size,L2_size}, {L1_assoc,L2_assoc}));
    std::ifstream file(argv[6]);
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
                long long int hexa=0;
                for(int i=0;i<s.length();i++)
                {
                    if(s[i]>='0' && s[i]<='9')
                    {
                        hexa=hexa*16+(s[i]-'0');
                    }
                    else if(s[i]>='a' && s[i]<='f')
                    {
                        hexa=hexa*16+(s[i]-'a'+10);
                    }
                    else if(s[i]>='A' && s[i]<='F')
                    {
                        hexa=hexa*16+(s[i]-'A'+10);
                    }
                }
                if(c=='r')
                {
                    heirarchy->read(hexa);
                }
                else if(c=='w')
                {
                    heirarchy->write(hexa);
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
    heirarchy->compute_stats();
	heirarchy->print_stats();
	return 0;
}