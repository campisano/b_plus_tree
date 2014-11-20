#define CS 2    // COUNTER_SIZE
#define KS 4    // KEY_SIZE
#define PS 8    // POINTER_SIZE
#define PX 8    // NEXT_POINTER_SIZE

//#define NK 340  // NUM_OF_KEY_PER_LEAF
//total: (4 + 8) * 340 + 2 + 8 = 4090
//#define BS 4096 // BLOCK_SIZE

//#define tmp_num_ceps 600000


#define NK 4  // NUM_OF_KEY_PER_LEAF
//total: (4 + 8) * 4 + 2 + 8 = 58
#define BS 60 // BLOCK_SIZE

#define tmp_num_ceps 15

#include <iostream>
#include <list>
#include "Node.h"

int main(int _n_args, char ** _v_args)
{
    std::list<long int> lst_keys;

    for(int i = 0; i < tmp_num_ceps; ++i)
    {
        lst_keys.push_back(i);
    }

    Node root(NK, BS);

    for(int i = 0; i < tmp_num_ceps; ++i)
    {
        root.insert(i, i);
        std::cout << "||||||||||| insert: " << i << " |||||||||||||||" << std::endl;
        std::cout << root.toString() << std::endl << std::endl;
    }

    std::cout << "done." << std::endl;

    return 0;
}
