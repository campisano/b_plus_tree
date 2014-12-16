/*
// 2 leafs pages structure:
//                                       A node
//
//                               count  key   key  next
//                               +--------------------+
//                               |   |  10 |  80 |    | (this pnt is actually used
//                               | 2 |-----+-----| pnt|  to determine if the
//                               |   | pnt | pnt |    |  node childrens are leaf)
//                               +--------------------+
//                                      |     |      \
//                                      |     |       NOT USED
//                                       \   /      IN THIS CODE
//                                        | |       /            \
//                                 +----------------+             (this is the
//                                 | index archive  |              original b+tree
//                                 +----------------+              structure)
//                                     /     /     \              /
//             _______________________/     /       \______NOT USED_____
//            /                            /             IN THIS CODE   \
//         Leaf 1                        Leaf 2                        -----
//
// count  key   key  next        count  key   key  next        count  key   key  next
// +--------------------+        +--------------------+        +--------------------+
// |   |  10 |  19 |    |        |   |  80 | ... |    |        |   | ... | ... |    |
// | 2 |-----+-----| pnt|---->---| 1 |-----+-----|NULL|        | . |-----+-----| ...|
// |   | pnt | pnt |    |        |   | pnt | ... |    |        |   | ... | ... |    |
// +--------------------+        +--------------------+        +--------------------+
//         \      \_______________      |
//          \____________________ \      \
//                               \ \      |
//                               +--------------------+
//                               | final data archive |
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

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <stdexcept>
#include <stdint.h>

#include "Node.h"

int main(int _n_args, char ** _v_args)
{
    // gerate random unique keys to insert
    std::list<uint32_t> lst_unique_keys;
    {
        uint32_t key;

        while(lst_unique_keys.size() < 23)
        {
            key =  rand() % 100;

            if(std::find(lst_unique_keys.begin(), lst_unique_keys.end(), key) == lst_unique_keys.end())
            {
                lst_unique_keys.push_back(key);
            }
        }
    }

    // open the index file, truncating
    std::fstream fs_index ("index.dat", std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);

    if(!fs_index)
    {
        std::cerr << "Can't open file" << std::endl;

        return 1;
    }

    std::cout << "keys per leaf: " << NK << ", block size: " << BS << std::endl << std::endl;

    // allocate root node
    Node * root = new Node(fs_index, NK, BS);

    // write initial root
    root->writeToNext(fs_index);

    bool inserted;
    uint32_t new_key;
    uint64_t new_address;
    std::list<uint32_t>::iterator it;

    // insert all keys with they disk address
    for(it = lst_unique_keys.begin(); it != lst_unique_keys.end(); ++it)
    {
        new_key = (*it);
        new_address = 100 - new_key;

        std::cout << "************************* INSERT: " << new_key << ", " << new_address <<  " *************************" << std::endl;

        // insert
        inserted = root->insert(new_key, new_address);

        // if not inserted then node need to be splitted and rotated
        if(!inserted)
        {
            // create new root node
            Node * new_root = new Node(fs_index, NK, BS);

            // rotate
            new_root->splitAndRotate(*root, new_key, new_address);

            // delete old root node
            delete root;

            // swap
            root = new_root;
        }

        std::cout << root->toString() << std::endl; // print status at every insert
    }

    // print finally index status
    std::cout << std::endl << "RESULT: =======================================================" << std::endl;
    std::cout << root->toString() << std::endl << std::endl;

    std::cout << "done." << std::endl;

    // free root node memory
    delete root;

    // close the index file
    fs_index.close();

    return 0;
}
