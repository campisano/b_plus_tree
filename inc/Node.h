#define __NODE_H__

#include <list>
#include <stdint.h>
#include <string>

class Leaf;

class Node
{
public:
    explicit Node(uint16_t _max_count, uint16_t _block_size);
    virtual ~Node();

    void insert(uint32_t _new_key, uint64_t _new_address);

    std::string toString();

private:
    uint16_t m_max_count;
    uint16_t m_block_size;

    std::list<uint32_t> m_keys;
    std::list<Leaf *> m_leafs;

    // data struct to write on disk // TODO [CMP] código duplicado em leaf
    uint16_t m_count;
    //uint32_t * m_keys;
    //uint64_t * m_addresses;
    //uint64_t m_next_leaf_address;
};
