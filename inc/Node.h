#define __NODE_H__

#include <list>
#include <string>

class Leaf;

class Node
{
public:
    explicit Node(unsigned short int _size, unsigned short int _block_size);
    virtual ~Node();

    Leaf * insert(unsigned long int _key, unsigned long long int _pointer);

    std::string toString();

private:
    unsigned short int m_size;
    unsigned short int m_block_size;

    std::list<unsigned long int> m_keys;
    std::list<Leaf *> m_leafs;
};
