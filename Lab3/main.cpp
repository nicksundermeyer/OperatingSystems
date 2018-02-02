/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.cpp
 * Author: Evan Reierson and Nick Sundermeyer
 *
 * Created on January 25, 2018, 3:45 PM
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "PageFrameAllocator.h"

using std::cout;
using std::endl;
using std::cerr;
using std::string;
using std::dec;
using std::hex;
using std::istringstream;
/*
 *
 */
int main(int argc, char** argv) {

    std::ifstream file;
    file.open(argv[1]);
    
    // make sure file read is valid
    if (!file){
        cerr << "cannot open file" << endl;
        exit(1);
    }

    int num_page_frames;
    std::vector<uint32_t> page_frames;
    string current_line;

    getline(file, current_line);
    istringstream iss(current_line);

    iss >> hex >> num_page_frames;

    cout << ">" << hex << num_page_frames << endl;;

    PageFrameAllocator allocator(num_page_frames); 
    
    while (getline(file, current_line)){
        cout << ">" << current_line << endl;
        istringstream iss(current_line);

        int command;
        int count;

        iss >> hex >> command;

        if(command == 0)
        {
            iss >> hex >> count;

            bool success = allocator.Deallocate(count, page_frames);
            cout << " ";
            
            if(success)
                cout << "T";
            else
                cout << "F";

            cout << " " << hex << allocator.get_page_frames_free() << endl;
        }
        else if(command == 1)
        {
            iss >> hex >> count;

            bool success = allocator.Allocate(count, page_frames);
            cout << " ";
            
            if(success)
                cout << "T";
            else
                cout << "F";

            cout << " " << hex << allocator.get_page_frames_free() << endl;
        }
        else if(command == 2)
        {
            cout << " ";
            uint32_t temp = allocator.get_free_list_head();
            while(temp != 0xFFFFFFFF)
            {
                cout << hex << temp << " ";
                temp = allocator.get_next_page(temp);
            }
            
            cout << endl;
        }

    }

    return 0;
}
