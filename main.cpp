#include <iostream>
#include <cassert>
#include "ChunkList.hpp"

using namespace fefu_laboratory_two;

int main() {
    Chunk<int> chunk1(5, Allocator<int>());
    for (int i = 0; i < 5; i++)
        chunk1[i] = i * 10;

    Chunk<int> chunk2(5, Allocator<int>());
    for (int i = 0; i < 5; i++)
        chunk2[i] = i * 20;

    ChunkList_iterator<int> iter1(&chunk1, 0, 0);

    ChunkList_iterator<int> iter2(&chunk2, 0, 0);

    assert(iter1==iter2);
//    assert(iter1!=iter2);
    std::cout << iter1.operator->();

    iter2++;
    ++iter2;

    assert(iter1 > iter2);

    iter2+=12;

    Chunk<int> *chunk = &chunk2;
    std::cout << &chunk[1];
    return 0;
}
