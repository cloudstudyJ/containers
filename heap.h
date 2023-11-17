#pragma once

#include "../utilities/typeHandler.h"

/*      ! TODO
    1. copy operator
    2. move operator
    3. operator[]
    4. iterator
    5. peek()
*/

struct Maximum{ };
struct Minimum{ };

template <typename T, typename PRIORITY = Maximum>
class Heap {
    using uint8  = unsigned char;
    using uint16 = unsigned short;
    using uint32 = unsigned int;

    private:
        class Node {
            public:
                Node();
                Node(const T& _data);
                Node(T&& _data) noexcept;
                ~Node() noexcept;

                Node& operator=(const T& _data);
                Node& operator=(T&& _data) noexcept;

                void swapData(Node& other);

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

        inline uint32 size() const;
        inline uint16 level() const;

        inline bool isEmpty() const;

    private:
        Node* getNodeAt(const uint32& idx) const;

        void pushUpdate(uint32 idx);
        void popUpdate();

    private:
        uint32 mSize{ };
        uint8 mLevel{ };

        Node*** mRoot = new Node**[32];
};

/* Node Definitions */
template <typename T, typename PRIORITY>
Heap<T, PRIORITY>::Node::Node() { }
template <typename T, typename PRIORITY>
Heap<T, PRIORITY>::Node::Node(const T& _data) { *this = _data; }
template <typename T, typename PRIORITY>
Heap<T, PRIORITY>::Node::Node(T&& _data) noexcept { *this = move(_data); }
template <typename T, typename PRIORITY>
Heap<T, PRIORITY>::Node::~Node() noexcept { }

template <typename T, typename PRIORITY>
typename Heap<T, PRIORITY>::Node& Heap<T, PRIORITY>::Node::operator=(const T& _data) {
    if (data != _data)
        data = _data;

    return *this;
}
template <typename T, typename PRIORITY>
typename Heap<T, PRIORITY>::Node& Heap<T, PRIORITY>::Node::operator=(T&& _data) noexcept {
    if (data != _data)
        data = move(_data);

    return *this;
}

template <typename T, typename PRIORITY>
void Heap<T, PRIORITY>::Node::swapData(Node& other) {
    if (this != &other) {
        T temp = move(data);

        data = move(other.data);
        other.data = move(temp);
    }
}

/* Heap Definitions */
template <typename T, typename PRIORITY>
Heap<T, PRIORITY>::Heap() { }
template <typename T, typename PRIORITY>
Heap<T, PRIORITY>::~Heap() noexcept {
    clear();

    delete[] mRoot;
}

/**
 * @brief
 *     idx에 해당하는 노드의 값 (데이터) 을 반환한다.
 *     idx는 0-based 를 사용한다.
 *
 * @param idx index 값
 *
 * @return T& : 해당 노드 값 (데이터) 의 참조
 */
template <typename T, typename PRIORITY>
T& Heap<T, PRIORITY>::operator[](const uint32& idx) { return getNodeAt(idx + 1)->data; }
template <typename T, typename PRIORITY>
const T& Heap<T, PRIORITY>::operator[](const uint32& idx) const { return getNodeAt(idx + 1)->data; }

template <typename T, typename PRIORITY>
Heap<T, PRIORITY>::operator bool() const { return !isEmpty(); }

/**
 * @brief
 *     데이터를 추가한다.
 *     root index는 0부터 시작한다. (0-based)
 *
 * @param data 추가할 데이터
 */
template <typename T, typename PRIORITY>
void Heap<T, PRIORITY>::push(const T& data) {
    if (mSize < 0xFFFFFFFF) {
        uint32 lvlMaxCapacity = 1 << mLevel;

        if ((++mSize) == lvlMaxCapacity)
            mRoot[mLevel++] = new Node*[lvlMaxCapacity]{ };

        uint8 actualLevel = mLevel - 1;
        uint32 levelIndex = mSize - (1 << actualLevel);

        mRoot[actualLevel][levelIndex] = new Node(data);
        pushUpdate((1 << actualLevel) + levelIndex);
    }
}
/**
 * @brief
 *     데이터를 삭제한다.
 */
template <typename T, typename PRIORITY>
void Heap<T, PRIORITY>::pop() {
    if (!isEmpty()) {
        uint8     actualLevel = mLevel - 1;
        uint32 lvlMaxCapacity = 1 << actualLevel;

        delete mRoot[0][0];
        mRoot[0][0] = mRoot[actualLevel][mSize - lvlMaxCapacity];
        mRoot[actualLevel][mSize - lvlMaxCapacity] = { };

        if (mLevel && (mSize == lvlMaxCapacity))
            delete[] mRoot[--mLevel];

        --mSize;
        popUpdate();
    }
}
/**
 * @brief
 *     할당된 모든 데이터를 삭제한다.
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
inline typename Heap<T, PRIORITY>::uint32 Heap<T, PRIORITY>::size() const { return mSize; }
/**
 * @brief
 *     트리의 레벨을 출력한다. (root는 레벨 0)
 *
 * @return Heap<T, PRIORITY>::uint16 : 트리의 레벨 값
 */
template <typename T, typename PRIORITY>
inline typename Heap<T, PRIORITY>::uint16 Heap<T, PRIORITY>::level() const { return static_cast<uint16>((mLevel != 0 ? (mLevel - 1) : 0)); }

template <typename T, typename PRIORITY>
inline bool Heap<T, PRIORITY>::isEmpty() const { return (mSize == 0); }

/**
 * @brief
 *     idx 값에 해당하는 노드를 반환한다.
 *     idx는 1-based 를 사용한다.
 *
 * @param idx 얻고자 하는 노드의 index
 *
 * @return Node* : idx 위치의 노드 포인터
 */
template <typename T, typename PRIORITY>
typename Heap<T, PRIORITY>::Node* Heap<T, PRIORITY>::getNodeAt(const uint32& idx) const {
    // idx interval checking? -> effect to operator[]
    uint8 level{ };

    while (static_cast<uint32>(1 << level) <= idx)
        ++level;

    uint8 actualLevel = level - 1;

    return mRoot[actualLevel][idx - (1 << (actualLevel))];
}

/**
 * @brief
 *     템플릿 매개변수 PRIORITY 정렬 방식에 따라 추가한 데이터를 업데이트한다.
 *     idx는 1-based 를 사용한다.
 *
 * @param idx 추가한 데이터의 index
 */
template <typename T, typename PRIORITY>
void Heap<T, PRIORITY>::pushUpdate(uint32 idx) {
    Node* current = getNodeAt(idx);
    Node*  parent{ };

    while (idx > 1) {
        parent = getNodeAt(idx % 2 ? ((idx + 1) / 2) - 1 : (idx + 1) / 2);

        if (isSame<PRIORITY, Maximum> ? (current->data > parent->data) : (current->data < parent->data))
            current->swapData(*parent);

        else
            break;

        current = parent;
        idx /= 2;
    }
}
template <typename T, typename PRIORITY>
void Heap<T, PRIORITY>::popUpdate() {
    uint32 idx = 1;

    Node*  current = getNodeAt(idx);
    Node* compNode{ };
    Node*    right{ };
    Node*     left{ };

    while ((idx * 2) <= mSize) {
        compNode = current;
           right = getNodeAt(idx * 2 + 1);
            left = getNodeAt(idx * 2);

        if (right != nullptr && (isSame<PRIORITY, Maximum> ? (compNode->data < right->data) : (compNode->data > right->data)))
            compNode = right;

        if (left != nullptr && (isSame<PRIORITY, Maximum> ? (compNode->data < left->data) : (compNode->data > left->data)))
            compNode = left;

        if (compNode != current) {
            current->swapData(*compNode);

            current = compNode;
            idx = (compNode == left ? (idx * 2) : (idx * 2 + 1));
        }
        else
            break;
    }
}