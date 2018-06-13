#ifndef MINISQL_MINITYPE_H
#define MINISQL_MINITYPE_H


#include <string>
#include <iostream>
#include <utility>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <map>

namespace MINI_TYPE
{
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

	enum Command {
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
        SqlValue(TypeId t, std::string str_val, int c_size = MaxChar) : type(t, c_size), str(str_val) {};

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
		std::vector<Attribute> attributes;
		int record_length = 0;
		int record_count = 0;
		std::map<std::string, std::string> indices;
        Attribute FetchAttribute(const std::string & attribute_name);
    };
    
   
    struct IndexInfo {
		std::string name;
		std::string table;
		std::string attribute;
		int attribute_idx;
	};
	
    struct Record
    {
        std::vector<SqlValue> values;
        void Conform(const TableInfo & table);
        Record Extract(const std::vector<int> indices) const;
        Record Extract(int index) const;
        Record Extract(const TableInfo & table, const std::vector<std::string> & attributes) const;
        Record Extract(const TableInfo & table, const std::string & attribute) const;
    };
    
    struct Condition;
	struct Table
	{
		Table (TableInfo i) : info(i) {}
		Table (TableInfo i, std::vector<Record> r) : info(i), records(r) {}
        Table Select(const std::vector<Condition> & conditions);
		TableInfo info;
		std::vector<Record> records;
    };

	struct Condition
	{
        Condition(){}
		Condition(Attribute attr, Operator ope, SqlValue val) : op(ope), value(val), attribute(attr) {}
		bool Test(SqlValue val) const;
		Operator op;
		SqlValue value;
		Attribute attribute;
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
    
    // multi-condition test
    bool Test(const std::vector<Condition> & conditions, const TableInfo & table, const Record & record);
    bool Test(Condition & condition, const TableInfo & table, const Record & record);
    
    // Operator getter
    Operator GetOpFromString(const std::string opStr);
    
    // nameing conventions
    inline std::string TableFileName(const std::string & table_name) {return table_name;}
    inline std::string IndexFileName(const std::string & index_name) {return index_name;}
    inline std::string IndexName(const std::string & table_name, const std::string & attribute_name) {return table_name + "_" + attribute_name;}
    
    // overloaded stream operations
    std::ostream &operator<<(std::ostream &out, const SqlValueType & sqlValueType);
    std::ostream &operator<<(std::ostream &out, const SqlValue & value);
    std::ostream &operator<<(std::ostream &out, const TableInfo tableInfo);
    std::ostream &operator<<(std::ostream &out, const Record & record);
    std::ostream &operator<<(std::ostream &out, const Attribute & attribute);
    std::ostream &operator<<(std::ostream &out, const std::pair<std::string, std::string> index);
    std::ostream &operator<<(std::ostream &out, const Table & table);
    
    std::istream &operator>>(std::istream &in, Attribute &attribute);
    std::istream &operator>>(std::istream &in, std::pair<std::string, std::string> &index);
    std::istream &operator>>(std::istream &in, TableInfo &tableInfo);
}


#endif //MINISQL_MINITYPE_H
