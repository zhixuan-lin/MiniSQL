
#include <string>
#include <iostream>
#include <utility>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <map>
#include "MiniType.h"

namespace MINI_TYPE
{
    // SqlValueType
    int SqlValueType::BPTreeDegree() const
    {
        return static_cast<int>(BlockSize / (TypeSize() + sizeof(int)));
    }
    
    size_t SqlValueType::TypeSize() const
    {
        switch(type)
        {
            case MiniInt: return sizeof(int);
            case MiniFloat: return sizeof(float);
            case MiniChar: return char_size;
        }
    }
    
    std::ostream &operator<<(std::ostream &out, const SqlValueType & sqlValueType){
        out << sqlValueType.type << ' ' << sqlValueType.char_size;
        return out;
    }
    
   
    
    // SqlValue
    void SqlValue::ReadFromMemory(const char * source, int byte_offset)
    {
        size_t size = type.TypeSize();
        char temp[MaxChar + 1];
        switch(type.type)
        {
            case MiniInt: std::memcpy(&i, source + byte_offset, size); break;
            case MiniFloat: std::memcpy(&f, source + byte_offset, size); break;
            case MiniChar:
                std::memset(temp, 0, MaxChar + 1);
                std::memcpy(temp, source + byte_offset, size);
                str = temp;
                break;
        }
    }
    void SqlValue::WriteToMemory(char * dest, int byte_offset) const
    {
        size_t size = type.TypeSize();
        switch(type.type)
        {
            case MiniInt: std::memcpy(dest + byte_offset, &i, size); break;
            case MiniFloat: std::memcpy(dest + byte_offset, &f, size); break;
            case MiniChar:
                char temp[MINI_TYPE::MaxChar + 1];
                std::memset(temp, 0, MINI_TYPE::MaxChar + 1);
                std::memcpy(temp, str.c_str(), str.size());
                std::memcpy(dest + byte_offset, temp, size); break;
        }
    }
    std::string SqlValue::ToStr() const
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
    bool SqlValue::operator<(const SqlValue & s) const
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
    bool SqlValue::operator<=(const SqlValue & s) const
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
    bool SqlValue::operator>(const SqlValue & s) const
    { return not (*this <= s); }
    bool SqlValue::operator>=(const SqlValue & s) const
    { return not (*this < s); }
    bool SqlValue::operator==(const SqlValue & s) const
    { return *this <= s and *this >= s; }
    bool SqlValue::operator!=(const SqlValue & s) const
    { return not (*this == s); }
    
    
    std::ostream &operator<<(std::ostream &out, const SqlValue & value)
    {
        switch(value.type.type)
        {
            case TypeId::MiniInt:
                out << value.i;
                break;
            case TypeId::MiniFloat:
                out << value.f;
                break;
            case TypeId::MiniChar:
                out << value.str;
                break;
            default:
                std::cerr << "Unknown Type!\n";
                exit(0);
        }
        return out;
    }
    
    
    // TableInfo
    TableInfo::TableInfo(const std::string nm, const std::vector<Attribute> attrs)
    : name(nm), attributes(attrs)
    {
        for (const auto & attr : attrs)
            record_length += attr.type.TypeSize();
    }
    
    Attribute TableInfo::FetchAttribute(const std::string & attribute_name)
    {
        for (const auto & attr : attributes)
        {
            if (attr.name == attribute_name)
                return attr;
        }
        std::cerr << "Attribute " + attribute_name + " does not exists!\n";
        std::exit(0);
    }
    
    std::ostream &operator<<(std::ostream &out, const TableInfo tableInfo) {
        
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
        return out;
    }
    
    
    // Record
    Record Record::Extract(const std::vector<int> indices) const
    {
        Record result;
        for (auto idx : indices)
            result.values.push_back(values[idx]);
        return result;
    }
    Record Record::Extract(int i) const
    {
        Record result;
        result.values.push_back(values[i]);
        return result;
    }
    Record Record::Extract(const TableInfo & table, const std::string & attribute) const
    {
        return Extract(table, std::vector<std::string>(1, attribute));
    }
    Record Record::Extract(const TableInfo & table, const std::vector<std::string> & attributes) const
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
    void Record::Conform(const TableInfo & table)
    {
        values.resize(table.attributes.size());
        for (int i = 0; i < table.attributes.size(); i++)
            values[i].type = table.attributes[i].type;
    }
    
    std::ostream &operator<<(std::ostream &out, const Record & record)
    {
        for (const auto & value : record.values)
            out << value << " ";
        out << std::endl;
        return out;
    }
    
    // Attribute
    
    std::ostream &operator<<(std::ostream &out, const Attribute & attribute){
        out << attribute.name << ' ' << attribute.type << ' ' << attribute.primary << ' ' << attribute.unique;
        return out;
    }
    
    // IndexInfo
    std::ostream &operator<<(std::ostream &out, const std::pair<std::string, std::string> index){
        out << index.first << ' ' << index.second;
        return out;
    }
    
    // Table
    Table Table::Select(const std::vector<Condition> & conditions)
    {
        Table result(this->info);
        for (const auto & record : records)
            if (Test(conditions, this->info, record))
                result.records.push_back(record);
        return result;
    }
    
    
    std::ostream &operator<<(std::ostream &out, const Table & table)
    {
        out << "Table: " << table.info.name << std::endl;
        for (const auto & record : table.records)
            out << record;
        if (table.records.size() == 0)
            out << "Empty\n";
        return out;
    }
    
    // Condition
    bool Condition::Test(SqlValue val) const
    {
        switch (op)
        {
            case Operator::Equal: return val == value;
            case Operator::GreaterThan: return val > value;
            case Operator::GreaterEqual: return val >= value;
            case Operator::LessThan: return val < value;
            case Operator::LessEqual: return val <= value;
            case Operator::NotEqual: return val != value;
        }
    }
    bool Test(const std::vector<Condition> & conditions, const TableInfo & table, const Record & record)
    {
        for (const auto & cond : conditions)
            if (not cond.Test(record.Extract(table, cond.attribute.name).values[0]))
                return false;
        return true;
    }
    bool Test(Condition & condition, const TableInfo & table, const Record & record)
    {
        if (not condition.Test(record.Extract(table, condition.attribute.name).values[0]))
            return false;
        return true;
    }
    
    
    
    
    std::istream &operator>>(std::istream &in, SqlValueType &sqlValueType) {
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
    
    std::istream &operator>>(std::istream &in, Attribute &attribute){
        in >> attribute.name >> attribute.type >> attribute.primary >> attribute.unique;
        return in;
    }
    
    std::istream &operator>>(std::istream &in, std::pair<std::string, std::string> &index){
        in >> index.first >> index.second;
        return in;
    }
    
    std::istream &operator>>(std::istream &in, TableInfo &tableInfo) {
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
        return in;
    }
    
    // Operator
    Operator GetOpFromString(const std::string opStr) {
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
}
