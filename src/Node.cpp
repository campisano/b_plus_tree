#include "Node.h"

#include <sstream>
#include <stdexcept>

#include "Leaf.h"

Node::Node(std::fstream & _fs_index, const uint16_t _max_count, const uint16_t _block_size) :
    IndexDataStructure(_max_count, _block_size), m_fs_index(_fs_index)
{
    m_next_sibling_address = -1;
}

Node::~Node()
{
    m_next_sibling_address = -1;
}

bool Node::hasLeafs()
{
    return m_next_sibling_address == ((uint64_t) -1);
}

Node * Node::insert(uint32_t _new_key, uint64_t _new_address)
{
    if(!hasLeafs())
    {
        return this;
    }

    // if is first leaf
    if(m_count == 0)
    {
        m_keys[0] = _new_key;
        m_addresses[0] = m_fs_index.tellp();

        Leaf new_leaf(m_max_count, m_block_size);
        new_leaf.insert(_new_key, _new_address);
        new_leaf.append(m_fs_index);

        m_count = 1;

        //TODO write node
    }
    // else there are some leafs
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
                    m_addresses[rpos] = m_addresses[rpos - 1];
                }

                m_keys[0] = _new_key;
                m_addresses[0] = m_fs_index.tellp();

                // insert new leaf at start, configuring next leaf as current
                Leaf new_leaf(m_max_count, m_block_size);
                new_leaf.setNextLeafAddress(m_addresses[1]); // link with the old first
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
                        leaf.loadAt(m_fs_index, m_addresses[pos]);

                        // then insert if not empty
                        if(!leaf.isEmpty())
                        {
                            leaf.insert(_new_key, _new_address);
                            leaf.update(m_fs_index, m_addresses[pos]);

                            return this;
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
                                // get the previous successor, if split_leaf is the last one then there is no successor
                                uint64_t previous_pos_successor = (pos + 1 == m_count) ? -1 : m_addresses[pos + 1];

                                Leaf split_leaf(m_max_count, m_block_size);
                                uint32_t split_leaf_first_key = leaf.split(split_leaf);
                                split_leaf.setNextLeafAddress(previous_pos_successor);

                                // shift all elements to next pos
                                for(uint16_t rpos = m_count; rpos > pos; --rpos)
                                {
                                    m_keys[rpos] = m_keys[rpos - 1];
                                    m_addresses[rpos] = m_addresses[rpos - 1];
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
                                m_addresses[pos + 1] = m_fs_index.tellp();
                                split_leaf.append(m_fs_index); // append new

                                leaf.setNextLeafAddress(m_addresses[pos + 1]); // link with the old first
                                leaf.update(m_fs_index, m_addresses[pos]); // save old splitted

                                ++m_count;

                                //TODO write node?

                                return this;
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
                        last_leaf.loadAt(m_fs_index, m_addresses[m_count - 1]);
                        last_leaf.setNextLeafAddress(m_fs_index.tellp());
                        last_leaf.update(m_fs_index, m_addresses[m_count - 1]);
                    }

                    m_keys[m_count] = _new_key;
                    m_addresses[m_count] = m_fs_index.tellp();

                    Leaf new_leaf(m_max_count, m_block_size);
                    new_leaf.insert(_new_key, _new_address);
                    new_leaf.append(m_fs_index);

                    ++m_count;
                }
            }

            //TODO write node?
        }
    }

    return this;
}

std::string Node::toString()
{
    std::stringstream ss;

    std::streampos pos = m_fs_index.tellg(); // remember the file position

    ss << '\t' << "Node, has leaf: " << (hasLeafs() ? "true" : "false");

    if(!hasLeafs())
    {
        ss << '\t' << "node count: " << m_count << std::endl;

        Node node(m_fs_index, m_max_count, m_block_size);

        for(uint16_t i = 0; i < m_count; ++i)
        {
            m_fs_index.seekg(m_addresses[i]); // go to node position on the index file
            node.readToNext(m_fs_index);

            ss << '\t' << "--------------------------------------------------" << std::endl;
            ss << '\t' << '\t' << "Node key: " << m_keys[i] << ", creation id: " << 1 + m_addresses[i] / m_block_size << ", index address: " << m_addresses[i] << std::endl;
            ss << node.toString();
        }
    }
    else
    {
        ss << '\t' << "leaf count: " << m_count << std::endl;

        Leaf leaf(m_max_count, m_block_size);

        for(uint16_t i = 0; i < m_count; ++i)
        {
            m_fs_index.seekg(m_addresses[i]); // go to leaf position on the index file
            leaf.readToNext(m_fs_index);

            ss << '\t' << "--------------------------------------------------" << std::endl;
            ss << '\t' << '\t' << "Leaf key: " << m_keys[i] << ", creation id: " << 1 + m_addresses[i] / m_block_size << ", index address: " << m_addresses[i] << std::endl;
            ss << leaf.toString();
        }
    }

    m_fs_index.seekg(pos); // restore original position

    return ss.str();
}
