#ifndef __NODE_H__
#define __NODE_H__

#include <fstream>
#include <stdint.h>
#include <string>

#include "IndexDataStructure.h"

class Leaf;

class Node : public IndexDataStructure
{
public:
    explicit Node(std::fstream & _fs_index, const uint16_t _max_count, const uint16_t _block_size);
    virtual ~Node();

    bool hasLeafs();
    Node * insert(uint32_t _new_key, uint64_t _new_address);

    std::string toString();

private:
    std::fstream & m_fs_index;
};

#endif
