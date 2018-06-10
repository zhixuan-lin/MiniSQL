#include "BPlusTree.h"

using namespace std;
/*
    These functions can be briefly divided into three parts:
    1) "make" functions: related to memory allocation
    2) "insert" functions: related to insertion
    3) "locate" functions: related to finding certain node
*/

/* Part One: "make" functions */

// create a new nonleaf node
Node make_nonleaf_node(void){
    Node new_node = new node;
    new_node->key_size = 0;         // used for controlling the insertion
    new_node->isLeaf = false;       // marker of leaves
    new_node->parent = NULL;        // NULL is the marker of root
    new_node->keys.resize(M+1);     // get slightly more space to avoid errors
    new_node->values.resize(M+1);
    new_node->pointers.resize(M+2);
    // new_node->next = NULL;
    return new_node;
}

// create a new leaf node
Node make_leaf_node(void){
    Node new_node = make_nonleaf_node();    // other information is the same
    new_node->isLeaf = true;                // except "isLeaf"
    return new_node;
}

// make a new root after splitting (not first time!)
void make_new_root(Node left, Node right, Key val){
    Node new_root = make_nonleaf_node();
    left->parent = new_root;                // becomes the shared parent
    right->parent = new_root;
    new_root->pointers[0] = left;           // left to the left
    new_root->pointers[1] = right;          // right to the right
    new_root->keys[0] = val;
    new_root->key_size = 1;
}

// make temp space for convenience
// when the original node is full
Node make_temp_nonleaf(Node big, Key val, Node down_ptr){
    Node temp_room = make_nonleaf_node();
    temp_room->keys = big->keys;            // copy the "big" node
    temp_room->pointers = big->pointers;
    temp_room->key_size = M-1;

    // and then insert a new element
    int index = locate_insert_index(val, temp_room->keys, temp_room->key_size);
    temp_room->keys.insert(temp_room->keys.begin()+index, val);
    temp_room->pointers.insert(temp_room->pointers.begin()+index+1, down_ptr);
    temp_room->key_size += 1;

    return temp_room;
}

// make temp space for convenience
// when the original node is full
Node make_temp_leaf(Node big, Key key, Value value){
    Node temp_room = make_leaf_node();
    temp_room->keys = big->keys;            // copy the "big" node
    temp_room->values = big->values;
    temp_room->key_size = M-1;
    insert_in_leaf(temp_room, key, value, M);
    return temp_room;
}

// create a Value for Python's interface
Value make_value(string keyword){
    Value new_value = new struct value;
    new_value->keyword = keyword;           // only keyword is needed as an id
    return new_value;
}

// create a Record for Python's interface
Record make_record(int file_id, int line_id){
    Record new_record = new struct record;
    new_record->file_id = file_id;          // file_id can be mapped to a file
    new_record->line_id = line_id;          // line_id can be mapped to a line
    return new_record;
}

/* Part Two: "insert" functions */

// all insertion starts from this function
Node general_insert(Node cur, Key key, Value value){
    cur = locate_leaf_node(cur, key);   // first find a leaf to insert
    if(cur->key_size < M-1) // if the leaf is not full
        insert_in_leaf(cur, key, value, M); // that's great! just insert it
    else{   // if full

        Node temp_leaf = make_temp_leaf(cur, key, value);   // pretend it's not full and insert

        cur->keys.assign(temp_leaf->keys.begin(), temp_leaf->keys.begin()+M/2); // get one half
        cur->values.assign(temp_leaf->values.begin(), temp_leaf->values.begin()+M/2);
        cur->key_size = M/2;

        Node sibling = make_leaf_node();    // initialize a new node to hold extra values
        // cur->next = sibling;
        sibling->parent = cur->parent;
        sibling->keys.assign(temp_leaf->keys.begin()+M/2, temp_leaf->keys.end());   // get another half
        sibling->values.assign(temp_leaf->values.begin()+M/2, temp_leaf->values.end());
        sibling->key_size = (int)ceil(M/2.0);

        Key val = sibling->keys[0]; // the first element in the right goes up
        insert_in_parent(cur, sibling, M, val); // judge if their parent is full
    }
    return get_root(cur);   // make sure that the root will not be missing
}

// used for insertion in a nonfull leaf
void insert_in_leaf(Node leaf, Key key, Value value, Size full){
    int index = locate_insert_index(key, leaf->keys, leaf->key_size);   // find the place
    leaf->keys.insert(leaf->keys.begin() + index, key); // just do it
    leaf->values.insert(leaf->values.begin() + index, value);
    leaf->key_size += 1;
}

// used for insertion and splitting in upper vertices
void insert_in_parent(Node left_side, Node right_side, Size full, Key val){
    if(left_side->parent == NULL){  // if it's root and it just can't hold them
        make_new_root(left_side, right_side, val);  // then make a new root
        return;
    }
    Node parent = left_side->parent;
    if(parent->key_size < full-1){  // or the parent is not full
        int index = locate_insert_index(val, parent->keys, parent->key_size);   // find the place
        parent->keys.insert(parent->keys.begin()+index, val);   // insert it
        parent->pointers.insert(parent->pointers.begin()+index+1, right_side);
        parent->key_size++;
        right_side->parent = parent;
    }
    else{ // or the parent is simply full
        Node temp_nonleaf = make_temp_nonleaf(parent, val, right_side); // merge them in temp space

        Key val = temp_nonleaf->keys[M/2];  // the middle one goes up
        parent->keys.assign(temp_nonleaf->keys.begin(), temp_nonleaf->keys.begin()+M/2);    // get one half
        parent->pointers.assign(temp_nonleaf->pointers.begin(), temp_nonleaf->pointers.begin()+M/2);
        parent->key_size = M/2;

        Node sibling = make_nonleaf_node(); // initialize a new one to hold elements
        sibling->keys.assign(temp_nonleaf->keys.begin()+M/2, temp_nonleaf->keys.end()); // get another half
        sibling->pointers.assign(temp_nonleaf->pointers.begin()+M/2, temp_nonleaf->pointers.end());
        sibling->key_size = (int)ceil(M/2.0) - 1;

        sibling->keys.erase(sibling->keys.begin()); // erase the first one because it's children are not leaves now!
        sibling->pointers.erase(sibling->pointers.begin());

        right_side->parent = sibling;
        insert_in_parent(parent, sibling, M, val);  // go up and repeat
    }
}

// build a new tree from records, used for search engine
Node build_tree_from_records(map<string, vector<pair<int, int> > > records){
    Node root = make_leaf_node();
    map<string, vector<pair<int, int> > >::iterator it; // iterate the records
    for(it = records.begin(); it != records.end(); it++){
        Value tree_value = make_value((*it).first); // first get the keyword
        vector<pair<int, int> >::iterator vec_it;   // and get corresponding records
        for(vec_it = (*it).second.begin(); vec_it != (*it).second.end(); vec_it++){
            Record tree_record = make_record((*vec_it).first, (*vec_it).second);
            tree_value->records.push_back(tree_record); // push single record back
        }
        root = general_insert(root, (*it).first, tree_value);   // insert new values in
    }
    return root;
}

/* Part Three: "locate" functions */

// used for update the root
Node get_root(Node cur){
    while(cur->parent)  // if it has a parent
        cur = cur->parent;
    return cur; // or it's an orphan in Les Miserables
}

// find the correct place to insert
int locate_insert_index(Key key, vector<Key> keys, unsigned key_size){
    for(int i = 0; i < key_size; i++)   // iterate all keys
        if(key < keys[i])   // if a bigger one in the front
            return i;   // be so afraid and daren't advance
    return key_size;
}

// find the leaf node to save data
Node locate_leaf_node(Node cur, Key key){
    while(cur->isLeaf == false){    // if you are still not a leaf
        int index = locate_insert_index(key, cur->keys, cur->key_size);
        cur = cur->pointers[index]; // then go down
    }
    return cur;
}

// search the correct node
Value search(Node root, Key key){
    Node leaf = locate_leaf_node(root, key);    // first locate the leaf node
    for(int i = 0; i < leaf->key_size; i++)     // iterate the keys
        if(key == leaf->keys[i])                // until find the wanted one
            return leaf->values[i];             // return it
    return NULL;                                // or return nothing
}



// This function is used only for testing.

// void print_tree(Node root){
//     queue<Node> q;
//     q.push(root);
//     while(!q.empty()){
//         Node next = q.front();
//         for(int i = 0; i < next->key_size; i++)
//             cout << next->keys[i] << ' ';
//         cout << "    ";
//         if(!next->isLeaf)
//             for(int i = 0; i < next->key_size+1; i++)
//                 q.push(next->pointers[i]);
//         q.pop();
//     }
// }