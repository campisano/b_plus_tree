#include "Node.h"

#include <sstream>
#include <stdexcept>

#include "Leaf.h"

Node::Node(std::fstream & _fs_index, const uint16_t _max_count, const uint16_t _block_size) :
    IndexDataStructure(_max_count, _block_size), m_fs_index(_fs_index)
{
    hasLeafs(false); // default
}

Node::~Node()
{
    hasLeafs(false); // default
}

bool Node::hasLeafs()
{
    if(m_next_sibling_address -1)
    {
        return false;
    }
    else if(m_next_sibling_address == 1)
    {
        return true;
    }

    throw std::logic_error("node has leaf or not is not defined");
}

void Node::hasLeafs(bool _has_leafs)
{
    if(_has_leafs)
    {
        m_next_sibling_address = 1;
    }
    else
    {
        m_next_sibling_address = -1;
    }
}

bool Node::insert(uint32_t _new_key, uint64_t _new_address)
{
    // if is first element,
    if(m_count == 0)
    {
        // if node is empty then will be a node with leafs
        hasLeafs(true);

        m_keys[0] = _new_key;
        m_addresses[0] = m_fs_index.tellp();

        Leaf new_leaf(m_max_count, m_block_size);
        new_leaf.insert(_new_key, _new_address);
        new_leaf.writeToNext(m_fs_index);

        m_count = 1;

        // update node
        writeAt(m_fs_index, getIndexPosition());
    }
    // else there are some leafs or nodes
    else
    {
        // if first key is greather then new key
        if(m_keys[0] > _new_key)
        {
            if(m_count == m_max_count)
            {
                return false;
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

                if(hasLeafs())
                {
                    // insert new leaf at start, configuring next leaf as current
                    Leaf new_leaf(m_max_count, m_block_size);
                    new_leaf.setNextLeafAddress(m_addresses[1]); // link with the old first
                    new_leaf.insert(_new_key, _new_address);
                    new_leaf.writeToNext(m_fs_index);
                }
                else
                {
                    // insert new node at start
                    Node new_node(m_fs_index, m_max_count, m_block_size);
                    new_node.writeToNext(m_fs_index); // must be first
                    new_node.insert(_new_key, _new_address); // auto update
                }

                ++m_count;

                // update node
                writeAt(m_fs_index, getIndexPosition());
            }
        }
        else
        {
            // find the correct element
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
                        if(hasLeafs())
                        {
                            Leaf leaf(m_max_count, m_block_size);
                            leaf.loadAt(m_fs_index, m_addresses[pos]);

                            // then insert if not empty
                            if(!leaf.isEmpty())
                            {
                                leaf.insert(_new_key, _new_address);
                                leaf.writeAt(m_fs_index, m_addresses[pos]);

                                return true;
                            }
                            // else split and insert
                            else
                            {
                                if(m_count == m_max_count)
                                {
                                    return false;
                                }
                                else
                                {
                                    // get the previous successor, if split_leaf is the last one then there is no successor
                                    uint64_t previous_pos_successor = (pos + 1 == m_count) ? -1 : m_addresses[pos + 1];

                                    Leaf split_leaf(m_max_count, m_block_size);
                                    leaf.split(split_leaf);
                                    uint32_t split_leaf_first_key = split_leaf.getFirstKey();
                                    split_leaf.setNextLeafAddress(previous_pos_successor);

                                    // shift all elements to next pos
                                    for(uint16_t rpos = m_count; rpos > pos; --rpos)
                                    {
                                        m_keys[rpos] = m_keys[rpos - 1];
                                        m_addresses[rpos] = m_addresses[rpos - 1];
                                    }

                                    if(_new_key < split_leaf_first_key )
                                    {
                                        leaf.insert(_new_key, _new_address);
                                        m_keys[pos] = leaf.getFirstKey(); // updating, may be needed
                                    }
                                    else
                                    {
                                        split_leaf.insert(_new_key, _new_address); // updating split key, may be needed
                                        split_leaf_first_key = split_leaf.getFirstKey();
                                    }

                                    m_keys[pos + 1] = split_leaf_first_key;
                                    m_addresses[pos + 1] = m_fs_index.tellp();
                                    split_leaf.writeToNext(m_fs_index); // append new

                                    leaf.setNextLeafAddress(m_addresses[pos + 1]); // link with the old first
                                    leaf.writeAt(m_fs_index, m_addresses[pos]); // save old splitted

                                    ++m_count;

                                    // update node
                                    writeAt(m_fs_index, getIndexPosition());

                                    return true;
                                }
                            }
                        }
                        else
                        {
                            // horrible code copy-paste
                            Node node(m_fs_index, m_max_count, m_block_size);
                            node.loadAt(m_fs_index, m_addresses[pos]);

                            // then insert if not empty
                            if(!node.isEmpty())
                            {
                                node.insert(_new_key, _new_address); // auto update

                                return true;
                            }
                            // else split and insert
                            else
                            {
                                if(m_count == m_max_count)
                                {
                                    return false;
                                }
                                else
                                {
                                    // not duplicated code!
                                    // split
                                    Node split_node(m_fs_index, m_max_count, m_block_size);
                                    split_node.hasLeafs(node.hasLeafs());
                                    node.split(split_node);

                                    // write
                                    node.writeAt(m_fs_index, node.getIndexPosition()); //update
                                    split_node.writeToNext(m_fs_index); // append new

                                    // shift all elements to next pos
                                    for(uint16_t rpos = m_count; rpos > pos; --rpos)
                                    {
                                        m_keys[rpos] = m_keys[rpos - 1];
                                        m_addresses[rpos] = m_addresses[rpos - 1];
                                    }

                                    uint32_t split_node_first_key = split_node.getFirstKey();

                                    if(_new_key < split_node_first_key )
                                    {
                                        node.insert(_new_key, _new_address);
                                        m_keys[pos] = node.getFirstKey(); // updating, may be needed
                                    }
                                    else
                                    {
                                        split_node.insert(_new_key, _new_address); // updating split key, may be needed
                                        split_node_first_key = split_node.getFirstKey();
                                    }

                                    node.writeAt(m_fs_index, node.getIndexPosition()); // save old splitted
                                    split_node.writeAt(m_fs_index, split_node.getIndexPosition()); // update

                                    m_keys[pos + 1] = split_node_first_key;
                                    m_addresses[pos + 1] = split_node.getIndexPosition();

                                    ++m_count;

                                    // update node
                                    writeAt(m_fs_index, getIndexPosition());

                                    return true;
                                }
                            }
                        }
                    }
                }
            }

            // if code reach this point, insert new leaf or node at end
            {
                if(m_count == m_max_count)
                {
                    return false;
                }
                else
                {
                    if(hasLeafs())
                    {
                        // link previous with new leaf
                        {
                            Leaf last_leaf(m_max_count, m_block_size);
                            last_leaf.loadAt(m_fs_index, m_addresses[m_count - 1]);
                            last_leaf.setNextLeafAddress(m_fs_index.tellp());
                            last_leaf.writeAt(m_fs_index, m_addresses[m_count - 1]);
                        }
                    }

                    m_keys[m_count] = _new_key;
                    m_addresses[m_count] = m_fs_index.tellp();

                    if(hasLeafs())
                    {
                        Leaf new_leaf(m_max_count, m_block_size);
                        new_leaf.insert(_new_key, _new_address);
                        new_leaf.writeToNext(m_fs_index);
                    }
                    else
                    {
                        Node new_node(m_fs_index, m_max_count, m_block_size);
                        new_node.writeToNext(m_fs_index); // must be first
                        new_node.insert(_new_key, _new_address); // auto update
                    }

                    ++m_count;
                }
            }

            // update node
            writeAt(m_fs_index, getIndexPosition());
        }
    }

    return true;
}

// current node is the new_root_node, _old_root_to_split is the old root _new_sibling_node is the new sibling node
void Node::splitOldRootAndRotate(Node & _old_root_to_split, uint32_t _new_key, uint64_t _new_address)
{
    if(_old_root_to_split.m_count != m_max_count)
    {
        throw std::invalid_argument("_old_root_to_split must be full");
    }

    if(m_count != 0)
    {
        throw std::invalid_argument("new_root_node must be empty");
    }

    // split
    Node _new_sibling_node(m_fs_index, m_max_count, m_block_size);
    _new_sibling_node.hasLeafs(_old_root_to_split.hasLeafs()); // if his brother has leafs, new brother will have leafs
    _old_root_to_split.split(_new_sibling_node);

    // save changes
    _old_root_to_split.writeAt(m_fs_index, _old_root_to_split.getIndexPosition()); // update old root
    _new_sibling_node.writeToNext(m_fs_index); // append new sibling

    if(_new_key < _new_sibling_node.getFirstKey())
    {
        // now must can insert, or something is wrong
        if(!_old_root_to_split.insert(_new_key, _new_address))
        {
            throw std::logic_error("this must not happen");
        }
    }
    else
    {
        // now must can insert, or something is wrong
        if(!_new_sibling_node.insert(_new_key, _new_address))
        {
            throw std::logic_error("this must not happen");
        }
    }

    // rotate, this node is the new node, will ever have only 2 child nodes at this point, index 0 and 1
    if(_old_root_to_split.getFirstKey() < _new_sibling_node.getFirstKey())
    {
        m_keys[0] = _old_root_to_split.getFirstKey();
        m_addresses[0] = _old_root_to_split.getIndexPosition();
        m_keys[1] = _new_sibling_node.getFirstKey();
        m_addresses[1] = _new_sibling_node.getIndexPosition();
    }
    else if(_old_root_to_split.getFirstKey() > _new_sibling_node.getFirstKey())
    {
        m_keys[0] = _new_sibling_node.getFirstKey();
        m_addresses[0] = _new_sibling_node.getIndexPosition();
        m_keys[1] = _old_root_to_split.getFirstKey();
        m_addresses[1] = _old_root_to_split.getIndexPosition();
    }
    else
    {
        throw std::logic_error("this must not happen");
    }

    m_count = 2;
    writeToNext(m_fs_index); // append this new root
}

std::string Node::toString()
{
    std::stringstream ss;

    std::streampos pos = m_fs_index.tellg(); // remember the file position

    ss << '\t' << "Node, has leaf: " << (hasLeafs() ? "true" : "false") << ", creation id: " << 1 + getIndexPosition() / m_block_size << ", index address: " << getIndexPosition() << std::endl;

    if(!hasLeafs())
    {
        ss << '\t' << "node count: " << m_count << std::endl;

        Node node(m_fs_index, m_max_count, m_block_size);

        for(uint16_t i = 0; i < m_count; ++i)
        {
            m_fs_index.seekg(m_addresses[i]); // go to node position on the index file
            node.readToNext(m_fs_index);

            ss << "''''''''''''''''''''''''''''''''''''''''''''''''" << std::endl;
            ss << "Node key: <" << m_keys[i] << '>' << std::endl;
            ss << "''''''''''''''''''''''''''''''''''''''''''''''''" << std::endl;
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
            ss << '\t' << '\t' << "Leaf key: <" << m_keys[i] << ">, creation id: " << 1 + m_addresses[i] / m_block_size << ", index address: " << m_addresses[i] << std::endl;
            ss << leaf.toString();
        }
    }

    m_fs_index.seekg(pos); // restore original position

    return ss.str();
}

uint64_t Node::getAddressForKey(std::fstream & _fs_index, uint32_t _key)
{
    if(m_count == 0)
    {
        throw std::logic_error("Cannot find any key: Node is empty");
    }

    if(_key < m_keys[0])
    {
        std::stringstream ss;
        ss << "Cannot find a key minor then first node key: " << _key << " < " << m_keys[0];
        throw std::invalid_argument(ss.str());
    }

    IndexDataStructure * ids;

    if(hasLeafs())
    {
        ids = new Leaf(m_max_count, m_block_size);
    }
    else
    {
        ids = new Node(m_fs_index, m_max_count, m_block_size);
    }

    uint64_t key;
    bool found = false;

    // find between first to last but one key
    for(uint16_t i = 0; i < (m_count - 1); ++i)
    {
        if(_key >= m_keys[i] && _key < m_keys[i + 1])
        {
            found = true;
            ids->loadAt(_fs_index, m_addresses[i]);
            key = ids->getAddressForKey(_fs_index, _key);
            break;
        }
    }

    // if not found, then must be in last position
    if(!found)
    {
        ids->loadAt(_fs_index, m_addresses[m_count - 1]);
        key = ids->getAddressForKey(_fs_index, _key);
    }

    delete ids;

    return key;
}
