// New BPTree

#ifndef MINISQL_BPTREE_H
#define MINISQL_BPTREE_H

#include <iostream>
#include <vector>
#include <string>

#include "MiniType.h"

using namespace std;

template<typename T>
class BPTreeNode {
public:
    BPTreeNode() = default;

//    BPTreeNode() {}

    BPTreeNode(int degree, bool isLeaf);

    ~BPTreeNode() {}

    bool search(const T &key, int &index) const;

    BPTreeNode *split(T &key);

    int add(const T &key);

    int add(const T &key, int offset);

    void removeAt(int index);

    bool isRoot() const { return parent == nullptr; }

    bool isLeaf;
    int degree, cnt;
    BPTreeNode *parent, *sibling;
    vector<T> keys;
    vector<int> keyOffset;
    vector<BPTreeNode<T> *> children;

    void debug(int id) {
        cout << "Keys [" << id << "]: ";
        for (int i = 0; i < this->cnt; i++) {
            cout << keys[i] << " ";
        }
        cout << endl;
    }

private:
    bool binarySearch(const T &key, int &index) const;
};

template<typename T>
BPTreeNode<T>::BPTreeNode(int degree, bool isLeaf):degree(degree), cnt(0), isLeaf(isLeaf),
                                                   parent(nullptr), sibling(nullptr) {
    keys.resize(degree);
    keyOffset.resize(degree);
    children.resize(degree + 1);
}

template<typename T>
bool BPTreeNode<T>::search(const T &key, int &index) const {
    if (cnt == 0) {
        index = 0;
        return false;
    }
    if (key < keys[0]) {
        index = 0;
        return false;
    }
    if (key > keys[cnt - 1]) {
        index = cnt;
        return false;
    }
    return binarySearch(key, index);
}

template<typename T>
bool BPTreeNode<T>::binarySearch(const T &key, int &index) const {
    int left = 0, mid, right = cnt - 1;
    while (left <= right) {
        mid = left + (right - left) / 2;
        if (key > keys[mid]) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    index = left;
    if (keys[index] == key)
        return true;
    else
        return false;
}

template<typename T>
BPTreeNode<T> *BPTreeNode<T>::split(T &key) {
    BPTreeNode<T> *NewBPTreeNode = new BPTreeNode<T>(degree, isLeaf);
    int Node2 = degree / 2;
    int Node1 = degree - Node2;

    if (isLeaf) {
        key = keys[Node1];
        for (int i = Node1; i < degree; i++) {
            NewBPTreeNode->keys[i - Node1] = keys[i];
            NewBPTreeNode->keyOffset[i - Node1] = keyOffset[i];
        }
        NewBPTreeNode->sibling = this->sibling;
        this->sibling = NewBPTreeNode;
        this->cnt = Node1;
    } else {
        key = keys[Node1];
        for (int i = Node1; i <= degree; i++) {
            NewBPTreeNode->children[i - Node1] = this->children[i];
            this->children[i]->parent = NewBPTreeNode;
            this->children[i] = nullptr;
        }
        for (int i = Node1; i < degree; i++) {
            NewBPTreeNode->keys[i - Node1] = keys[i];
        }
        this->cnt = Node1 - 1;
    }
    NewBPTreeNode->cnt = Node2;
    NewBPTreeNode->parent = this->parent;

    return NewBPTreeNode;
}

template<typename T>
int BPTreeNode<T>::add(const T &key) {
    int Pos;
    if (search(key, Pos)) {
        cerr << "Key already exist in BPTree!" << endl;
        exit(10);
    }
    for (int i = cnt; i > Pos; i--) {
        keys[i] = keys[i - 1];
        children[i + 1] = children[i];
    }
    children[Pos + 1] = nullptr;
    keys[Pos] = key;
    cnt++;
    return Pos;
}

template<typename T>
int BPTreeNode<T>::add(const T &key, int offset) {
    int Pos;
    if (search(key, Pos)) {
        cerr << "Key already exist in BPTree!" << endl;
        exit(10);
    }
    for (int i = cnt; i > Pos; i--) {
        keys[i] = keys[i - 1];
        keyOffset[i] = keyOffset[i - 1];
    }
    keyOffset[Pos] = offset;
    keys[Pos] = key;
    cnt++;

    //Test
    // cout << "##### Add #####" << endl;
    // for(int i = 0; i < cnt; i++)
    // {
    // 	cout << keys[i] << " ";
    // }
    // cout << endl << "###############" << endl;

    return Pos;
}

template<typename T>
void BPTreeNode<T>::removeAt(int index) {
    for (int i = index + 1; i < cnt; i++) {
        keys[i - 1] = keys[i];
    }
    if (isLeaf) {
        for (int i = index + 1; i < cnt; i++) {
            keyOffset[i - 1] = keyOffset[i];
        }
        keyOffset[cnt - 1] = 0;
        keys[cnt - 1] = T();
    } else {
        for (int i = index + 1; i < cnt; i++) {
            children[i] = children[i + 1];
        }
        keys[cnt - 1] = T();
        children[cnt] = nullptr;
    }
    cnt--;

    //Test
    // cout << "##### rem #####" << endl;
    // for(int i = 0; i < cnt; i++)
    // {
    // 	cout << keys[i] << " ";
    // }
    // cout << endl << "###############" << endl;
}

template<typename T>
struct NodeSearchParse {
    NodeSearchParse() : index(0), node(nullptr) {}
    NodeSearchParse(BPTreeNode<T> * nd, int idx) : node(nd), index(idx){}
    bool operator==(const NodeSearchParse & n) {return node == n.node and index == n.index; }
    int index;
    BPTreeNode<T> *node;
};

template<typename T>
class BPTree {
public:
    typedef BPTreeNode<T> *TreeNode;

    BPTree(string fileName, int sizeofKey, int degree);

    ~BPTree();

    TreeNode getHeadNode() const { return head; }

    int find(const T &key);

    NodeSearchParse<T> findNode(const T &key);

    bool insert(const T &key, int offset);

    bool remove(const T &key);

    void show(void);

private:
    string fileName;
    TreeNode root, head;
    int sizeofKey, level, keyCount, nodeCount, degree;

    bool findKeyFromNode(TreeNode node, const T &key, NodeSearchParse<T> &res);

    void cascadeInsert(TreeNode node);

    bool cascadeDelete(TreeNode node);

    bool leafmergeL1(TreeNode parent, TreeNode node, TreeNode sibling, int index);

    bool leafmergeL2(TreeNode parent, TreeNode node, TreeNode sibling, int index);

    bool leafmergeR1(TreeNode parent, TreeNode node, TreeNode sibling, int index);

    bool leafmergeR2(TreeNode parent, TreeNode node, TreeNode sibling, int index);

    bool branchmergeL1(TreeNode parent, TreeNode node, TreeNode sibling, int index);

    bool branchmergeL2(TreeNode parent, TreeNode node, TreeNode sibling, int index);

    bool branchmergeR1(TreeNode parent, TreeNode node, TreeNode sibling, int index);

    bool branchmergeR2(TreeNode parent, TreeNode node, TreeNode sibling, int index);

    void debug(TreeNode node, int id) {
        node->debug(id);
        if (!node->isLeaf) {
            for (int i = 0; i <= node->cnt; i++) {
                debug(node->children[i], i);
            }
        }
    }
};

template<typename T>
BPTree<T>::BPTree(string fileName, int sizeofKey, int degree)
        :fileName(fileName), sizeofKey(sizeofKey), degree(degree) {
    root = new BPTreeNode<T>(degree, true);
    keyCount = 0;
    nodeCount = 1;
    level = 1;
    head = root;
}

template<typename T>
BPTree<T>::~BPTree() {

}

template<typename T>
void BPTree<T>::show(void) {
    TreeNode p = head;
    cout << "keyCount = " << keyCount << endl;
    cout << "nodeCount = " << nodeCount << endl;
    while (p != nullptr) {
        cout << "cnt = " << p->cnt << " : ";
        for (int i = 0; i < p->cnt; i++) {
            cout << p->keys[i] << " ";
        }
        cout << "parent : ";
        if (p->parent) {
            for (int i = 0; i < p->parent->cnt; i++) {
                cout << p->parent->keys[i] << " ";
            }
        }
        cout << "## ";
        p = p->sibling;
    }
    cout << endl;
}

template<typename T>
int BPTree<T>::find(const T &key) {
    NodeSearchParse<T> res;
    if (!root) { return -1; }
    if (findKeyFromNode(root, key, res)) { return res.node->keyOffset[res.index]; }
    else { return NodeSearchParse<T>(); }
}

template<typename T>
NodeSearchParse<T> BPTree<T>::findNode(const T &key) {
    NodeSearchParse<T> res;
    if (root) {
        findKeyFromNode(root, key, res);
    }
    return res;
}

template<typename T>
bool BPTree<T>::findKeyFromNode(TreeNode node, const T &key, NodeSearchParse<T> &res) {
    int index;
    if (node->search(key, index)) {
        if (node->isLeaf) {
            res.index = index;
            res.node = node;
        } else {
            node = node->children[index + 1];
            while (!node->isLeaf) {
                node = node->children[0];
            }
            res.node = node;
            res.index = 0;
        }
        return true;
    } else {
        if (node->isLeaf) {
            res.node = node;
            res.index = index;
            return false;
        } else {
            return findKeyFromNode(node->children[index], key, res);
        }
    }
}

template<typename T>
bool BPTree<T>::insert(const T &key, int offset) {
    NodeSearchParse<T> res;
    if (!findKeyFromNode(root, key, res)) {
        res.node->add(key, offset);
        if (res.node->cnt == degree) {
            cascadeInsert(res.node);
        }
        keyCount++;

        TreeNode p = head;

        return true;
    } else {
        cerr << "The key already exist in BPTree !" << endl;
        return false;
    }
}

template<typename T>
void BPTree<T>::cascadeInsert(TreeNode node) {
    T key;
    TreeNode sibling = node->split(key);
    this->nodeCount++;

    if (node->isRoot()) {
        TreeNode NewRoot = new BPTreeNode<T>(degree, false);
        this->root = NewRoot;
        node->parent = NewRoot;
        sibling->parent = NewRoot;
        NewRoot->add(key);
        NewRoot->children[0] = node;
        NewRoot->children[1] = sibling;
        this->level++;
        this->nodeCount++;
    } else {
        int index = node->parent->add(key);
        node->parent->children[index + 1] = sibling;
        sibling->parent = node->parent;
        if (node->parent->cnt == degree) {
            cascadeInsert(node->parent);
        }
    }
}

template<typename T>
bool BPTree<T>::remove(const T &key) {
    NodeSearchParse<T> res;
    if (!root) {
        cerr << "Empty tree ! " << endl;
        return false;
    }
    if (findKeyFromNode(root, key, res)) {
        if (res.node == root) {
            res.node->removeAt(res.index);
            keyCount--;
            return cascadeDelete(res.node);
        } else {
            if (res.index == 0 && head != res.node) {
                int index;
                TreeNode currentParent = res.node->parent;
                bool keyFound = currentParent->search(key, index);
                while (!keyFound) {
                    if (!currentParent->parent) { break; }
                    currentParent = currentParent->parent;
                    keyFound = currentParent->search(key, index);
                }
                currentParent->keys[index] = res.node->keys[1];
                res.node->removeAt(res.index);
                keyCount--;
                return cascadeDelete(res.node);
            } else {
                res.node->removeAt(res.index);
                keyCount--;
                return cascadeDelete(res.node);
            }
        }
    } else {
        cerr << "Can't find the key !" << endl;
        return false;
    }
}

template<typename T>
bool BPTree<T>::cascadeDelete(TreeNode node) {
    int minbranch = degree - degree / 2 - 1;
    int minleaf = (degree - 1) - (degree - 1) / 2;
    if ((node->isRoot() && node->cnt) || (node->isLeaf && node->cnt >= minleaf) ||
        (!node->isRoot() && !node->isLeaf && node->cnt >= minleaf)) {
        return true;
    }

    if (node->isRoot()) {
        if (root->isLeaf) {
            root = nullptr;
            head = nullptr;
        } else {
            root = node->children[0];
            root->parent = nullptr;
        }
        delete node;
        nodeCount--;
        level--;
        return true;
    }

    if (node->isLeaf) {
        TreeNode parent = node->parent, sibling;
        int index;
        parent->search(node->keys[0], index);
        if (parent->children[0] != node && parent->cnt == index + 1) {
            sibling = parent->children[index];
            if (sibling->cnt > minleaf) {
                return leafmergeR2(parent, node, sibling, index);
            } else {
                return leafmergeR1(parent, node, sibling, index);
            }
        } else {
            if (parent->children[0] == node) {
                sibling = parent->children[1];
            } else {
                sibling = parent->children[index + 2];
            }
            if (sibling->cnt > minleaf) {
                return leafmergeL2(parent, node, sibling, index);
            } else {
                return leafmergeL1(parent, node, sibling, index);
            }
        }
    } else {
        TreeNode parent = node->parent, sibling;
        int index;
        parent->search(node->children[0]->keys[0], index);
        if (parent->children[0] != node && parent->cnt == index + 1) {
            sibling = parent->children[index];
            if (sibling->cnt > minbranch) {
                return branchmergeR2(parent, node, sibling, index);
            } else {
                return branchmergeR1(parent, node, sibling, index);
            }
        } else {
            if (parent->children[0] == node) {
                sibling = parent->children[1];
            } else {
                sibling = parent->children[index + 2];
            }
            if (sibling->cnt > minbranch) {
                return branchmergeL2(parent, node, sibling, index);
            } else {
                return branchmergeL1(parent, node, sibling, index);
            }
        }
    }
}

template<typename T>
bool BPTree<T>::leafmergeR1(TreeNode parent, TreeNode node, TreeNode sibling, int index) {
    int Cnt2 = sibling->cnt, Cnt1 = node->cnt;
    for (int i = 0; i < Cnt1; i++) {
        sibling->keys[Cnt2 + i] = node->keys[i];
        sibling->keyOffset[Cnt2 + i] = node->keyOffset[i];
        sibling->cnt++;
    }
    parent->children[parent->cnt] = nullptr;
    sibling->sibling = nullptr;
    nodeCount--;
    parent->cnt--;
    delete node;

    return cascadeDelete(parent);
}

template<typename T>
bool BPTree<T>::leafmergeR2(TreeNode parent, TreeNode node, TreeNode sibling, int index) {
    T key = sibling->keys[sibling->cnt - 1];
    int offset = sibling->keyOffset[sibling->cnt - 1];
    sibling->cnt--;
    parent->keys[parent->cnt - 1] = key;
    node->add(key, offset);

    return true;
}

template<typename T>
bool BPTree<T>::leafmergeL1(TreeNode parent, TreeNode node, TreeNode sibling, int index) {
    T key = sibling->keys[0];
    int Cnt1 = node->cnt, Cnt2 = sibling->cnt;

    for (int i = 0; i < Cnt2; i++) {
        node->keys[Cnt1 + i] = sibling->keys[i];
        node->keyOffset[Cnt1 + i] = sibling->keyOffset[i];
    }
    if (node == parent->children[0]) {
        parent->removeAt(0);
    } else {
        parent->removeAt(index + 1);
    }
    node->sibling = sibling->sibling;
    node->cnt = Cnt1 + Cnt2;
    delete sibling;
    nodeCount--;

    return cascadeDelete(parent);
}

template<typename T>
bool BPTree<T>::leafmergeL2(TreeNode parent, TreeNode node, TreeNode sibling, int index) {
    T key = sibling->keys[0];
    int Cnt1 = node->cnt;

    node->keys[Cnt1] = sibling->keys[0];
    node->keyOffset[Cnt1] = sibling->keyOffset[0];
    node->cnt++;
    if (parent->children[0] == node) {
        parent->keys[0] = sibling->keys[0];
    } else {
        parent->keys[index + 1] = sibling->keys[0];
    }
    sibling->removeAt(0);

    return true;
}

template<typename T>
bool BPTree<T>::branchmergeR1(TreeNode parent, TreeNode node, TreeNode sibling, int index) {
    int Cnt1 = node->cnt, Cnt2 = sibling->cnt;
    T key;
    TreeNode N = node;
    while (!N->isLeaf) N = N->children[0];
    sibling->keys[Cnt2] = N->keys[0];
    sibling->children[Cnt2 + 1] = node->children[0];
    node->children[0]->parent = sibling;
    sibling->cnt++;
    for (int i = 0; i < Cnt1; i++) {
        node->children[i + 1]->parent = sibling;
        sibling->children[Cnt2 + i + 2] = node->children[i + 1];
        sibling->keys[Cnt2 + i + 1] = node->keys[i];
        sibling->cnt++;
    }
    nodeCount--;
    parent->removeAt(index);
    delete node;

    return cascadeDelete(parent);
}

template<typename T>
bool BPTree<T>::branchmergeR2(TreeNode parent, TreeNode node, TreeNode sibling, int index) {
    int Cnt1 = node->cnt, Cnt2 = sibling->cnt;
    T key = sibling->keys[Cnt2 - 1];
    node->add(key);
    node->children[0] = sibling->children[Cnt2];
    sibling->children[Cnt2]->parent = node;
    sibling->children[Cnt2] = nullptr;
    sibling->cnt--;
    TreeNode N = node;
    while (!N->isLeaf) N = N->children[0];
    parent->keys[index] = N->keys[0];

    return true;
}

template<typename T>
bool BPTree<T>::branchmergeL1(TreeNode parent, TreeNode node, TreeNode sibling, int index) {
    int Cnt1 = node->cnt, Cnt2 = sibling->cnt;
    T key;
    TreeNode N = sibling;
    while (!N->isLeaf) N = N->children[0];
    node->keys[Cnt1] = N->keys[0];
    node->children[Cnt1 + 1] = sibling->children[0];
    sibling->children[0]->parent = node;
    node->cnt++;
    for (int i = 0; i < Cnt2; i++) {
        sibling->children[i + 1]->parent = node;
        node->children[Cnt1 + i + 2] = sibling->children[i + 1];
        node->keys[Cnt1 + i + 1] = sibling->keys[i];
        node->cnt++;
    }
    nodeCount--;
    if (node == parent->children[0]) {
        parent->removeAt(0);
    } else {
        parent->removeAt(index + 1);
    }
    delete sibling;

    return cascadeDelete(parent);
}

template<typename T>
bool BPTree<T>::branchmergeL2(TreeNode parent, TreeNode node, TreeNode sibling, int index) {
    sibling->children[0]->parent = node;
    node->children[node->cnt + 1] = sibling->children[0];
    node->keys[node->cnt] = sibling->children[0]->keys[0];
    node->cnt++;

    if (node == parent->children[0]) {
        parent->keys[0] = sibling->keys[0];
    } else {
        parent->keys[index + 1] = sibling->keys[0];
    }

    sibling->children[0] = sibling->children[1];
    sibling->removeAt(0);
    return true;
}

#endif