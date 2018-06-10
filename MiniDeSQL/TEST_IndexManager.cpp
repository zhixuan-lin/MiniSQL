
#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include "BufferManager.h"
#include "IndexManager.hpp"
#include "MiniType.h"

int main()
{
    IndexManager im;
    MINI_TYPE::SqlValueType type(MINI_TYPE::MiniInt);
    im.CreateIndex("hi", type);
    const int N = 5;
    std::array<MINI_TYPE::SqlValue, N> arr;
    std::vector<int> vec= {3, 7, 2, 0, 5};
    int j = 0;
    for (auto & x : arr)
    {
        x.type = type;
        x.i = vec[j++];
        im.InsertKey("hi", x, j + 1);
    }
    MINI_TYPE::SqlValue val;
    val.i = 3;
    val.type = type;
    for (auto iter = im.Find("hi", val); iter != im.End("hi"); iter++)
    {
        std::cout << (*iter).first.i << " ";
    }
    
}
