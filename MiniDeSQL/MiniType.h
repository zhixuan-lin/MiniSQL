#ifndef MINISQL_MINITYPE_H
#define MINISQL_MINITYPE_H


#include <string>
#include <iostream>
#include <utility>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <map>

namespace MINI_TYPE
{
    // nameing conventions
    inline std::string TableFileName(const std::string & table_name) {return table_name;}
    inline std::string IndexFileName(const std::string & index_name) {return index_name;}
    inline std::string IndexName(const std::string & table_name, const std::string & attribute_name) {return table_name + "_" + attribute_name;}
    
    
    const int BlockSize = 4096;
    const int MaxBlocks = 128;
    const char Empty = 0;
    const char NonEmpty = 1;
    const int MaxChar = 256;
    
	enum TypeId
	{
		MiniInt,
    	MiniFloat,
    	MiniChar
	};

	enum CommandType {
	    CreateTableCmd,    // arg: TableInfo
	    DropTableCmd,      // arg: TableName
	    CreateIndexCmd,    // arg: IndexInfo
	    DropIndexCmd,      // arg: IndexName
	    SelectCmd,         // arg: TableName, CondArray
	    InsertCmd,         // arg: TableName, ValueArray
	    DeleteCmd,         // arg: TableName, CondArray
	    QuitCmd,           // arg:
	    ExecfileCmd        // arg: FileName
	};

	enum Operator {
	    Equal,
	    NotEqual,
	    GreaterThan,
	    LessThan,
	    GreaterEqual,
	    LessEqual
	};

	struct SqlValueType
	{
        SqlValueType() {}
		SqlValueType(TypeId id, size_t c_size=MaxChar) : type(id), char_size(c_size) {};
		TypeId type;
		std::size_t TypeSize() const;
        int BPTreeDegree() const;
		std::size_t char_size;

		bool operator==(const SqlValueType &s) const;

        bool operator!=(const SqlValueType &s) const;
	};

	struct SqlValue
	{
	public:
        int i;
        float f;
        std::string str;

		SqlValueType type;
        SqlValue() {}
        SqlValue(TypeId t, int i_val) : type(t), i(i_val) {};
        SqlValue(TypeId t, float f_val) : type(t), f(f_val) {};

		SqlValue(SqlValueType t, int i_val) : type(t), i(i_val) {};
		SqlValue(SqlValueType t, float f_val) : type(t), f(f_val) {};
		SqlValue(SqlValueType t, std::string str_val) : type(t), str(std::move(str_val)) {};
		std::string ToStr() const;
		void ReadFromMemory(const char * source, int byte_offset);
		void WriteToMemory(char * dest, int byte_offset) const;
		bool operator<(const SqlValue & s) const;
		bool operator<=(const SqlValue & s) const;
		bool operator>(const SqlValue & s) const;
		bool operator>=(const SqlValue & s) const;
		bool operator==(const SqlValue & s) const;
		bool operator!=(const SqlValue & s) const;
    };

	struct Attribute
	{
        Attribute() : unique(false), primary(false) {}
		Attribute(std::string nm, SqlValueType t, bool is_primary=false, bool is_unique=false)
			: name(std::move(nm)), type(t), primary(is_primary), unique(is_unique){}
		std::string name;
		SqlValueType type;
		bool primary;
		bool unique;
    };

	
	struct TableInfo
	{
        TableInfo() = default;
        TableInfo(const std::string nm, const std::vector<Attribute> attrs);
		std::string name;
		std::string primaryKey;
		std::vector<Attribute> attributes;
		int record_length = 0;
		int record_count = 0;
		std::map<std::string, std::string> indices;
        Attribute FetchAttribute(const std::string & attribute_name);
    };
    
   
    struct IndexInfo {
		IndexInfo() {};

        IndexInfo(std::string table, std::string attribute) :
                name(IndexName(table, attribute)), table(table), attribute(attribute), alias("NULLALIAS") {};
        IndexInfo(std::string table, std::string attribute, std::string alias) :
                name(IndexName(table, attribute)), table(table), attribute(attribute), alias(std::move(alias)) {};
		std::string name;
		std::string alias;
		std::string table;
		std::string attribute;
	};
	
    struct Record
    {
    	Record() {};

        explicit Record(std::vector<SqlValue> sqlValue) : values(std::move(sqlValue)) {};
        std::vector<SqlValue> values;
        // Make the record satisfy the table scheme
        void Conform(const TableInfo & table);
        // Extract values with indices specifed
        Record Extract(const std::vector<int> indices) const;
        // Extract the "index" the value
        Record Extract(int index) const;
        Record Extract(const TableInfo & table, const std::vector<std::string> & attributes) const;
        Record Extract(const TableInfo & table, const std::string & attribute) const;
    };
    
    struct Condition;
	struct Table
	{
		Table (){}
		Table (TableInfo i) : info(i) {}
		Table (TableInfo i, std::vector<Record> r) : info(i), records(r) {}
        Table Select(const std::vector<Condition> & conditions);
		TableInfo info;
		std::vector<Record> records;
		void DisplayAttr(std::vector<std::string> attrList);
    };

	struct Condition
	{
        Condition(){}
		Condition(std::string attr, Operator ope, SqlValue val) : op(ope), value(val), attributeName(attr) {}

		bool Test(SqlValue val) const;
		Operator op;
		SqlValue value;
		std::string attributeName;
    };

    struct SqlCommand {
        CommandType commandType;
        TableInfo tableInfo;
        IndexInfo indexInfo;
        std::string tableName;
        std::string indexName;
        std::vector<Condition> condArray;
        std::vector<SqlValue> valueArray;
        std::vector<std::string> attrList;
    };

    class SyntaxError : public std::exception {
    public:
        explicit SyntaxError(std::string errMessage) : errMessage(std::move(errMessage)) {};
        
        const char *what() const noexcept override {
            return errMessage.c_str();
        }
        
    private:
        std::string errMessage;
    };

    inline bool IsValidString(const size_t charSize) {
        return charSize >= 1 && charSize <= 255;
    }
    
    // multi-condition test
    bool Test(const std::vector<Condition> & conditions, const TableInfo & table, const Record & record);
    bool Test(Condition & condition, const TableInfo & table, const Record & record);
    
    // Operator getter
    Operator GetOpFromString(const std::string opStr);
    
    
    // overloaded stream operations
    std::ostream &operator<<(std::ostream &out, const SqlValueType & sqlValueType);
    std::ostream &operator<<(std::ostream &out, const SqlValue & value);
    std::ostream &operator<<(std::ostream &out, const TableInfo tableInfo);
    std::ostream &operator<<(std::ostream &out, const Record & record);
    std::ostream &operator<<(std::ostream &out, const Attribute & attribute);
    std::ostream &operator<<(std::ostream &out, const std::pair<std::string, std::string> index);
    std::ostream &operator<<(std::ostream &out, const Table & table);
	std::ostream &operator<<(std::ostream &out, const IndexInfo & indexInfo);
    
    std::istream &operator>>(std::istream &in, Attribute &attribute);
    std::istream &operator>>(std::istream &in, std::pair<std::string, std::string> &index);
    std::istream &operator>>(std::istream &in, TableInfo &tableInfo);

	std::istream &operator>>(std::istream &in, IndexInfo &indexInfo);
    std::istream &operator>>(std::istream &in, SqlValueType &sqlValueType);
}


#endif //MINISQL_MINITYPE_H
