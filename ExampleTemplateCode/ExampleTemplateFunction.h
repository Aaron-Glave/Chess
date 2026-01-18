#pragma once
#include <iostream>

template <class T> void sayMyStuff(const T* obj);

template<class T>
inline void sayMyStuff(const T* obj)
{
    std::cout << "I don't know this type!" << std::endl;
}
