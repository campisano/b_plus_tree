#ifndef __INDEX_DATA_STRUCTURE_H__
#define __INDEX_DATA_STRUCTURE_H__

#include <fstream>
#include <stdint.h>

class IndexDataStructure
{
public:
    inline bool isEmpty() { return m_count == m_max_count; }

    void readToNext(std::fstream & _input_file);
    void append(std::fstream & _output_file);

protected:
    explicit IndexDataStructure(const uint16_t _max_count, const uint16_t _block_size);
    virtual ~IndexDataStructure();

    const uint16_t m_max_count;
    const uint16_t m_block_size;
    uint16_t m_real_data_size;

    // data struct to write on disk
    uint16_t m_count;
    uint32_t * m_keys;
    uint64_t * m_addresses;
    uint64_t m_next_sibling_address;
};

#endif
