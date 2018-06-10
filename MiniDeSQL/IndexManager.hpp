//
//  IndexManager.hpp
//  MiniSQL
//
//  Created by lin on 09/06/2018.
//  Copyright © 2018 lin. All rights reserved.
//

#ifndef IndexManager_hpp
#define IndexManager_hpp
#include <utility>
#include "MiniType.h"
#include "BPTree.h"
#include <map>
#include <stdio.h>


class IndexManager {
public:
	class iterator
	{
	public:
        
		iterator(NodeSearchParse<MINI_TYPE::SqlValue>);
		void operator++(int);
        std::pair<MINI_TYPE::SqlValue, int> operator*();
        bool operator==(iterator i) { return search_node == i.search_node; }
        bool operator!=(iterator i) { return not(search_node == i.search_node); }
	private:
		NodeSearchParse<MINI_TYPE::SqlValue> search_node;
    };

    static iterator end;
    
	void CreateIndex(const string & index_name, const MINI_TYPE::SqlValueType & type);

	void DropIndex(const string &index_name);

	iterator Find(const string &index_name, const MINI_TYPE::SqlValue &vals);

    iterator Begin(const string &index_name);

    iterator End(const string &index_name);

    void InsertKey(const string &index_name, const MINI_TYPE::SqlValue & val, int offset);

    void RemoveKey(const string &index_name, const MINI_TYPE::SqlValue & val);
private:
    

    template <typename T>
    using Trees = std::map<std::string, BPTree<T> *>;
	
	Trees<MINI_TYPE::SqlValue> trees;

	Trees<MINI_TYPE::SqlValue>::iterator FindIndex(const string &index_name);
};
#endif /* IndexManager_hpp */
