#define KS 2    // KEY_SIZE
#define CS 2    // COUNTER_SIZE
#define PS 8    // POINTER_SIZE
#define BS 4096 // BLOCK_SIZE
#define NK 340  // NUM_OF_KEY_PER_LEAF

#define tmp_num_ceps 600000

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

    Node root(NK);

    for(int i = 0; i < tmp_num_ceps; ++i)
    {
        root.insert(i, i);
    }

    std::cout << "done." << std::endl;

    return 0;
}
