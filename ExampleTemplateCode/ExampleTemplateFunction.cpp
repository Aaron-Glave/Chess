// TemplateClassExample.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "TemplateHeader.h"

class A {
public:
    int a;
    A(int A_a) {
        a = A_a;
    }

};

class B {
public:
    int b, b2;
    B(int B_b) {
        b = B_b;
        b2 = B_b + 2;
    }
};


template<> void sayMyStuff<A> (const A* obj) {
    std::cout << "Class A: a = " << obj->a << std::endl;
}

template<> void sayMyStuff<B> (const B* obj) {
    std::cout << "Class B: 2b + 2 = " << obj->b + obj->b2 << std::endl;
}


int main()
{
    std::cout << "Hello World!\n";
    A my_a = A(1);
    B my_b = B(3);
    std::cout << "A: " << my_a.a << " B: " << my_b.b << std::endl;
    sayMyStuff(&my_a);
    sayMyStuff(&my_b);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
