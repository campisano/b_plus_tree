#include "Leaf.h"

#include <sstream>
#include <stdexcept>
//#include <iostream>

Leaf::Leaf(unsigned short int _size, unsigned short int _block_size)
{
    m_size = _size;
    m_block_size = _block_size;

    short int real_size =
        sizeof(m_count) +
        m_size * sizeof(m_keys[0]) +
        m_size * sizeof(m_pointers[0]) +
        sizeof(m_next_leaf_pointer);

    m_free_size = m_block_size - real_size;

    if(m_free_size < 0)
    {
        throw std::invalid_argument("Block size is less then real leaf size");
    }

    m_count = 0;
    m_next_leaf_pointer = -1;

    m_keys = new unsigned long int[m_size];
    m_pointers = new unsigned long long int[m_size];

/*
    std::cout << "m_count: " << sizeof(m_count) << std::endl;
    std::cout << "unsigned short int: " << sizeof(unsigned short int) << std::endl;
    std::cout << "m_keys[0]: " << sizeof(m_keys[0]) << std::endl;
    std::cout << "unsigned long int: " << sizeof(unsigned long int) << std::endl;
    std::cout << "m_pointers[0]: " << sizeof(m_pointers[0]) << std::endl;
    std::cout << "unsigned long long int: " << sizeof(unsigned long long int) << std::endl;
    std::cout << "m_next_leaf_pointer: " << sizeof(m_next_leaf_pointer) << std::endl;
    std::cout << "unsigned long long int: " << sizeof(unsigned long long int) << std::endl;

    std::cout << "total: " << sizeof(m_count) +
        m_size * sizeof(m_keys[0]) +
        m_size * sizeof(m_pointers[0]) +
        sizeof(m_next_leaf_pointer) << std::endl;
*/
}

Leaf::~Leaf()
{
    delete m_pointers;
    delete m_keys;

    m_count = 0;
    m_next_leaf_pointer = -1;
}

void Leaf::insert(unsigned long int _key, unsigned long long int _pointer)
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
            throw std::runtime_error("Unordered insertion not implemented");
            //shift all remainig left items
            // ex:
            // m_keys[pos + 1] = m_keys[pos];
            // m_pointers[pos + 1] = m_pointers[pos];
            // break;
        }
    }

    m_keys[pos] = _key;
    m_pointers[pos] = _pointer;
    ++m_count;
}

void Leaf::split(Leaf* _new_leaf, unsigned long long int _new_leaf_pointer)
{
    if(m_count != m_size)
    {
        throw std::invalid_argument("Input leaf must be full");
    }

    if(_new_leaf->m_count != 0)
    {
        throw std::invalid_argument("Output leaf must be empty");
    }
                                               // ex 7 = 13 / 2 + 1
    unsigned short int half = m_count / 2 + 1; // ex 5 = 9 / 2 + 1

    for(unsigned short int i = half; i < m_count; ++i)
    {
        _new_leaf->m_keys[i - half] = m_keys[i];
        _new_leaf->m_pointers[i - half] = m_pointers[i];
    }
                                     // ex 6 = 13 - 7
    _new_leaf->m_count = m_count - half; // ex 4 = 9 - 5
    m_count = half; // ex 7 // ex 5

    _new_leaf->m_next_leaf_pointer = m_next_leaf_pointer;
    m_next_leaf_pointer = _new_leaf_pointer;
}

void Leaf::setNextLeafPointer(unsigned long long int _next_leaf_pointer)
{
    m_next_leaf_pointer = _next_leaf_pointer;
}

void Leaf::readLeaf(FILE * _input_file)
{
    fread(&m_count, sizeof(m_count), 1, _input_file);
    fread(&m_keys, sizeof(m_keys[0]), m_size, _input_file);
    fread(&m_pointers, sizeof(m_pointers[0]), m_size, _input_file);
    fread(&m_next_leaf_pointer, sizeof(m_next_leaf_pointer), 1, _input_file);

    fseek(_input_file, m_free_size, SEEK_CUR);
}

void Leaf::writeLeaf(FILE * _output_file)
{
    fwrite(&m_count, sizeof(m_count), 1, _output_file);
    fwrite(&m_keys, sizeof(m_keys[0]), m_size, _output_file);
    fwrite(&m_pointers, sizeof(m_pointers[0]), m_size, _output_file);
    fwrite(&m_next_leaf_pointer, sizeof(m_next_leaf_pointer), 1, _output_file);

    fseek(_output_file, m_free_size, SEEK_CUR);
}

std::string Leaf::toString()
{
    std::stringstream ss;

    ss << '\t' << '\t' << "count: " << m_count << std::endl;

    for(unsigned short pos = 0; pos < m_count; ++pos)
    {
        ss << '\t' << '\t' << '\t' << "key: " << m_keys[pos] << " pointer: " << m_pointers[pos] << std::endl;
    }

    ss << '\t' << '\t' << "next_pointer: " << m_next_leaf_pointer << std::endl;

    return ss.str();
}
