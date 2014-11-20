#include "Node.h"

#include <sstream>
#include <stdexcept>

#include "Leaf.h"

Node::Node(std::fstream & _fs_index, const uint16_t _max_count, const uint16_t _block_size) :
    m_fs_index(_fs_index), m_max_count(_max_count), m_block_size(_block_size)
{
    m_count = 0;

    m_keys = new uint32_t[m_max_count - 1];
    m_leaf_addresses = new uint64_t[m_max_count];

    m_real_data_size =
        sizeof(m_count) +
        (m_max_count - 1) * sizeof(m_keys[0]) +
        m_max_count * sizeof(m_leaf_addresses[0]);

    if(m_block_size < m_real_data_size)
    {
        std::stringstream ss;
        ss << "Block size is less then real leaf size:" << std::endl;
        ss << "Block defined size = " << m_block_size << std::endl;
        ss << "m_max_count = " << m_max_count << std::endl;
        ss << "sizeof(m_count): " << sizeof(m_count) << std::endl;
        ss << "sizeof(m_keys[0]): " << sizeof(m_keys[0]) << std::endl;
        ss << "sizeof(m_leaf_addresses[0]): " << sizeof(m_leaf_addresses[0]) << std::endl;
        ss << "m_max_count * sizeof(m_keys[0]) + m_max_count * sizeof(m_leaf_addresses[0]): " << m_max_count * sizeof(m_keys[0]) + m_max_count * sizeof(m_leaf_addresses[0]) << std::endl;
        ss << "Total real leaf size = " << m_real_data_size << std::endl;

        throw std::invalid_argument(ss.str());
    }
}

Node::~Node()
{
    delete m_leaf_addresses;
    delete m_keys;

    m_count = 0;
}


// TODO [CMP]
// - implementar a criação de nodes, que cresca para cima
#include <iostream>

void Node::insert(uint32_t _new_key, uint64_t _new_address)
{
    // first subnode
    if(m_count == 0)
    {
        m_keys[0] = _new_key;
        m_leaf_addresses[0] = m_fs_index.tellp();

        Leaf * new_leaf = new Leaf(m_max_count, m_block_size);
        new_leaf->insert(_new_key, _new_address);
        new_leaf->write(m_fs_index);

        m_count = 1;
    }
    // if there are leafs
    else if(m_count > 0)
    {
        /*
        // if first key is greather then new key
        if(m_keys[0] > _new_key)
        {
            if(m_count == m_max_count)
            {
                // split node and rotate //TODO [CMP] not implemented
                throw std::out_of_range("Node is full, split not implemented");
            }
            else
            {
                // shift all elements to next pos
                for(uint16_t rpos = m_count; rpos > 0; --rpos)
                {
                    m_keys[rpos] = m_keys[rpos - 1];
                    m_leaf_addresses[rpos] = m_leaf_addresses[rpos - 1];
                }

                m_keys[0] = _new_key;
                m_leaf_addresses[0] = ftell(m_fs_index);

                // insert new leaf at start, configuring next leaf as current
                Leaf * new_leaf = new Leaf(m_max_count, m_block_size);
                new_leaf->setNextLeafAddress(m_leaf_addresses[1]); // link with the old first
                new_leaf->insert(_new_key, _new_address);
                new_leaf->write(m_fs_index);

                ++m_count;
            }

            //TODO write node?

            return;
        }
        */
/*
        uint32_t key;
        Leaf * leaf;
        std::list<uint32_t>::iterator it_next_key;
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

                        return;
                    }
                    // else split and insert
                    else
                    {
                        Leaf * new_leaf = new Leaf(m_max_count, m_block_size);

                        // TODO [CMP] leaf address to emulate next_leaf disk reference is the new_leaf_first_key, temporary
                        uint32_t new_leaf_first_key = leaf->split(new_leaf, 0); // TODO [CMP] 0 is temporary
                        leaf->setNextLeafAddress(new_leaf_first_key);

                        if(m_count == m_max_count)
                        {
                            // split node and rotate //TODO [CMP] not implemented
                            throw std::out_of_range("Node is full, split not implemented");
                        }
                        else
                        {
                            m_keys.insert(it_next_key, new_leaf_first_key);
                            m_leafs.insert(it_next_leafs, new_leaf);
                        }

                        if(_new_key < new_leaf_first_key )
                        {
                            leaf->insert(_new_key, _new_address);
                        }
                        else
                        {
                            new_leaf->insert(_new_key, _new_address);
                        }

                        //TODO write node?

                        return;
                    }
                }
            }

            ++it_keys;
            ++it_leafs;
        }
*/
        // if code reach this point, insert new leaf at end
        {
            if(m_count == m_max_count)
            {
                // split node and rotate //TODO [CMP] not implemented
                throw std::out_of_range("Node is full, split not implemented");
            }
            else
            {
                // link previous with new leaf
                {
                    // remember the file position
                    std::streampos next_r_pos = m_fs_index.tellg();
                    std::streampos next_w_pos = m_fs_index.tellp();

                    // go to last leaf read position
                    m_fs_index.seekg(m_leaf_addresses[m_count - 1]);
                    Leaf last_leaf(m_max_count, m_block_size);
                    last_leaf.read(m_fs_index);
                    last_leaf.setNextLeafAddress(next_w_pos);

                    // go to last leaf write position
                    m_fs_index.seekp(m_leaf_addresses[m_count - 1]);
                    last_leaf.write(m_fs_index);

                    // restore original position
                    m_fs_index.seekg(next_r_pos);
                    m_fs_index.seekp(next_w_pos);
                }

                m_keys[m_count] = _new_key;
                m_leaf_addresses[m_count] = m_fs_index.tellp();

                Leaf * new_leaf = new Leaf(m_max_count, m_block_size);
                new_leaf->insert(_new_key, _new_address);
                new_leaf->write(m_fs_index);

                ++m_count;
            }
        }

        //TODO write node?

        return;
    }
}

std::string Node::toString()
{
    std::stringstream ss;

    ss << '\t' << "Node, leaf count: " << m_count << std::endl;

    std::streampos pos = m_fs_index.tellg(); // remember the file position
    Leaf leaf(m_max_count, m_block_size);

    for(uint16_t i = 0; i < m_count; ++i)
    {
        m_fs_index.seekg(m_leaf_addresses[i]); // go to leaf position on the index file
        leaf.read(m_fs_index);

        ss << '\t' << "------------------------------" << std::endl;
        ss << '\t' << '\t' << "Leaf key: " << m_keys[i] << std::endl;
        ss << '\t' << '\t' << "Leaf address: " << m_leaf_addresses[i] << std::endl;
        ss << leaf.toString();
    }

    m_fs_index.seekg(pos); // restore original position

    return ss.str();
}
