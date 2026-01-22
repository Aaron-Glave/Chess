#pragma once
template<typename T> class Node {
public:
    T data;
    Node* next;
    Node* prev;
    Node(const T& value) : data(value), next(nullptr), prev(nullptr) {}
};

