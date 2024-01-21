#include <iostream>
#include <cassert>
#include "ChunkList.hpp"

int main() {
    fefu_laboratory_two::Chunk<int> chunk1(5);
    fefu_laboratory_two::Chunk<int> chunk2(3);

    chunk2[0] = 1;
    chunk2[1] = 2;
    chunk2[2] = 3;

    fefu_laboratory_two::ChunkList_iterator<int> iter1(&chunk1, 2);
    fefu_laboratory_two::ChunkList_iterator<int> iter2(&chunk2, 1);
    fefu_laboratory_two::ChunkList_iterator<int> iter3(&chunk1, 4);

    ++iter1;
    --iter2;
    iter1++;
    iter2++;

    fefu_laboratory_two::ChunkList_iterator<int> iter_sum = iter1 + 2;

//    assert(iter1 != iter2);
//    assert(iter1 < iter3);
//    assert(iter3 > iter2);
//    assert(iter3 >= iter2);
//    assert(iter2 <= iter3);
//    assert(iter1[2] == iter2[1]);

    fefu_laboratory_two:: ChunkList_iterator<int> iter_copy = iter1;

    iter1 += 2;
    iter_sum -= 2;

    return 0;
}
