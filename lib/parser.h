#pragma once

#include <filesystem>
#include <fstream>
#include <istream>
#include <vector>
#include <string>
#include <unordered_map>


namespace omfl {

    class value {
    public:
        char type;

        int intVal;
        float floatVal;
        bool boolVal;
        std::string stringVal;
        std::vector<value> array;
    };

    class OMFL {
    public:
        bool valid() {
            return isValid;
        }
        bool IsInt() {
            return lastFlag.type == 'i';
        }
        bool IsFloat() {
            return lastFlag.type == 'f';
        }
        bool IsBool() {
            return lastFlag.type == 'b';
        }
        bool IsString() {
            return lastFlag.type == 's';
        }
        bool IsArray() {
            return lastFlag.type == 'a';
        }
        int AsInt() {
            return lastFlag.intVal;
        }
        bool AsBool() {
            return lastFlag.boolVal;
        }
        float AsFloat() {
            return lastFlag.floatVal;
        }
        std::string AsString() {
            return lastFlag.stringVal;
        }
        int AsIntOrDefault(int val) {
            if (IsInt()) {
                return lastFlag.intVal;
            }
            else return val;
        }
        float AsFloatOrDefault(float val) {
            if (IsFloat()) {
                return lastFlag.floatVal;
            }
            else return val;
        }
        std::string AsStringOrDefault(std::string str) {
            if (IsString()) {
                return lastFlag.stringVal;
            }
            else return str;
        }

        OMFL operator[](int i) {
            if (i < lastFlag.array.size())
                lastFlag = lastFlag.array[i];
            return *this;
        }

        OMFL Get(std::string str) {
            if (flags.find(searchName + str) == flags.end()) {
                searchName += str;
                searchName += '.';
            } else lastFlag = flags[searchName + str];
            return *this;
        }

        bool isValid = 1;
        value lastFlag;
        std::string searchName;
        std::unordered_map <std::string, value> flags;
    };
    OMFL parse(const std::filesystem::path& path);
    OMFL parse(const std::string& str);


}// namespace