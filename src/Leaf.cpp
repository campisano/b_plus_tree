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

void Leaf::insert(uint32_t _key, uint64_t _address)
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
            std::stringstream ss;
            ss << "Duplicated Key: " << _key << " at address " << _address;
            throw std::invalid_argument(ss.str());
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

void Leaf::setNextLeafAddress(uint64_t _next_leaf_address)
{
    m_next_sibling_address = _next_leaf_address;
}

std::string Leaf::toString()
{
    std::stringstream ss;
    ss << '\t' << '\t' << "count: " << m_count << std::endl;

    for(uint16_t pos = 0; pos < m_count; ++pos)
    {
        ss << '\t' << '\t' << '\t' << "key: <" << m_keys[pos] << "> address: " << m_addresses[pos] << std::endl;
    }

    ss << '\t' << '\t' << "next_address: " << m_next_sibling_address << std::endl;

    return ss.str();
}

uint64_t Leaf::getAddressForKey(std::fstream & _fs_index, uint32_t _key)
{
    if(m_count == 0)
    {
        throw std::logic_error("Cannot find any key: Leaf is empty");
    }

    for(uint16_t i = 0; i < m_count; ++i)
    {
        if (m_keys[i] == _key)
        {
            return m_addresses[i];
        }
    }

    throw std::out_of_range("Cannot find requested key");
}
