/*
// 2 leafs pages structure:
//                                       A node
//
//                               count  key   key  next
//                               +--------------------+
//                               |   |  10 |  80 |    | (this pnt is actually used
//                               | 2 |-----+-----| pnt|  to determine if the current
//                               |   | pnt | pnt |    |  node childrens are leafs)
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

#define NK 340  // NUM_OF_KEY_PER_LEAF
#define BS 4096 // BLOCK_SIZE: 2 + (4 + 8) * 340 + 8 = 4090

//#define NK 4  // NUM_OF_KEY_PER_LEAF
//#define BS 60 // BLOCK_SIZE: 2 + (4 + 8) * NK + 8 = 58

//#define NK 2  // NUM_OF_KEY_PER_LEAF
//#define BS 40 // BLOCK_SIZE: 2 + (4 + 8) * NK + 8 = 34

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
#include <stdexcept>
#include <stdint.h>

#include "Node.h"

typedef struct {

    char endereco[80];
    char bairro[74];
    char cidade[27];
    char nomeEstado[23];
    char siglaEstado[3];
    char cep[9];

} Endereco;

int main(int _n_args, char ** _v_args)
{
    // TODO DEBUG
    /*
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
    */

    FILE * cep_file;
    {
        cep_file = fopen("cep_ordenado_ascii_uniq.dat", "r");

        if(!cep_file)
        {
            std::cerr << "Can't open data file " << "cep_ordenado_ascii_uniq.dat" << " for reading." << std::endl;

            return 1;
        }
    }

    // open the index file, truncating
    std::fstream fs_index ("index.dat", std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);

    if(!fs_index)
    {
        std::cerr << "Can't open index file " << "index.dat" << " for writing." << std::endl;

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
    //std::list<uint32_t>::iterator it;
    Endereco e;

    // insert all keys with they disk address
    //for(it = lst_unique_keys.begin(); it != lst_unique_keys.end(); ++it)
    do
    {
        new_address = ftell(cep_file);

        memset(&e, 0, sizeof(Endereco));
        fread(&e.endereco, 1, 79, cep_file);
        fread(&e.bairro, 1, 73, cep_file);
        fread(&e.cidade, 1, 26, cep_file);
        fread(&e.nomeEstado, 1, 22, cep_file);
        fread(&e.siglaEstado, 1, 2, cep_file);
        fread(&e.cep, 1, 8, cep_file);
        fseek(cep_file, 2, SEEK_CUR);
/*
        if(strcmp(e.cep, "20771020") == 0)
        {
            printf("\n");
            printf("Endereco.....: %s\n", e.endereco);
            printf("Bairro.......: %s\n", e.bairro);
            printf("Cidade.......: %s\n", e.cidade);
            printf("Nome Estado..: %s\n", e.nomeEstado);
            printf("Sigla Estado.: %s\n", e.siglaEstado);
            printf("CEP..........: %s\n", e.cep);
            break;
        }
*/
        //new_key = (*it);
        new_key = atoi(e.cep);
        //new_address = 100 - new_key;

        //std::cout << "************************* INSERT: " << new_key << ", " << new_address <<  " *************************" << std::endl;

        // insert
        inserted = root->insert(new_key, new_address);

        // if not inserted then node need to be splitted and rotated
        if(!inserted)
        {
            // create new root node
            Node * new_root = new Node(fs_index, NK, BS);

            // rotate
            new_root->splitOldRootAndRotate(*root, new_key, new_address);

            // delete old root node
            delete root;

            // swap
            root = new_root;
        }

        //std::cout << root->toString() << std::endl; // print status at every insert
    } while (strlen(e.cep) > 0);

    // print finally index status
    std::cout << std::endl << "RESULT: =======================================================" << std::endl;
    //std::cout << root->toString() << std::endl << std::endl;

    std::cout << "root index position: " << root->getIndexPosition() << std::endl;
    std::cout << "done." << std::endl;



    uint64_t address = root->getAddressForKey(fs_index, 20771020);

    memset(&e, 0, sizeof(Endereco));
    fseek(cep_file, address, SEEK_SET);
    fread(&e.endereco, 1, 79, cep_file);
    fread(&e.bairro, 1, 73, cep_file);
    fread(&e.cidade, 1, 26, cep_file);
    fread(&e.nomeEstado, 1, 22, cep_file);
    fread(&e.siglaEstado, 1, 2, cep_file);
    fread(&e.cep, 1, 8, cep_file);
    fseek(cep_file, 2, SEEK_CUR);

    printf("\n");
    printf("Endereco.....: %s\n", e.endereco);
    printf("Bairro.......: %s\n", e.bairro);
    printf("Cidade.......: %s\n", e.cidade);
    printf("Nome Estado..: %s\n", e.nomeEstado);
    printf("Sigla Estado.: %s\n", e.siglaEstado);
    printf("CEP..........: %s\n", e.cep);



    // free root node memory
    delete root;

    // close data file
    fclose(cep_file);

    // close the index file
    fs_index.close();

    return 0;
}
