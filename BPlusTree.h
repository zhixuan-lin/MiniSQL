#ifndef MINISQL_BPLUSTREE_H
#define MINISQL_BPLUSTREE_H

#include <string>
#include <vector>
#include <map>
#include <cmath>

#define M 3

typedef std::string Key;
typedef int Size;

typedef struct value
{
    std::string keyword;
    std::vector<struct record*> records;
} *Value;

typedef struct record
{
    int file_id;
    int line_id;
} *Record;

typedef struct node
{
    unsigned int key_size;
    bool isLeaf;
    std::vector<Key> keys;
    std::vector<Value> values;
    std::vector<struct node*> pointers;
    struct node* parent;
    // struct node* next;
} *Node;

Node make_nonleaf_node(void);
Node make_leaf_node(void);
void make_new_root(Node left, Node right, Key val);
Node make_temp_nonleaf(Node big, Key val, Node down_ptr);
Node make_temp_leaf(Node big, Key key, Value value);
int locate_insert_index(Key key, std::vector<Key> keys, unsigned key_size);
Node locate_leaf_node(Node cur, Key key);
void insert_in_leaf(Node leaf, Key key, Value value, Size full);
void insert_in_parent(Node left_side, Node right_side, Size full, Key val);
Node get_root(Node cur);
Node general_insert(Node cur, Key key, Value value);
Value make_value(std::string keyword);
Record make_record(int file_id, int line_id);
Value search(Node root, Key key);
Node build_tree_from_records(std::map<std::string, std::vector<std::pair<int, int> > > records);


#endif //MINISQL_BPLUSTREE_H
