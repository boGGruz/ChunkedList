#include "gtest/gtest.h"
#include "../ChunkList/ChunkList.hpp"

using namespace fefu_laboratory_two;

TEST(ChunkListTest, PushBack) {
    ChunkList<int, 5, Allocator<int>> chunkList;
    chunkList.push_back(1);
    chunkList.push_back(2);
    ASSERT_EQ(chunkList.size(), 2);
    ASSERT_EQ(chunkList.front(), 1);
    ASSERT_EQ(chunkList.back(), 2);
}

TEST(ChunkListTest, CopyConstructorWithAllocator) {
    Allocator<int> my_allocator;
    ChunkList<int, 4> source_list;
    for (int i = 0; i < 6; i++) {
        source_list.push_back(i);
    }

    ChunkList<int, 4, Allocator<int>> destination_list(source_list, my_allocator);

    ASSERT_EQ(source_list.size(), destination_list.size());

    for (int i = 0; i < source_list.size(); i++) {
        ASSERT_EQ(source_list[i], destination_list[i]);
    }
}

TEST(ChunkListTest, Assign) {
    Allocator<int> my_allocator;
    ChunkList<int, 4> my_list(6, 7, my_allocator);
    my_list.assign(4, 2);
    ASSERT_EQ(4, my_list.size());
    for (int i = 0; i < my_list.size(); i++) {
        ASSERT_EQ(2, my_list[i]);
    }
}

TEST(ChunkListTest, At) {
    ChunkList<int, 3> my_list;
    for (int i = 0; i < 5; i++) {
        my_list.push_back(i);
    }

    ASSERT_EQ(2, my_list.at(2));
    ASSERT_EQ(4, my_list.at(4));
}
TEST(ChunkListTest, ModifiedBracketsTest) {
    ChunkList<int, 3> custom_list;
    for (int custom_index = 0; custom_index < 30; custom_index++) {
        custom_list.push_back(custom_index);
    }
    for (int custom_index = 0; custom_index < 30; custom_index++) {
        ASSERT_EQ(custom_index, custom_list[custom_index]);
    }
}

TEST(ChunkListTest, ModifiedClearTest) {
    ChunkList<int, 8> custom_list;
    for (int custom_value = 6; custom_value < 74; custom_value++) {
        custom_list.push_back(custom_value);
    }
    custom_list.clear();
    ASSERT_TRUE(custom_list.empty());
}

TEST(ChunkListTest, ModifiedFrontTest) {
    ChunkList<int, 8> custom_list;
    for (int custom_value = 6; custom_value < 74; custom_value++) {
        custom_list.push_back(custom_value);
    }
    ASSERT_EQ(6, custom_list.front());
}

TEST(ChunkListTest, ModifiedBackTest) {
    ChunkList<int, 8> custom_list;
    for (int custom_value = 6; custom_value < 74; custom_value++) {
        custom_list.push_back(custom_value);
    }
    ASSERT_EQ(73, custom_list.back());
}
TEST(ChunkListTest, ModifiedIteratorsTest) {
    ChunkList<int, 8> custom_list;
    int* custom_array = new int[10];
    for (int custom_index = 0; custom_index < 10; custom_index++) {
        custom_list.push_back(custom_index);
        custom_array[custom_index] = custom_index;
    }
    auto custom_it1 = custom_list.begin();
    auto custom_it2 = custom_list.begin();
    int j =0;
    for (auto custom_iter : custom_list)
        ASSERT_TRUE(custom_array[j++] == custom_iter);
    ASSERT_TRUE(custom_it1 == custom_it2);
    ASSERT_TRUE(custom_it1 >= custom_it2);
    custom_it2++;
    ASSERT_TRUE(custom_it2 > custom_it1);
    custom_it1 += 2;
    custom_it2 -= 1;
    ASSERT_TRUE(custom_it1 > custom_it2);
    auto custom_it3 = custom_list.back();
    ASSERT_EQ(9, custom_it3);
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
