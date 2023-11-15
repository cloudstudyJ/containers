#pragma once

#include "../utilities/typeHandler.h"

struct Maximum{ };
struct Minimum{ };

template <typename T, typename PRIORITY = Maximum>
class Heap {
    using uint8  = unsigned char;
    using uint32 = unsigned int;

    private:
        class Node {
            public:
                Node();
                Node(const T& _data);
                ~Node() noexcept;

                Node& operator=(const T& _data);

            public:
                T data{ };

                Node* prev{ };
                Node* next{ };
        };

    public:
        Heap();
        ~Heap() noexcept;

        T& operator[](const uint32& idx);
        const T& operator[](const uint32& idx) const;

        explicit operator bool() const;

        void push(const T& data);
        void pop();
        void clear();

              T& peek();
        const T& peek() const;

        inline bool isEmpty() const;

    private:
        uint32 mSize{ };
        uint8 mLevel{ };

        Node*** mRoot = new Node**[32];
};

/* Node Definitions */
template <typename T, typename PRIORITY>
Heap<T, PRIORITY>::Node::Node() { }
template <typename T, typename PRIORITY>
Heap<T, PRIORITY>::Node::Node(const T& _data)
    : data{_data} { }
template <typename T, typename PRIORITY>
Heap<T, PRIORITY>::Node::~Node() noexcept { }

template <typename T, typename PRIORITY>
typename Heap<T, PRIORITY>::Node& Heap<T, PRIORITY>::Node::operator=(const T& _data) {
    data = _data;

    return *this;
}

/* Heap Definitions */
template <typename T, typename PRIORITY>
Heap<T, PRIORITY>::Heap() { }
template <typename T, typename PRIORITY>
Heap<T, PRIORITY>::~Heap() noexcept {
    clear();

    delete[] mRoot;
}

template <typename T, typename PRIORITY>
Heap<T, PRIORITY>::operator bool() const { return !isEmpty(); }

template <typename T, typename PRIORITY>
void Heap<T, PRIORITY>::push(const T& data) {
    if (mSize < 0xFFFFFFFF) {
        ++mSize;

        if (mSize == static_cast<uint32>(1 << mLevel))
            mRoot[mLevel++] = new Node*[1 << mLevel]{ };

        uint8 actualLevel = mLevel - 1;
        uint32 levelIndex = mSize - (1 << actualLevel);

        mRoot[actualLevel][levelIndex] = new Node(data);
    }
}
/**
 * @brief
 *     할당된 모든 데이터를 제거한다.
 */
template <typename T, typename PRIORITY>
void Heap<T, PRIORITY>::clear() {
    for (uint8 i = 0; i < mLevel; ++i) {
        for (uint8 j = 0; j < (1 << i); ++j)
            delete mRoot[i][j];

        delete[] mRoot[i];
    }

    mSize  = { };
    mLevel = { };
}

template <typename T, typename PRIORITY>
inline bool Heap<T, PRIORITY>::isEmpty() const { return (mSize == 0); }