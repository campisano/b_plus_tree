#define __LEAF_H__

class Leaf
{
public:
    explicit Leaf(unsigned short int _size);
    virtual ~Leaf();

    inline bool isEmpty() { return m_count == m_size; }
    void insert(long int _key, unsigned long long int _pointer);

private:
    unsigned short int m_size;
    unsigned short int m_count;
    long int * m_keys;
    unsigned long long int * m_pointers;
    Leaf * m_next;
};
