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

void help(char * _command_name);
int createIndex(char * _input_data, char * _output_index);
int findData(char * _input_data, char * _output_index, char * _key);

int main(int _n_args, char ** _v_args)
{
    if(_n_args == 4 && (strcmp(_v_args[1], "--create-index") == 0))
    {
        return createIndex(_v_args[2], _v_args[3]);
    }
    else if(_n_args == 5 && (strcmp(_v_args[1], "--find-data") == 0))
    {
        return findData(_v_args[2], _v_args[3], _v_args[4]);
    }
    else
    {
        help(_v_args[0]);

        return 1;
    }
}

void help(char * _command_name)
{
    std::cerr << "Wrong command" << std::endl;
    std::cerr << "Usage: " << _command_name << " --create-index <input_data.dat> <output_index.dat>" << std::endl;
    std::cerr << " Or" << std::endl;
    std::cerr << "Usage: " << _command_name << " --find-data <input_data.dat> <output_index.dat> <KEY>" << std::endl;
}

int createIndex(char * _input_data, char * _output_index)
{
    FILE * cep_file;
    {
        cep_file = fopen(_input_data, "r");

        if(!cep_file)
        {
            std::cerr << "Can't open data file " << _input_data << " for reading." << std::endl;

            return 1;
        }
    }

    // open the index file, truncating
    std::fstream fs_index (_output_index, std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);

    if(!fs_index)
    {
        std::cerr << "Can't open index file " << _output_index << " for writing." << std::endl;

        return 1;
    }

    std::cout << "keys per leaf: " << NK << ", block size: " << BS << std::endl << std::endl;

    // skip initial space to write final root position
    uint64_t final_root_address = 0;
    {
        // write an empty address position
        fs_index.write((char *)(&final_root_address), sizeof(final_root_address));

        // skip free block space
        fs_index.seekg(BS - sizeof(final_root_address), fs_index.cur);
    }

    // allocate root node
    Node * root = new Node(fs_index, NK, BS);

    // write initial (temporary) root at current position
    root->writeToNext(fs_index);

    bool inserted;
    uint32_t new_key;
    uint64_t new_address;
    Endereco e;

    // insert all keys with they disk address
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

        new_key = atoi(e.cep);

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

    } while (strlen(e.cep) > 0);

    // print finally index status
    //std::cout << std::endl << "RESULT: =======================================================" << std::endl;
    //std::cout << root->toString() << std::endl << std::endl;

    std::cout << "root index position: " << root->getIndexPosition() << std::endl;
    std::cout << "done." << std::endl;

    // write final root address at start position
    final_root_address = root->getIndexPosition();
    {
        // rewind to first byte of index file
        fs_index.seekp(0, fs_index.beg);

        // write final root position
        fs_index.write((char *)(&final_root_address), sizeof(final_root_address));
    }

    // free root node memory
    delete root;

    // close data file
    fclose(cep_file);

    // close the index file
    fs_index.close();

    return 0;
}

int findData(char * _input_data, char * _output_index, char * _key)
{
    FILE * cep_file;
    {
        cep_file = fopen(_input_data, "r");

        if(!cep_file)
        {
            std::cerr << "Can't open data file " << _input_data << " for reading." << std::endl;

            return 1;
        }
    }

    // open the index file, readonly
    std::fstream fs_index ("index.dat", std::fstream::in | std::fstream::binary);

    if(!fs_index)
    {
        std::cerr << "Can't open index file " << _output_index << " for writing." << std::endl;

        return 1;
    }

    std::cout << "keys per leaf: " << NK << ", block size: " << BS << std::endl << std::endl;

    // load root position
    uint64_t final_root_address = 0;
    {
        // write an empty address position
        fs_index.read((char *)(&final_root_address), sizeof(final_root_address));
    }

    // allocate root node
    Node * root = new Node(fs_index, NK, BS);

    // load initial root
    root->loadAt(fs_index, final_root_address);
    std::cout << "root index position: " << root->getIndexPosition() << std::endl;

    // find input key
    uint64_t address = root->getAddressForKey(fs_index, atoi(_key));

    Endereco e;
    {
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
    }

    // free root node memory
    delete root;

    // close data file
    fclose(cep_file);

    // close the index file
    fs_index.close();

    return 0;
}
