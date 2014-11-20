#include "Node.h"

#include <list>
#include <sstream>
#include <stdexcept>

#include "Leaf.h"

Node::Node(unsigned short int _size, unsigned short int _block_size)
{
    m_size = _size;
    m_block_size = _block_size;
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

Leaf * Node::insert(unsigned long int _key, unsigned long long int _pointer)
{
    {
        std::list<unsigned long int>::iterator it_keys = m_keys.begin();
        std::list<Leaf *>::iterator it_leafs = m_leafs.begin();

        unsigned long int key;
        Leaf * leaf;

        // find the correct leaf
        while(it_keys != m_keys.end())
        {
            key = (*it_keys);
            leaf = (*it_leafs);

            if(key < _key)
            {
                if(!leaf->isEmpty())
                {
                    leaf->insert(_key, _pointer);

                    return leaf;
                }
                else
                {
                    Leaf * new_leaf = new Leaf(m_size, m_block_size);
                    leaf->split(new_leaf);
                    new_leaf->insert(_key, _pointer);

                    m_keys.push_back(_key);
                    m_leafs.push_back(new_leaf);

                    return new_leaf;
                }
            }

            ++it_keys;
            ++it_leafs;
        }
    }

    // if code reach this point, insert new leaf at end
    Leaf * new_leaf = new Leaf(m_size, m_block_size);
    new_leaf->insert(_key, _pointer);

    m_keys.push_back(_key);
    m_leafs.push_back(new_leaf);

    return new_leaf;
}

std::string Node::toString()
{
    std::stringstream ss;

    ss << "Node, leaf count: " << m_keys.size() << std::endl << std::endl;

    std::list<unsigned long int>::iterator it_keys = m_keys.begin();
    std::list<Leaf *>::iterator it_leafs = m_leafs.begin();

    unsigned long int key;
    Leaf * leaf;

    while(it_keys != m_keys.end())
    {
        key = (*it_keys);
        leaf = (*it_leafs);

        ss << "__________________________________________________" << std::endl;
        ss << "Leaf key: " << key << std::endl;
        ss << leaf->toString();

        ++it_keys;
        ++it_leafs;
    }

    return ss.str();
}
