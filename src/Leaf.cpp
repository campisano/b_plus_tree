#include "Leaf.h"

#include <sstream>
#include <stdexcept>

Leaf::Leaf(unsigned short int _size, unsigned short int _block_size)
{
    m_size = _size;
    m_block_size = _block_size;

    short int real_size =
        sizeof(m_count) +
        m_size * sizeof(m_keys[0]) +
        m_size * sizeof(m_addresses[0]) +
        sizeof(m_next_leaf_address);

    m_free_size = m_block_size - real_size;

    if(m_free_size < 0)
    {
        throw std::invalid_argument("Block size is less then real leaf size");
    }

    m_count = 0;
    m_next_leaf_address = -1;

    m_keys = new unsigned long int[m_size];
    m_addresses = new unsigned long long int[m_size];
}

Leaf::~Leaf()
{
    delete m_addresses;
    delete m_keys;

    m_count = 0;
    m_next_leaf_address = -1;
}

void Leaf::insert(unsigned long int _key, unsigned long long int _address)
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
            for(int rpos = m_count; rpos > pos; --rpos)
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

unsigned long int Leaf::split(Leaf* _new_leaf, unsigned long long int _new_leaf_address)
{
    if(m_count != m_size)
    {
        throw std::invalid_argument("Input leaf must be full");
    }

    if(_new_leaf->m_count != 0)
    {
        throw std::invalid_argument("Output leaf must be empty");
    }
                                            // ex 4 = 8 / 2
    unsigned short int half = m_count / 2;  // ex 6 = 13 / 2

    for(unsigned short int i = half; i < m_count; ++i)
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

void Leaf::setNextLeafAddress(unsigned long long int _next_leaf_address)
{
    m_next_leaf_address = _next_leaf_address;
}

void Leaf::readLeaf(FILE * _input_file)
{
    fread(&m_count, sizeof(m_count), 1, _input_file);
    fread(&m_keys, sizeof(m_keys[0]), m_size, _input_file);
    fread(&m_addresses, sizeof(m_addresses[0]), m_size, _input_file);
    fread(&m_next_leaf_address, sizeof(m_next_leaf_address), 1, _input_file);

    fseek(_input_file, m_free_size, SEEK_CUR);
}

void Leaf::writeLeaf(FILE * _output_file)
{
    fwrite(&m_count, sizeof(m_count), 1, _output_file);
    fwrite(&m_keys, sizeof(m_keys[0]), m_size, _output_file);
    fwrite(&m_addresses, sizeof(m_addresses[0]), m_size, _output_file);
    fwrite(&m_next_leaf_address, sizeof(m_next_leaf_address), 1, _output_file);

    fseek(_output_file, m_free_size, SEEK_CUR);
}

std::string Leaf::toString()
{
    std::stringstream ss;

    ss << '\t' << '\t' << "count: " << m_count << std::endl;

    for(unsigned short pos = 0; pos < m_count; ++pos)
    {
        ss << '\t' << '\t' << '\t' << "key: " << m_keys[pos] << " address: " << m_addresses[pos] << std::endl;
    }

    ss << '\t' << '\t' << "next_address: " << m_next_leaf_address << std::endl;

    return ss.str();
}
