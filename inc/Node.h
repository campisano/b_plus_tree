#define __NODE_H__

#include <list>

class Leaf;

class Node
{
public:
    explicit Node(unsigned short int _size);
    virtual ~Node();

    Leaf * insert(long int _key, unsigned long long int _pointer);

private:
    unsigned short int m_size;
    std::list<long int> m_keys;
    std::list<Leaf *> m_leafs;
};
