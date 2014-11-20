/*
// 2 leafs pages structure:
//                                       A node
//
//                               count  key   key  next
//                               +--------------------+
//                               |   |  10 |  80 |    |
//                               | 2 |-----+-----| pnt|
//                               |   | pnt | pnt |    |
//                               +--------------------+
//                                     /     /      \
//             _______________________/     /        \_____NOT USED_____
//            /                            /             IN THIS CODE   \
//         Leaf 1                        Leaf 2                        ----
//
// count  key   key  next        count  key   key  next        count  key   key  next
// +--------------------+        +--------------------+        +--------------------+
// |   |  01 |  09 |    |        |   |  10 | ... |    |        |   | ... | ... |    |
// | 2 |-----+-----| pnt|---->---| 1 |-----+-----|NULL|        | . |-----+-----| ...|
// |   | pnt | pnt |    |        |   | pnt | ... |    |        |   | ... | ... |    |
// +--------------------+        +--------------------+        +--------------------+
//         \      \_______________      |
//          \____________________ \      \
//                               \ \      |
//                               +--------------------+
//                               | real data archive  |
//                               +--------------------+
*/

#define CS 2    // uint16_t COUNTER_SIZE
#define KS 4    // uint32_t KEY_SIZE
#define PS 8    // uint64_t POINTER_SIZE
#define PX 8    // uint64_t NEXT_POINTER_SIZE

//#define NK 340  // NUM_OF_KEY_PER_LEAF
//total
//#define BS 4096 // BLOCK_SIZE: 2 + (4 + 8) * 340 + 8 = 4090

#define NK 4  // NUM_OF_KEY_PER_LEAF
#define BS 60 // BLOCK_SIZE: 2 + (4 + 8) * NK + 8 = 58

#define tmp_num_ceps 23

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <stdint.h>

#include "Node.h"

int main(int _n_args, char ** _v_args)
{
    // random unique insertions
    std::list<uint32_t> lst_unique_keys;

    /*
    uint32_t key;

    while(lst_unique_keys.size() < tmp_num_ceps)
    {
        key =  rand() % 100;

        if(std::find(lst_unique_keys.begin(), lst_unique_keys.end(), key) == lst_unique_keys.end())
        {
            lst_unique_keys.push_back(key);
        }
    }
    */

    lst_unique_keys.push_back(15);
    lst_unique_keys.push_back(24);
    lst_unique_keys.push_back(32);
    lst_unique_keys.push_back(61);

    std::fstream fs_index ("index.dat", std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);

    if(!fs_index)
    {
        std::cerr << "Can't open file" << std::endl;

        return 1;
    }

    Node root(fs_index, NK, BS);
    std::cout << "keys per leaf: " << NK << ", block size: " << BS << std::endl << std::endl;

    std::list<uint32_t>::iterator it;

    for(it = lst_unique_keys.begin(); it != lst_unique_keys.end(); ++it)
    {
        std::cout << "********** insert: " << (*it) << ", " << 100 - (*it) <<  " **********" << std::endl;
        root.insert((*it), 100 - (*it));
        std::cout << root.toString() << std::endl;
    }

    std::cout << std::endl << "=== result: ===" << std::endl;
    std::cout << root.toString() << std::endl << std::endl;

    std::cout << "done." << std::endl;

    fs_index.close();

    return 0;
}
