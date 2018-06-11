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
		SqlValueType(TypeId id, size_t c_size=0) : type(id), char_size(c_size) {};
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
        Attribute(){}
		Attribute(std::string nm, SqlValueType t, bool is_primary=false, bool is_unique=false)
			: name(std::move(nm)), type(t), primary(is_primary), unique(is_unique){}
		std::string name;
		SqlValueType type;
		bool primary;
		bool unique;
    };

	
	struct TableInfo
	{
		std::string name;
		std::vector<Attribute> attributes;
		int record_length;
		int record_count;
		std::map<std::string, std::string> indices;
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
    };
    
	struct Table
	{
		Table (TableInfo i) : info(i) {};
		TableInfo info;
		std::vector<Record> records;
    };

	struct Condition
	{
		Condition(Attribute attr, Operator ope, SqlValue val) : op(ope), value(val), attribute(attr) {}
		bool Test(SqlValue val) const;
	private:
		Operator op;
		SqlValue value;
		Attribute attribute;
    };

    inline std::ostream &operator<<(std::ostream &out, const SqlValueType sqlValueType){
        out << sqlValueType.type << ' ' << sqlValueType.char_size;
    }

    inline std::ostream &operator<<(std::ostream &out, const Attribute attribute){
        out << attribute.name << ' ' << attribute.type << ' ' << attribute.primary << ' ' << attribute.unique;
    }

    inline std::ostream &operator<<(std::ostream &out, const std::pair<std::string, std::string> index){
        out << index.first << ' ' << index.second;
    }

    inline std::ostream &operator<<(std::ostream &out, const TableInfo tableInfo) {

        out << tableInfo.name << ' '
            << tableInfo.record_count << ' '
            << tableInfo.record_length << ' '
            << tableInfo.attributes.size() << ' '
            << tableInfo.indices.size() << ' ';

        for (auto &attribute : tableInfo.attributes)
            out << attribute << ' ';

        for (auto &index : tableInfo.indices)
            out << index << ' ';

        out << std::endl;
    }

    inline std::istream &operator>>(std::istream &in, SqlValueType &sqlValueType) {
        int sqlValueTypeInt = 0;
        in >> sqlValueTypeInt;
        switch(sqlValueTypeInt) {
            case MiniInt  : sqlValueType.type = MiniInt;   break;
            case MiniFloat: sqlValueType.type = MiniFloat; break;
            case MiniChar : sqlValueType.type = MiniChar;  break;
            default:        sqlValueType.type = MiniInt;   break;
        }

        in >> sqlValueType.char_size;
    }

    inline std::istream &operator>>(std::istream &in, Attribute &attribute){
        in >> attribute.name >> attribute.type >> attribute.primary >> attribute.unique;
    }

    inline std::istream &operator>>(std::istream &in, std::pair<std::string, std::string> &index){
        in >> index.first >> index.second;
    }

    inline std::istream &operator>>(std::istream &in, TableInfo &tableInfo) {
        int attrSize = 0;
        int idxSize = 0;

        in >> tableInfo.name >> tableInfo.record_count >> tableInfo.record_length >> attrSize >> idxSize;

        for (int i = 0; attrSize > i; i++) {
            Attribute attribute;
            in >> attribute;
            tableInfo.attributes.push_back(attribute);
        }

        for (int i = 0; idxSize > i; i++) {
            std::pair<std::string, std::string> index;
            in >> index;
            tableInfo.indices.insert(index);
        }
    }

    inline void SqlValue::ReadFromMemory(const char * source, int byte_offset)
    {
        size_t size = type.TypeSize();
        char temp[MaxChar];
    	switch(type.type)
    	{
            case MiniInt: std::memcpy(&i, source + byte_offset, size); break;
    		case MiniFloat: std::memcpy(&f, source + byte_offset, size); break;
    		case MiniChar: 
    			std::memcpy(temp, source + byte_offset, size);
    			temp[size] = '\0';
    			str = temp;
    			break;
    	}
    }
	inline void SqlValue::WriteToMemory(char * dest, int byte_offset) const
	{
        size_t size = type.TypeSize();
		switch(type.type)
    	{
    		case MiniInt: std::memcpy(dest + byte_offset, &i, size); break;
    		case MiniFloat: std::memcpy(dest + byte_offset, &f, size); break;
    		case MiniChar: std::memcpy(dest + byte_offset, str.c_str(), size); break;
    	}
	}
	inline std::string SqlValue::ToStr() const
	{
		switch(type.type)
		{
            case MiniInt: return std::to_string(i);
            case MiniFloat: return std::to_string(f);
			case MiniChar: return str;
			default:
				std::cerr << "Unknown Type!\n";
				std::exit(0);
		}
	}
	inline bool SqlValue::operator<(const SqlValue & s) const
	{
		switch(type.type)
		{
			case MiniInt: return i < s.i;
			case MiniFloat: return f < s.f;
			case MiniChar: return str < s.str;
			default:
				std::cerr << "Unknown Type!\n";
				std::exit(0);
		}
	}
	inline bool SqlValue::operator<=(const SqlValue & s) const
	{
		switch(type.type)
		{
			case MiniInt: return i <= s.i;
			case MiniFloat: return f <= s.f;
			case MiniChar: return str <= s.str;
			default:
				std::cerr << "Unknown Type!\n";
				std::exit(0);
		}
	}
	inline bool SqlValue::operator>(const SqlValue & s) const
		{ return not (*this <= s); }
	inline bool SqlValue::operator>=(const SqlValue & s) const
		{ return not (*this < s); }
	inline bool SqlValue::operator==(const SqlValue & s) const
		{ return *this <= s and *this >= s; }
	inline bool SqlValue::operator!=(const SqlValue & s) const
		{ return not (*this == s); }
	inline int SqlValueType::BPTreeDegree() const
	{
		return static_cast<int>(BlockSize / (TypeSize() + sizeof(int)));
    }
    inline size_t SqlValueType::TypeSize() const
    {
        switch(type)
        {
            case MiniInt: return sizeof(int);
            case MiniFloat: return sizeof(float);
            case MiniChar: return char_size;
        }
    }
    inline std::string TableFileName(const std::string & table_name) {return table_name;}
    inline std::string IndexFileName(const std::string & index_name) {return index_name;}
}


#endif //MINISQL_MINITYPE_H
