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
    // if there are subnodes
    if(m_keys.size() > 0)
    {
        std::list<unsigned long int>::iterator it_keys = m_keys.begin();
        std::list<Leaf *>::iterator it_leafs = m_leafs.begin();

        //TODO [CMP] test!!!
        // if first key is greather then new key
        if((*it_keys) > _key)
        {
            // insert new leaf at start, configuring next leaf as current
            Leaf * new_leaf = new Leaf(m_size, m_block_size);
            new_leaf->setNextLeafPointer((unsigned long long int) (*it_leafs));
            new_leaf->insert(_key, _pointer);

            m_keys.push_front(_key);
            m_leafs.push_front(new_leaf);

            return new_leaf;
        }

        unsigned long int key;
        Leaf * leaf;
        std::list<unsigned long int>::iterator it_next_key;

        // find the correct leaf
        while(it_keys != m_keys.end())
        {
            key = (*it_keys);
            leaf = (*it_leafs);

            // node key must be less or equal then next sub node (or leaf) key
            if(key <= _key)
            {
                it_next_key = it_keys;
                ++it_next_key;

                // if is equal
                if((key == _key) ||
                   // or there is no next key
                   (it_next_key == m_keys.end()) ||
                   // or next key is greather then new key
                   (*it_next_key) > _key)
                {
                    // then insert if not empty
                    if(!leaf->isEmpty())
                    {
                        leaf->insert(_key, _pointer);

                        return leaf;
                    }
                    // else split and insert
                    else
                    {
                        Leaf * new_leaf = new Leaf(m_size, m_block_size);

                        // TODO [CMP] leaf pointer to emulate disk reference is the memory leaf pointer, temporary
                        leaf->split(new_leaf, (unsigned long long int) new_leaf);
                        new_leaf->insert(_key, _pointer);

                        m_keys.push_back(_key);
                        m_leafs.push_back(new_leaf);

                        return new_leaf;
                    }
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

    ss << '\t' << "Node, leaf count: " << m_keys.size() << std::endl;

    std::list<unsigned long int>::iterator it_keys = m_keys.begin();
    std::list<Leaf *>::iterator it_leafs = m_leafs.begin();

    unsigned long int key;
    Leaf * leaf;

    while(it_keys != m_keys.end())
    {
        key = (*it_keys);
        leaf = (*it_leafs);

        ss << '\t' << "------------------------------" << std::endl;
        ss << '\t' << '\t' << "Leaf key: " << key << std::endl;
        ss << leaf->toString();

        ++it_keys;
        ++it_leafs;
    }

    return ss.str();
}
