#ifndef __LEAF_H__
#define __LEAF_H__

#include <fstream>
#include <stdint.h>
#include <string>

#include "IndexDataStructure.h"

class Leaf : public IndexDataStructure
{
public:
    explicit Leaf(const uint16_t _max_count, const uint16_t _block_size);
    virtual ~Leaf();

    uint32_t insert(uint32_t _key, uint64_t _address);
    uint32_t split(Leaf & _new_leaf);
    void setNextLeafAddress(uint64_t _next_leaf_address);

    void loadAt(std::fstream & _input_file, std::streampos _position);
    void update(std::fstream & _output_file, std::streampos _position);

    std::string toString();
};

#endif
