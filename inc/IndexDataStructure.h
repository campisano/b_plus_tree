#ifndef __INDEX_DATA_STRUCTURE_H__
#define __INDEX_DATA_STRUCTURE_H__

#include <fstream>
#include <stdint.h>

class IndexDataStructure
{
protected:
    explicit IndexDataStructure(const uint16_t _max_count, const uint16_t _block_size);

public:
    virtual ~IndexDataStructure();

public:
    inline bool isEmpty() { return m_count == m_max_count; }
    uint32_t getFirstKey();

    void split(IndexDataStructure & _new_index_data);

    void readToNext(std::fstream & _input_file);
    void writeToNext(std::fstream & _output_file);
    void loadAt(std::fstream & _input_file, std::streampos _position);
    void writeAt(std::fstream & _output_file, std::streampos _position);
    std::streampos getIndexPosition();

    virtual uint64_t getAddressForKey(std::fstream & _fs_index, uint32_t _key) = 0;

protected:
    const uint16_t m_max_count;
    const uint16_t m_block_size;
    uint16_t m_real_data_size;

    // data struct to write on disk
    uint16_t m_count;
    uint32_t * m_keys;
    uint64_t * m_addresses;
    uint64_t m_next_sibling_address;

private:
    // auxiliary data
    bool m_is_indexed;
    std::streampos m_index_position;
};

#endif
