#ifndef __LEAF_H__
#define __LEAF_H__

#include <stdint.h>
#include <string>

#include "IndexDataStructure.h"

class Leaf : public IndexDataStructure
{
public:
    explicit Leaf(const uint16_t _max_count, const uint16_t _block_size);
    virtual ~Leaf();

public:
    void insert(uint32_t _key, uint64_t _address);
    void setNextLeafAddress(uint64_t _next_leaf_address);

    std::string toString();

    virtual uint64_t getAddressForKey(std::fstream & _fs_index, uint32_t _key);
};

#endif
