#include "Leaf.h"

#include <stdexcept>


Leaf::Leaf(unsigned short int _size)
{
    m_count = 0;
    m_size = _size;
    m_next = NULL;

    m_keys = new long int[m_size];
    m_pointers = new unsigned long long int[m_size];
}

Leaf::~Leaf()
{
    delete m_pointers;
    delete m_keys;

    if(m_next != NULL)
    {
        delete m_next;
        m_next = NULL;
    }

    m_count = 0;
}

void Leaf::insert(long int _key, unsigned long long int _pointer)
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
