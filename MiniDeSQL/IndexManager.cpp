
#include "IndexManager.hpp"
#include "MiniType.h"

IndexManager::iterator IndexManager::end = IndexManager::iterator(NodeSearchParse<MINI_TYPE::SqlValue>());
IndexManager::iterator::iterator(NodeSearchParse<MINI_TYPE::SqlValue> node) : search_node(node) {}

void IndexManager::iterator::operator++(int i)
{
    if (*this != IndexManager::end)
	{
		search_node.index++;
		if (search_node.index == search_node.node->cnt)
		{
			search_node.node = search_node.node->sibling;
			search_node.index = 0;
		}	
	}
}

std::pair<MINI_TYPE::SqlValue, int> IndexManager::iterator::operator*()
{
    if (*this == IndexManager::end or search_node.index >= search_node.node->cnt)
	{
		std::cerr << "Searching out of bound!\n";
		std::exit(0);
	}
	return std::make_pair(search_node.node->keys[search_node.index], search_node.node->keyOffset[search_node.index]);
}

void IndexManager::CreateIndex(const string & index_name, const MINI_TYPE::SqlValueType & type)
{
	auto iter = trees.find(index_name);
	if (iter != trees.end())
	{
		std::cerr << "Index already exists!\n";
		std::exit(0);
	}
    int type_size = type.TypeSize();
	int degree = type.BPTreeDegree();
	
    auto new_tree = new BPTree<MINI_TYPE::SqlValue>(index_name, type_size, degree);
	trees.insert(std::make_pair(index_name, new_tree));

}

void IndexManager::DropIndex(const string & index_name)
{
	auto iter = FindIndex(index_name);
	delete iter->second;
	trees.erase(iter);
}

IndexManager::iterator IndexManager::Find(const string &index_name, const MINI_TYPE::SqlValue &vals)
{
	auto iter = FindIndex(index_name);
	
	auto search_node = iter->second->findNode(vals);

	return iterator(search_node);
}

IndexManager::iterator IndexManager::Begin(const string &index_name)
{
	auto iter = FindIndex(index_name);

	return NodeSearchParse<MINI_TYPE::SqlValue>(iter->second->getHeadNode(), 0);
}

IndexManager::iterator IndexManager::End(const string &index_name)
{
    return IndexManager::end;
}

void IndexManager::InsertKey(const string &index_name, const MINI_TYPE::SqlValue & val, int offset)
{
	auto iter = FindIndex(index_name);
    iter->second->insert(val, offset);
}

void IndexManager::RemoveKey(const string &index_name, const MINI_TYPE::SqlValue & val)
{
	auto iter = FindIndex(index_name);
	iter->second->remove(val);
}


IndexManager::Trees<MINI_TYPE::SqlValue>::iterator IndexManager::FindIndex(const string &index_name)
{
	auto iter = trees.find(index_name);
	if (iter == trees.end())
	{
		std::cerr << "Index does not exist!\n";
        std::exit(0);
	}
	return iter;
}
