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
		SqlValueType(TypeId id, size_t c_size=0) : type(id), char_size(c_size) {};
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
		std::string name;
        std::string primaryKey;
		std::vector<Attribute> attributes;
		int record_length;
		int record_count;
		std::map<std::string, std::string> indices;
        Attribute FetchAttribute(const std::string & attribute_name);
    };

    inline std::string IndexName(const std::string &table_name, const std::string &attribute_name) {
        return table_name + "_" + attribute_name;
    }
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
        Record Extract(const std::vector<int> indices) const;
        Record Extract(int index) const;
        Record Extract(const TableInfo & table, const std::vector<std::string> & attributes) const;
        Record Extract(const TableInfo & table, const std::string & attribute) const;
    };
    
	struct Table
	{
	    Table (){}
		Table (TableInfo i) : info(i) {}
		Table (TableInfo i, std::vector<Record> r) : info(i), records(r) {}
		
		TableInfo info;
		std::vector<Record> records;

		void DisplayAttr(std::vector<std::string> attrList);
    };

	struct Condition
	{
        Condition(){}
		Condition(std::string attr, Operator ope, SqlValue val) : op(ope), value(val), attributeName(attr) {}

        bool Test(SqlValue val) const {};
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

    inline bool Test(const std::vector<Condition> & conditions, const TableInfo & table, const Record & record)
    {
    	for (const auto & cond : conditions)
    		if (not cond.Test(record.Extract(table, cond.attributeName).values[0]))
    			return false;
    	return true;
    }
    inline bool Test(Condition & condition, const TableInfo & table, const Record & record)
    {
        if (not condition.Test(record.Extract(table, condition.attributeName).values[0]))
            return false;
        return true;
    }

	inline void Table::DisplayAttr(std::vector<std::string> attrList) {
		std::vector<int> attrIdx;

		for (int i = 0; info.attributes.size() > i; i++)
			if (std::find(attrList.begin(), attrList.end(), info.attributes[i].name) != attrList.end()) {
				attrIdx.push_back(i);
				std::cout << info.attributes[i].name << '\t';
			}

		std::cout << std::endl;

		for (auto &record : records) {
			for (auto &idx : attrIdx) {
				std::cout << record.values[idx].ToStr() << '\t';
			}
			std::cout << std::endl;
		}
	}

    inline std::ostream &operator<<(std::ostream &out, const Table table) {

        for (auto& attr : table.info.attributes)
            out << attr.name << '\t';

        out << std::endl;

        for (auto& record : table.records) {
            for (auto &value : record.values) {
                out << value.ToStr() << '\t';
            }
            out << std::endl;
        }

        return out;
    }

    inline std::ostream &operator<<(std::ostream &out, const IndexInfo indexInfo) {
        out << indexInfo.name << ' ' << indexInfo.attribute << ' ' << indexInfo.table << ' ' << indexInfo.alias << std::endl;
        return out;
    }

    inline std::ostream &operator<<(std::ostream &out, const SqlValueType sqlValueType){
        out << sqlValueType.type << ' ' << sqlValueType.char_size;
        return out;
    }

    inline std::ostream &operator<<(std::ostream &out, const Attribute attribute){
        out << attribute.name << ' ' << attribute.type << ' ' << attribute.primary << ' ' << attribute.unique;
        return out;
    }

    inline std::ostream &operator<<(std::ostream &out, const std::pair<std::string, std::string> index){
        out << index.first << ' ' << index.second;
        return out;
    }

    inline std::ostream &operator<<(std::ostream &out, const TableInfo tableInfo) {

        out << tableInfo.name << ' '
            << tableInfo.primaryKey << ' '
            << tableInfo.record_count << ' '
            << tableInfo.record_length << ' '
            << tableInfo.attributes.size() << ' '
            << tableInfo.indices.size() << ' ';

        for (auto &attribute : tableInfo.attributes)
            out << attribute << ' ';

        for (auto &index : tableInfo.indices)
            out << index << ' ';

        out << std::endl;
        return out;
    }

    inline std::istream &operator>>(std::istream &in, IndexInfo indexInfo) {
        in >> indexInfo.name >> indexInfo.attribute >> indexInfo.table >> indexInfo.alias;
        return in;
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
        return in;
    }

    inline std::istream &operator>>(std::istream &in, Attribute &attribute){
        in >> attribute.name >> attribute.type >> attribute.primary >> attribute.unique;
        return in;
    }

    inline std::istream &operator>>(std::istream &in, std::pair<std::string, std::string> &index){
        in >> index.first >> index.second;
        return in;
    }

    inline std::istream &operator>>(std::istream &in, TableInfo &tableInfo) {
        int attrSize = 0;
        int idxSize = 0;

        in >> tableInfo.name >> tableInfo.primaryKey >> tableInfo.record_count >> tableInfo.record_length >> attrSize
           >> idxSize;

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
        return in;
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

    inline bool SqlValueType::operator==(const SqlValueType &s) const {
        return s.type == type;
    }

    inline bool SqlValueType::operator!=(const SqlValueType &s) const {
        return s.type != type;
    }
    inline std::string TableFileName(const std::string & table_name) {return table_name;}
    inline std::string IndexFileName(const std::string & index_name) {return index_name;}

    inline Operator GetOpFromString(const std::string opStr) {
        static std::map<std::string, Operator> toOp = {
                {"=",  Equal},
                {"!=", NotEqual},
                {">",  GreaterThan},
                {">=", GreaterEqual},
                {"<",  LessThan},
                {"<=", LessEqual}
        };
        return toOp[opStr];
    };
    inline Record Record::Extract(const std::vector<int> indices) const
    {
    	Record result;
    	for (auto idx : indices)
    		result.values.push_back(values[idx]);
    	return result;
    }
    inline Record Record::Extract(int i) const
    {
    	Record result;
    	result.values.push_back(values[i]);
    	return result;
    }
    
    inline Attribute TableInfo::FetchAttribute(const std::string & attribute_name)
    {
    	for (const auto & attr : attributes)
    	{
    		if (attr.name == attribute_name)
    			return attr;
    	}
    	std::cerr << "Attribute " + attribute_name + " does not exist!\n";
    	std::exit(0);
    }
    inline Record Record::Extract(const TableInfo & table, const std::string & attribute) const
    {
    	return Extract(table, std::vector<std::string>(1, attribute));
    }
    inline Record Record::Extract(const TableInfo & table, const std::vector<std::string> & attributes) const
    {
    	std::vector<int> indices;
    	for (const auto & attr : attributes)
    	{
    		bool found = false;
    		for (int i = 0; i < table.attributes.size(); i++)
	    	{
				
				if (table.attributes[i].name == attr)
				{
					indices.push_back(i);
					found = true;
				}
	    	}
	    	if (not found)
	    	{
	    		std::cerr << "Attribute " + attr + " does not exists!\n";
    			std::exit(0);
	    	}
    	}

    	return Extract(indices);
    }

}


#endif //MINISQL_MINITYPE_H
