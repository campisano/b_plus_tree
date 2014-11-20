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

Leaf * Node::insert(unsigned long int _new_key, unsigned long long int _new_address)
{
    // if there are subnodes
    if(m_keys.size() > 0)
    {
        std::list<unsigned long int>::iterator it_keys = m_keys.begin();
        std::list<Leaf *>::iterator it_leafs = m_leafs.begin();

        //TODO [CMP] test!!!
        // if first key is greather then new key
        if((*it_keys) > _new_key)
        {
            // insert new leaf at start, configuring next leaf as current
            Leaf * new_leaf = new Leaf(m_size, m_block_size);
            // TODO [CMP] leaf address to emulate disk reference is the (*it_keys), temporary
            new_leaf->setNextLeafAddress((*it_keys));
            new_leaf->insert(_new_key, _new_address);

            m_keys.push_front(_new_key);
            m_leafs.push_front(new_leaf);

            return new_leaf;
        }

        unsigned long int key;
        Leaf * leaf;
        std::list<unsigned long int>::iterator it_next_key;
        std::list<Leaf *>::iterator it_next_leafs;

        // find the correct leaf
        while(it_keys != m_keys.end())
        {
            key = (*it_keys);
            leaf = (*it_leafs);

            // node key must be less or equal then next sub node (or leaf) key
            if(key <= _new_key)
            {
                it_next_key = it_keys;
                ++it_next_key;
                it_next_leafs = it_leafs;
                ++it_next_leafs;

                // if is equal
                if((key == _new_key) ||
                   // or there is no next key
                   (it_next_key == m_keys.end()) ||
                   // or next key is greather then new key
                   (*it_next_key) > _new_key)
                {
                    // then insert if not empty
                    if(!leaf->isEmpty())
                    {
                        leaf->insert(_new_key, _new_address);

                        return leaf;
                    }
                    // else split and insert
                    else
                    {
                        Leaf * new_leaf = new Leaf(m_size, m_block_size);

                        // TODO [CMP] leaf address to emulate next_leaf disk reference is the new_leaf_first_key, temporary
                        unsigned long int new_leaf_first_key = leaf->split(new_leaf, 0); // TODO [CMP] 0 is temporary
                        leaf->setNextLeafAddress(new_leaf_first_key);
                        m_keys.insert(it_next_key, new_leaf_first_key);
                        m_leafs.insert(it_next_leafs, new_leaf);

                        if(_new_key < new_leaf_first_key )
                        {
                            leaf->insert(_new_key, _new_address);
                        }
                        else
                        {
                            new_leaf->insert(_new_key, _new_address);
                        }

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
    new_leaf->insert(_new_key, _new_address);

    m_keys.push_back(_new_key);
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
