#include "Node.h"

#include <list>
#include <stdexcept>

#include "Leaf.h"

Node::Node(unsigned short int _size)
{
    m_size = _size;
}

Node::~Node()
{
    m_keys.clear();

    std::list<Leaf *>::iterator it;

    for(it = m_leafs.begin(); it != m_leafs.end(); ++it)
    {
        delete (*it);
    }

    m_leafs.clear();
}

Leaf * Node::insert(long int _key, unsigned long long int _pointer)
{
    std::list<long int>::iterator it_keys = m_keys.begin();
    std::list<Leaf *>::iterator it_leafs = m_leafs.begin();

    //long int key;
    Leaf * leaf;

    // find the correct and not empty leaf
    while(it_keys != m_keys.end())
    {
        //key = (*it_keys);
        leaf = (*it_leafs);

        //TODO not implemented unordered insert
        //if(_key < key)
        {
            if(!leaf->isEmpty())
            {
                leaf->insert(_key, _pointer);

                return leaf;
            }
        }

        ++it_keys;
        ++it_leafs;
    }

    leaf = new Leaf(m_size);
    leaf->insert(_key, _pointer);

    m_keys.push_back(_key);
    m_leafs.push_back(leaf);

    return leaf;
}
