#include "Leaf.h"

#include <sstream>
#include <stdexcept>

Leaf::Leaf(const uint16_t _max_count, const uint16_t _block_size) :
    m_max_count(_max_count), m_block_size(_block_size)
{
    m_count = 0;
    m_next_leaf_address = -1;

    m_keys = new uint32_t[m_max_count];
    m_addresses = new uint64_t[m_max_count];

    m_real_data_size =
        sizeof(m_count) +
        m_max_count * sizeof(m_keys[0]) +
        m_max_count * sizeof(m_addresses[0]) +
        sizeof(m_next_leaf_address);

    if(m_block_size < m_real_data_size)
    {
        std::stringstream ss;
        ss << "Block size is less then real leaf size:" << std::endl;
        ss << "Block defined size = " << m_block_size << std::endl;
        ss << "m_max_count = " << m_max_count << std::endl;
        ss << "sizeof(m_count): " << sizeof(m_count) << std::endl;
        ss << "sizeof(m_keys[0]): " << sizeof(m_keys[0]) << std::endl;
        ss << "sizeof(m_addresses[0]): " << sizeof(m_addresses[0]) << std::endl;
        ss << "m_max_count * (m_keys[0] size + m_addresses[0] size): " << m_max_count * (sizeof(m_keys[0]) + sizeof(m_addresses[0])) << std::endl;
        ss << "sizeof(m_next_leaf_address): " << sizeof(m_next_leaf_address) << std::endl;
        ss << "Total real leaf size = " << m_real_data_size << std::endl;

        throw std::invalid_argument(ss.str());
    }
}

Leaf::~Leaf()
{
    delete m_addresses;
    delete m_keys;

    m_count = 0;
    m_next_leaf_address = -1;
}

void Leaf::insert(uint32_t _key, uint64_t _address)
{
    if(isEmpty())
    {
        throw std::out_of_range("Leaf is full");
    }

    unsigned short pos;

    //find the position to insert
    for(pos = 0; pos < m_count; ++pos)
    {
        if(m_keys[pos] == _key)
        {
            throw std::invalid_argument("Duplicated Key");
        }
        else if (m_keys[pos] > _key)
        {
            // shift all elements to next pos
            for(uint16_t rpos = m_count; rpos > pos; --rpos)
            {
                m_keys[rpos] = m_keys[rpos - 1];
                m_addresses[rpos] = m_addresses[rpos - 1];
            }

            break;
        }
    }

    m_keys[pos] = _key;
    m_addresses[pos] = _address;
    ++m_count;
}

uint32_t Leaf::split(Leaf* _new_leaf, uint64_t _new_leaf_address)
{
    if(m_count != m_max_count)
    {
        throw std::invalid_argument("Input leaf must be full");
    }

    if(_new_leaf->m_count != 0)
    {
        throw std::invalid_argument("Output leaf must be empty");
    }
                                            // ex 4 = 8 / 2
    uint16_t half = m_count / 2;  // ex 6 = 13 / 2

    for(uint16_t i = half; i < m_count; ++i)
    {
        _new_leaf->m_keys[i - half] = m_keys[i];
        _new_leaf->m_addresses[i - half] = m_addresses[i];
    }
                                            // ex 4 = 8 - 4
    _new_leaf->m_count = m_count - half;    // ex 7 = 13 - 6
    m_count = half; // ex 7 // ex 5

    _new_leaf->m_next_leaf_address = m_next_leaf_address;
    m_next_leaf_address = _new_leaf_address;

    // return new leaf first key
    return _new_leaf->m_keys[0];
}

void Leaf::setNextLeafAddress(uint64_t _next_leaf_address)
{
    m_next_leaf_address = _next_leaf_address;
}

void Leaf::read(std::fstream & _input_file)
{
    _input_file.read((char *)(&m_count), sizeof(m_count));
    _input_file.read((char *)(m_keys), sizeof(m_keys[0]) * m_max_count);
    _input_file.read((char *)(m_addresses), sizeof(m_addresses[0]) * m_max_count);
    _input_file.read((char *)(&m_next_leaf_address), sizeof(m_next_leaf_address));

    // skip free block space
    _input_file.seekg(m_block_size - m_real_data_size, _input_file.cur);
}

void Leaf::write(std::fstream & _output_file)
{
    _output_file.write((char *)(&m_count), sizeof(m_count));
    _output_file.write((char *)(m_keys), sizeof(m_keys[0]) * m_max_count);
    _output_file.write((char *)(m_addresses), sizeof(m_addresses[0]) * m_max_count);
    _output_file.write((char *)(&m_next_leaf_address), sizeof(m_next_leaf_address));

    // skip free block space
    _output_file.seekg(m_block_size - m_real_data_size, _output_file.cur);
}

std::string Leaf::toString()
{
    std::stringstream ss;

    ss << '\t' << '\t' << "count: " << m_count << std::endl;

    for(uint16_t pos = 0; pos < m_count; ++pos)
    {
        ss << '\t' << '\t' << '\t' << "key: " << m_keys[pos] << " address: " << m_addresses[pos] << std::endl;
    }

    ss << '\t' << '\t' << "next_address: " << m_next_leaf_address << std::endl;

    return ss.str();
}
