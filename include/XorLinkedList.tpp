#include <iostream>
#include <utility>
#include <cassert>
#include <stdexcept>

// Helper to XOR two pointers
template<typename Data>
Node<Data>* ptr_xor(Node<Data>* p, Node<Data>* q) {
    return reinterpret_cast<Node<Data>*>(
        reinterpret_cast<uintptr_t>(p) ^ reinterpret_cast<uintptr_t>(q)
    );
}

// initializer_list constructor
template<typename Data>
XorLinkedList<Data>::XorLinkedList(std::initializer_list<Data> list) {
    for (const auto& val : list) add_to_back(val);
}

// copy constructor
template<typename Data>
XorLinkedList<Data>::XorLinkedList(const XorLinkedList<Data>& src) {
    Node<Data>* prev = nullptr;
    Node<Data>* curr = src.head;
    while (curr) {
        add_to_back(curr->data);
        Node<Data>* next = ptr_xor<Data>(prev, curr->both);
        prev = curr;
        curr = next;
    }
}

// copy assignment
template<typename Data>
XorLinkedList<Data>& XorLinkedList<Data>::operator=(const XorLinkedList<Data>& src) {
    if (this != &src) {
        this->~XorLinkedList();
        head = nullptr;
        size = 0;
        Node<Data>* prev = nullptr;
        Node<Data>* curr = src.head;
        while (curr) {
            add_to_back(curr->data);
            Node<Data>* next = ptr_xor<Data>(prev, curr->both);
            prev = curr;
            curr = next;
        }
    }
    return *this;
}

// move constructor
template<typename Data>
XorLinkedList<Data>::XorLinkedList(XorLinkedList<Data>&& src) noexcept {
    head = src.head;
    size = src.size;
    src.head = nullptr;
    src.size = 0;
}

// move assignment
template<typename Data>
XorLinkedList<Data>& XorLinkedList<Data>::operator=(XorLinkedList<Data>&& src) noexcept {
    if (this != &src) {
        this->~XorLinkedList();
        head = src.head;
        size = src.size;
        src.head = nullptr;
        src.size = 0;
    }
    return *this;
}

// destructor
template<typename Data>
XorLinkedList<Data>::~XorLinkedList() {
    Node<Data>* curr = head;
    Node<Data>* prev = nullptr;
    while (curr) {
        Node<Data>* next = ptr_xor<Data>(prev, curr->both);
        delete curr;
        prev = curr;
        curr = next;
    }
    head = nullptr;
    size = 0;
}

// add to front
template<typename Data>
void XorLinkedList<Data>::add_to_front(const Data& data) {
    Node<Data>* node = new Node<Data>{data, head};
    if (head)
        head->both = ptr_xor<Data>(node, ptr_xor<Data>(nullptr, head->both));
    head = node;
    size++;
}

// add to back
template<typename Data>
void XorLinkedList<Data>::add_to_back(const Data& data) {
    if (!head) {
        add_to_front(data);
        return;
    }
    Node<Data>* prev = nullptr;
    Node<Data>* curr = head;
    while (ptr_xor<Data>(prev, curr->both)) {
        Node<Data>* next = ptr_xor<Data>(prev, curr->both);
        prev = curr;
        curr = next;
    }
    Node<Data>* newNode = new Node<Data>{data, curr};
    curr->both = ptr_xor<Data>(prev, newNode);
    size++;
}

// remove from front
template<typename Data>
void XorLinkedList<Data>::remove_from_front() {
    if (!head) throw std::out_of_range("List empty");
    Node<Data>* next = ptr_xor<Data>(nullptr, head->both);
    delete head;
    if (next)
        next->both = ptr_xor<Data>(nullptr, ptr_xor<Data>(head, next->both));
    head = next;
    size--;
}

// remove from back
template<typename Data>
void XorLinkedList<Data>::remove_from_back() {
    if (!head) throw std::out_of_range("List empty");
    if (size == 1) { delete head; head = nullptr; size = 0; return; }

    Node<Data>* prev = nullptr;
    Node<Data>* curr = head;
    while (ptr_xor<Data>(prev, curr->both)) {
        Node<Data>* next = ptr_xor<Data>(prev, curr->both);
        prev = curr;
        curr = next;
    }
    prev->both = ptr_xor<Data>(ptr_xor<Data>(prev->both, curr), nullptr);
    delete curr;
    size--;
}

// get by index
template<typename Data>
Data& XorLinkedList<Data>::get(int index) {
    if (index < 0 || index >= size) throw std::out_of_range("Invalid index");
    Node<Data>* prev = nullptr;
    Node<Data>* curr = head;
    for (int i = 0; i < index; i++) {
        Node<Data>* next = ptr_xor<Data>(prev, curr->both);
        prev = curr;
        curr = next;
    }
    return curr->data;
}

// operator[]
template<typename Data>
const Data& XorLinkedList<Data>::operator[](int index) const {
    return const_cast<XorLinkedList<Data>*>(this)->get(index);
}
template<typename Data>
Data& XorLinkedList<Data>::operator[](int index) {
    return get(index);
}

// count
template<typename Data>
int XorLinkedList<Data>::count() const { return size; }

// reverse
template<typename Data>
void XorLinkedList<Data>::reverse() {
    Node<Data>* prev = nullptr;
    Node<Data>* curr = head;
    Node<Data>* next;
    while (curr) {
        next = ptr_xor<Data>(prev, curr->both);
        curr->both = ptr_xor<Data>(next, prev);
        prev = curr;
        curr = next;
    }
    head = prev;
}

// iterator implementation
template<typename Data>
class XorLinkedList<Data>::Iterator {
    Node<Data>* curr;
    Node<Data>* prev;
public:
    Iterator(Node<Data>* start) : curr(start), prev(nullptr) {}
    bool operator!=(const Iterator& other) const { return curr != other.curr; }
    Iterator& operator++() {
        Node<Data>* next = ptr_xor<Data>(prev, curr->both);
        prev = curr;
        curr = next;
        return *this;
    }
    Data& operator*() { return curr->data; }
};

template<typename Data>
typename XorLinkedList<Data>::Iterator XorLinkedList<Data>::begin() { return Iterator(head); }

template<typename Data>
typename XorLinkedList<Data>::Iterator XorLinkedList<Data>::end() { return Iterator(nullptr); }
