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

#define tmp_num_ceps 23

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <list>

#include "Node.h"

int main(int _n_args, char ** _v_args)
{
    //TODO [CMP] bug!
    // em caso de split, o novo leaf é adicionado no topo, mas node está achando que os leaf embaixo deles estão ordenados.
    // na verdade no disco essa varridura deve ser ordenada

    // random unique insertions
    std::list<unsigned long int> lst_unique_keys;

    unsigned long int key;

    while(lst_unique_keys.size() < tmp_num_ceps)
    {
        key =  rand() % 100;

        if(std::find(lst_unique_keys.begin(), lst_unique_keys.end(), key) == lst_unique_keys.end())
        {
            lst_unique_keys.push_back(key);
        }
    }

    Node root(NK, BS);

    std::list<unsigned long int>::iterator it;

    for(it = lst_unique_keys.begin(); it != lst_unique_keys.end(); ++it)
    {
        std::cout << "********** insert: " << (*it) << " **********" << std::endl;
        root.insert((*it), 100 - (*it));
        std::cout << root.toString() << std::endl;
    }

    std::cout << "done." << std::endl;

    return 0;
}
