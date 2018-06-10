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
#include "DataStructure.h"
#include "BPTree.h"
#include <map>
#include <stdio.h>


class IndexManager {
public:
	class iterator
	{
	public:
        
		iterator(NodeSearchParse<MINISQL_BASE::SqlValue>);
		void operator++(int);
        std::pair<MINISQL_BASE::SqlValue, int> operator*();
        bool operator==(iterator i) { return search_node == i.search_node; }
        bool operator!=(iterator i) { return not(search_node == i.search_node); }
	private:
		NodeSearchParse<MINISQL_BASE::SqlValue> search_node;
    };

    static iterator end;
    
	void CreateIndex(const string & index_name, const MINISQL_BASE::SqlValueType & type);

	void DropIndex(const string &index_name);

	iterator Find(const string &index_name, const MINISQL_BASE::SqlValue &vals);

    iterator Begin(const string &index_name);

    iterator End(const string &index_name);

    void InsertKey(const string &index_name, const MINISQL_BASE::SqlValue & val, int offset);

    void RemoveKey(const string &index_name, const MINISQL_BASE::SqlValue & val);
private:
    

    template <typename T>
    using Trees = std::map<std::string, BPTree<T> *>;
	
	Trees<MINISQL_BASE::SqlValue> trees;

	Trees<MINISQL_BASE::SqlValue>::iterator FindIndex(const string &index_name);
};
#endif /* IndexManager_hpp */
