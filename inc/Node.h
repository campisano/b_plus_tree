#define __NODE_H__

#include <fstream>
#include <stdint.h>
#include <string>

class Leaf;

class Node
{
public:
    explicit Node(std::fstream & _fs_index, const uint16_t _max_count, const uint16_t _block_size);
    virtual ~Node();

    void insert(uint32_t _new_key, uint64_t _new_address);

    std::string toString();

private:
    std::fstream & m_fs_index;

    const uint16_t m_max_count;
    const uint16_t m_block_size;
    uint16_t m_real_data_size;

    // data struct to write on disk
    uint16_t m_count;
    uint32_t * m_keys;
    uint64_t * m_leaf_addresses;
};
