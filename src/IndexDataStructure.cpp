#include "IndexDataStructure.h"

#include <sstream>
#include <stdexcept>

IndexDataStructure::IndexDataStructure(const uint16_t _max_count, const uint16_t _block_size) :
    m_max_count(_max_count), m_block_size(_block_size)
{
    // data struct to write on disk
    {
        m_count = 0;
        m_keys = new uint32_t[m_max_count];
        m_addresses = new uint64_t[m_max_count];
        m_next_sibling_address = 0;
    }

    m_is_indexed = false;
    m_index_position = 0;

    m_real_data_size =
        sizeof(m_count) +
        m_max_count * sizeof(m_keys[0]) +
        m_max_count * sizeof(m_addresses[0]) +
        sizeof(m_next_sibling_address);

    if(m_block_size < m_real_data_size)
    {
        std::stringstream ss;
        ss << "Block size is less then real index data size:" << std::endl;
        ss << "Block defined size = " << m_block_size << std::endl;
        ss << "m_max_count = " << m_max_count << std::endl;
        ss << "sizeof(m_count): " << sizeof(m_count) << std::endl;
        ss << "sizeof(m_keys[0]): " << sizeof(m_keys[0]) << std::endl;
        ss << "sizeof(m_addresses[0]): " << sizeof(m_addresses[0]) << std::endl;
        ss << "m_max_count * sizeof(m_keys[0]) + m_max_count * sizeof(m_addresses[0]): " << m_max_count * sizeof(m_keys[0]) + m_max_count * sizeof(m_addresses[0]) << std::endl;
        ss << "sizeof(m_next_sibling_address): " << sizeof(m_next_sibling_address) << std::endl;
        ss << "Total real index data size = " << m_real_data_size << std::endl;

        throw std::invalid_argument(ss.str());
    }
}

IndexDataStructure::~IndexDataStructure()
{
    delete m_addresses;
    delete m_keys;

    m_count = 0;
}

void IndexDataStructure::split(IndexDataStructure & _new_index_data)
{
    if(m_count != m_max_count)
    {
        throw std::invalid_argument("Input index data must be full");
    }

    if(_new_index_data.m_count != 0)
    {
        throw std::invalid_argument("Output index data must be empty");
    }
                                  // ex 4 = 8 / 2
    uint16_t half = m_count / 2;  // ex 6 = 13 / 2

    for(uint16_t i = half; i < m_count; ++i)
    {
        _new_index_data.m_keys[i - half] = m_keys[i];
        _new_index_data.m_addresses[i - half] = m_addresses[i];
    }
                                                 // ex 4 = 8 - 4
    _new_index_data.m_count = m_count - half;    // ex 7 = 13 - 6
    m_count = half; // ex 7 // ex 5

    _new_index_data.m_next_sibling_address = m_next_sibling_address;
}

uint32_t IndexDataStructure::getFirstKey()
{
    if(m_count == 0)
    {
        throw std::logic_error("there is no first key, data index is empty");
    }

    // return new index data minor key (the first one)
    return m_keys[0];
}

void IndexDataStructure::readToNext(std::fstream & _input_file)
{
    m_is_indexed = true;
    m_index_position = _input_file.tellg();

    _input_file.read((char *)(&m_count), sizeof(m_count));
    _input_file.read((char *)(m_keys), sizeof(m_keys[0]) * m_max_count);
    _input_file.read((char *)(m_addresses), sizeof(m_addresses[0]) * m_max_count);
    _input_file.read((char *)(&m_next_sibling_address), sizeof(m_next_sibling_address));

    // skip free block space
    _input_file.seekg(m_block_size - m_real_data_size, _input_file.cur);
}

void IndexDataStructure::writeToNext(std::fstream & _output_file)
{
    m_is_indexed = true;
    m_index_position = _output_file.tellp();

    _output_file.write((char *)(&m_count), sizeof(m_count));
    _output_file.write((char *)(m_keys), sizeof(m_keys[0]) * m_max_count);
    _output_file.write((char *)(m_addresses), sizeof(m_addresses[0]) * m_max_count);
    _output_file.write((char *)(&m_next_sibling_address), sizeof(m_next_sibling_address));

    // skip free block space
    _output_file.seekg(m_block_size - m_real_data_size, _output_file.cur);
}

void IndexDataStructure::loadAt(std::fstream & _input_file, std::streampos _position)
{
    // remember the read file position
    std::streampos r_pos = _input_file.tellg();

    // go to asked read position
    _input_file.seekg(_position);
    readToNext(_input_file);

    // restore original read position
    _input_file.seekg(r_pos);
}

void IndexDataStructure::writeAt(std::fstream & _output_file, std::streampos _position)
{
    // remember the write file position
    std::streampos w_pos = _output_file.tellp();

    // go to asked write position
    _output_file.seekp(_position);
    writeToNext(_output_file);

    // restore original write position
    _output_file.seekp(w_pos);
}

std::streampos IndexDataStructure::getIndexPosition()
{
    if(!m_is_indexed)
    {
        throw std::logic_error("this data is not indexed");
    }

    return m_index_position;
}
