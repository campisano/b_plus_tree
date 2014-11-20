#define __LEAF_H__

#include <cstdio>
#include <string>

class Leaf
{
public:
    explicit Leaf(unsigned short int _size, unsigned short int _block_size);
    virtual ~Leaf();

    inline bool isEmpty() { return m_count == m_size; }
    void insert(unsigned long int _key, unsigned long long int _pointer);
    void split(Leaf* _new_leaf, unsigned long long int _new_leaf_pointer);
    void setNextLeafPointer(unsigned long long int _next_leaf_pointer);

    void readLeaf(FILE * _input_file);
    void writeLeaf(FILE * _output_file);

    std::string toString();

private:
    unsigned short int m_size;
    unsigned short int m_block_size;
    unsigned short int m_free_size;

    // data struct to write on disk
    unsigned short int m_count;
    unsigned long int * m_keys;
    unsigned long long int * m_pointers;
    unsigned long long int m_next_leaf_pointer;
};
