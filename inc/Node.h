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

public:
    bool hasLeafs();
    void hasLeafs(bool _has_leafs);
    bool insert(uint32_t _new_key, uint64_t _new_address);

    void splitOldRootAndRotate(Node & _old_root_to_split, uint32_t _new_key, uint64_t _new_address);

    std::string toString();

    virtual uint64_t getAddressForKey(std::fstream & _fs_index, uint32_t _key);

private:
    std::fstream & m_fs_index;
};

#endif
