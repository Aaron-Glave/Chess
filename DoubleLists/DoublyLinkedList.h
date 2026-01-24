#pragma once
#include "Node.h"
#include <stdexcept>

/* Doubly linked lists can do anything stacks or queues can do, and more!*/
template <typename T> class DoublyLinkedList {
private:
    Node<T>* head;
    Node<T>* tail;

public:
    DoublyLinkedList() : head(nullptr), tail(nullptr) {}
    ~DoublyLinkedList() {
        Node<T>* current = head;
        while (current) {
            Node<T>* next = current->next;
            delete current;
            current = next;
        }
    }

    bool isEmpty() const {
        return head == nullptr;
    }

    void add_tail(const T& value) {
        Node<T>* newNode = new Node<T>(value);
        if (isEmpty()) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
    }

    void add_head(const T& value) {
        Node<T>* newNode = new Node<T>(value);
        if (isEmpty()) {
            head = tail = newNode;
        } else {
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
        }
    }

    T remove_tail() {
        if (isEmpty()) throw std::runtime_error("List is empty");
        Node<T>* toDelete = tail;
        T value = tail->data;
        tail = tail->prev;
        if (tail) tail->next = nullptr;
        else head = nullptr; // List is now empty
        delete toDelete;
        return value;
    }

    T remove_head() {
        if (isEmpty()) throw std::runtime_error("List is empty");
        Node<T>* toDelete = head;
        T value = head->data;
        head = head->next;
        if (head) head->prev = nullptr;
        else tail = nullptr; // List is now empty
        delete toDelete;
        return value;
    }
};