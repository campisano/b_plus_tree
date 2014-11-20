#define __LEAF_H__

#include <fstream>
#include <stdint.h>
#include <string>

class Leaf
{
public:
    explicit Leaf(const uint16_t _max_count, const uint16_t _block_size);
    virtual ~Leaf();

    inline bool isEmpty() { return m_count == m_max_count; }
    void insert(uint32_t _key, uint64_t _address);
    uint32_t split(Leaf* _new_leaf, uint64_t _new_leaf_address);
    void setNextLeafAddress(uint64_t _next_leaf_address);

    void read(std::fstream & _input_file);
    void write(std::fstream & _output_file);

    std::string toString();

private:
    const uint16_t m_max_count;
    const uint16_t m_block_size;
    uint16_t m_real_data_size;

    // data struct to write on disk
    uint16_t m_count;
    uint32_t * m_keys;
    uint64_t * m_addresses;
    uint64_t m_next_leaf_address;
};
