#include "Node.h"

#include <sstream>
#include <stdexcept>

#include "Leaf.h"

// remove all new

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
//#include <iostream>

void Node::insert(uint32_t _new_key, uint64_t _new_address)
{
    // first subnode
    if(m_count == 0)
    {
        m_keys[0] = _new_key;
        m_leaf_addresses[0] = m_fs_index.tellp();

        Leaf new_leaf(m_max_count, m_block_size);
        new_leaf.insert(_new_key, _new_address);
        new_leaf.append(m_fs_index);

        m_count = 1;

        //TODO write node
    }
    // if there are leafs
    else if(m_count > 0)
    {
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
                m_leaf_addresses[0] = m_fs_index.tellp();

                // insert new leaf at start, configuring next leaf as current
                Leaf new_leaf(m_max_count, m_block_size);
                new_leaf.setNextLeafAddress(m_leaf_addresses[1]); // link with the old first
                new_leaf.insert(_new_key, _new_address);
                new_leaf.append(m_fs_index);

                ++m_count;

                //TODO write node
            }
        }
        else
        {
            // find the correct leaf
            for(uint16_t pos = 0; pos < m_count; ++pos)
            {
                // node key must be less or equal then next sub node (or leaf) key
                if(m_keys[pos] <= _new_key)
                {
                    // if keys are equal
                    if((m_keys[pos] == _new_key) ||
                       // or there is no next key
                       (pos + 1 == m_count) ||
                       // or next key is greather then new key
                       (m_keys[pos + 1] > _new_key))
                    {
                        Leaf leaf(m_max_count, m_block_size);
                        leaf.loadAt(m_fs_index, m_leaf_addresses[pos]);

                        // then insert if not empty
                        if(!leaf.isEmpty())
                        {
                            leaf.insert(_new_key, _new_address);
                            leaf.update(m_fs_index, m_leaf_addresses[pos]);

                            return;
                        }
                        // else split and insert
                        else
                        {
                            if(m_count == m_max_count)
                            {
                                // split node and rotate //TODO [CMP] not implemented
                                throw std::out_of_range("Node is full, split not implemented");
                            }
                            else
                            {

                                Leaf split_leaf(m_max_count, m_block_size);
                                uint32_t split_leaf_first_key = leaf.split(split_leaf);
                                split_leaf.setNextLeafAddress(m_leaf_addresses[pos + 1]); // link with the previous successor

                                // shift all elements to next pos
                                for(uint16_t rpos = m_count; rpos > pos; --rpos)
                                {
                                    m_keys[rpos] = m_keys[rpos - 1];
                                    m_leaf_addresses[rpos] = m_leaf_addresses[rpos - 1];
                                }

                                if(_new_key < split_leaf_first_key )
                                {
                                    uint32_t first_key = leaf.insert(_new_key, _new_address);
                                    m_keys[pos] = first_key; // updating, may be needed
                                }
                                else
                                {
                                    split_leaf_first_key = split_leaf.insert(_new_key, _new_address); // updating split key, may be needed
                                }

                                m_keys[pos + 1] = split_leaf_first_key;
                                m_leaf_addresses[pos + 1] = m_fs_index.tellp();
                                split_leaf.append(m_fs_index); // append new

                                leaf.setNextLeafAddress(m_leaf_addresses[pos + 1]); // link with the old first
                                leaf.update(m_fs_index, m_leaf_addresses[pos]); // save old splitted

                                ++m_count;

                                //TODO write node?

                                return;
                            }
                        }
                    }
                }
            }

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
                        Leaf last_leaf(m_max_count, m_block_size);
                        last_leaf.loadAt(m_fs_index, m_leaf_addresses[m_count - 1]);
                        last_leaf.setNextLeafAddress(m_fs_index.tellp());
                        last_leaf.update(m_fs_index, m_leaf_addresses[m_count - 1]);
                    }

                    m_keys[m_count] = _new_key;
                    m_leaf_addresses[m_count] = m_fs_index.tellp();

                    Leaf new_leaf(m_max_count, m_block_size);
                    new_leaf.insert(_new_key, _new_address);
                    new_leaf.append(m_fs_index);

                    ++m_count;
                }
            }

            //TODO write node?
        }
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
        leaf.readToNext(m_fs_index);

        ss << '\t' << "--------------------------------------------------" << std::endl;
        ss << '\t' << '\t' << "Leaf key: " << m_keys[i] << ", creation id: " << 1 + m_leaf_addresses[i] / m_block_size << ", index address: " << m_leaf_addresses[i] << std::endl;
        ss << leaf.toString();
    }

    m_fs_index.seekg(pos); // restore original position

    return ss.str();
}
