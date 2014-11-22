#include "Leaf.h"

#include <sstream>
#include <stdexcept>

Leaf::Leaf(const uint16_t _max_count, const uint16_t _block_size) :
    IndexDataStructure(_max_count, _block_size)
{
    m_next_sibling_address = -1;
}

Leaf::~Leaf()
{
    m_next_sibling_address = -1;
}

uint32_t Leaf::insert(uint32_t _key, uint64_t _address)
{
    if(isEmpty())
    {
        throw std::out_of_range("Leaf is full");
    }

    uint16_t pos;

    // find the position to insert
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

    // return minor key (the first one)
    return m_keys[0];
}

uint32_t Leaf::split(Leaf & _new_leaf)
{
    if(m_count != m_max_count)
    {
        throw std::invalid_argument("Input leaf must be full");
    }

    if(_new_leaf.m_count != 0)
    {
        throw std::invalid_argument("Output leaf must be empty");
    }
                                  // ex 4 = 8 / 2
    uint16_t half = m_count / 2;  // ex 6 = 13 / 2

    for(uint16_t i = half; i < m_count; ++i)
    {
        _new_leaf.m_keys[i - half] = m_keys[i];
        _new_leaf.m_addresses[i - half] = m_addresses[i];
    }
                                           // ex 4 = 8 - 4
    _new_leaf.m_count = m_count - half;    // ex 7 = 13 - 6
    m_count = half; // ex 7 // ex 5

    _new_leaf.m_next_sibling_address = m_next_sibling_address;

    // return new leaf minor key (the first one)
    return _new_leaf.m_keys[0];
}

void Leaf::setNextLeafAddress(uint64_t _next_leaf_address)
{
    m_next_sibling_address = _next_leaf_address;
}

void Leaf::loadAt(std::fstream & _input_file, std::streampos _position)
{
    // remember the read file position
    std::streampos r_pos = _input_file.tellg();

    // go to asked leaf read position
    _input_file.seekg(_position);
    readToNext(_input_file);

    // restore original read position
    _input_file.seekg(r_pos);
}

void Leaf::update(std::fstream & _output_file, std::streampos _position)
{
    // remember the write file position
    std::streampos w_pos = _output_file.tellp();

    // go to asked write position
    _output_file.seekp(_position);
    append(_output_file);

    // restore original write position
    _output_file.seekp(w_pos);
}

std::string Leaf::toString()
{
    std::stringstream ss;
    ss << '\t' << '\t' << "count: " << m_count << std::endl;

    for(uint16_t pos = 0; pos < m_count; ++pos)
    {
        ss << '\t' << '\t' << '\t' << "key: " << m_keys[pos] << " address: " << m_addresses[pos] << std::endl;
    }

    ss << '\t' << '\t' << "next_address: " << m_next_sibling_address << std::endl;

    return ss.str();
}
