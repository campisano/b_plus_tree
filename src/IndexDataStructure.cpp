#include "IndexDataStructure.h"

#include <sstream>
#include <stdexcept>

IndexDataStructure::IndexDataStructure(const uint16_t _max_count, const uint16_t _block_size) :
    m_max_count(_max_count), m_block_size(_block_size)
{
    m_count = 0;

    m_keys = new uint32_t[m_max_count];
    m_addresses = new uint64_t[m_max_count];

    m_real_data_size =
        sizeof(m_count) +
        m_max_count * sizeof(m_keys[0]) +
        m_max_count * sizeof(m_addresses[0]) +
        sizeof(m_next_sibling_address);

    if(m_block_size < m_real_data_size)
    {
        std::stringstream ss;
        ss << "Block size is less then real leaf size:" << std::endl;
        ss << "Block defined size = " << m_block_size << std::endl;
        ss << "m_max_count = " << m_max_count << std::endl;
        ss << "sizeof(m_count): " << sizeof(m_count) << std::endl;
        ss << "sizeof(m_keys[0]): " << sizeof(m_keys[0]) << std::endl;
        ss << "sizeof(m_addresses[0]): " << sizeof(m_addresses[0]) << std::endl;
        ss << "m_max_count * sizeof(m_keys[0]) + m_max_count * sizeof(m_addresses[0]): " << m_max_count * sizeof(m_keys[0]) + m_max_count * sizeof(m_addresses[0]) << std::endl;
        ss << "sizeof(m_next_sibling_address): " << sizeof(m_next_sibling_address) << std::endl;
        ss << "Total real leaf size = " << m_real_data_size << std::endl;

        throw std::invalid_argument(ss.str());
    }
}

IndexDataStructure::~IndexDataStructure()
{
    delete m_addresses;
    delete m_keys;

    m_count = 0;
}

void IndexDataStructure::readToNext(std::fstream & _input_file)
{
    _input_file.read((char *)(&m_count), sizeof(m_count));
    _input_file.read((char *)(m_keys), sizeof(m_keys[0]) * m_max_count);
    _input_file.read((char *)(m_addresses), sizeof(m_addresses[0]) * m_max_count);
    _input_file.read((char *)(&m_next_sibling_address), sizeof(m_next_sibling_address));

    // skip free block space
    _input_file.seekg(m_block_size - m_real_data_size, _input_file.cur);
}

void IndexDataStructure::append(std::fstream & _output_file)
{
    _output_file.write((char *)(&m_count), sizeof(m_count));
    _output_file.write((char *)(m_keys), sizeof(m_keys[0]) * m_max_count);
    _output_file.write((char *)(m_addresses), sizeof(m_addresses[0]) * m_max_count);
    _output_file.write((char *)(&m_next_sibling_address), sizeof(m_next_sibling_address));

    // skip free block space
    _output_file.seekg(m_block_size - m_real_data_size, _output_file.cur);
}
