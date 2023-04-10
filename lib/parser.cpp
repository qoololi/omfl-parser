#include "parser.h"
#include <iostream>

bool isValid = 1;

omfl::value get_val(bool& isValid, int& i, const std::string& str);

omfl::value get_array(bool& isValid, int& i, const std::string& str);

std::string get_section(bool& isValid, int& i, const std::string& str) {
    std::string section;
    if (str[i] == '[') i++;
    while (str[i] != ']') {
        section += str[i];
        i++;
    }
    i++;
    return section;
}

std::string get_name(int& i, const std::string& str) {
    std::string name;
    while (i < str.size() && str[i] != ' ' && str[i] != '=') {
        name += str[i];
        i++;
    }
    return name;
}

omfl::value get_number(bool& isValid, int& i, const std::string& str) {
    omfl::value val;
    val.type = 'i';
    std::string num;
    while (i < str.size() && str[i] != ' ' && str[i] != '\n' && str[i] != ',' && str[i] != ']') {
        if (str[i] == '.') {
            val.type = 'f';
        }
        num += str[i];
        i++;
    }
    if ((num[0] == '+' || num[0] == '-') && num.size() == 1) isValid = 0;
    if (num[0] == '.') isValid = 0;
    for (int i = 1; i < num.size(); i++) {
        if (num[i] == '+' || num[i] == '-') isValid = 0;
        if(num[i] == '.' && (i < 1 || num[i-1]=='+' || num[i-1]=='-')) isValid = 0;
    }
    if (num[num.size() - 1] == '.') isValid = 0;
    if (!isValid) return val;
    if (val.type == 'i') {
        val.intVal = stoi(num);
    }
    else {
        val.floatVal = stof(num);
    }
    return val;
}

omfl::value get_string(bool& isValid, int& i, const std::string& str) {
    omfl::value val;
    val.type = 's';
    if (str[i] == '"') i++;
    while (i < str.size() && str[i] != '"') {
        val.stringVal += str[i];
        i++;
    }
    if (i >= str.size()) isValid = 0;
    i++;
    return val;
}

omfl::value get_bool(bool& isValid, int& i, const std::string& str) {
    omfl::value val;
    val.type = 'b';
    std::string num;
    while (i < str.size() && str[i] != '\n' && str[i] != ' ' && str[i] != ',' && str[i] != ']') {
        num += str[i];
        i++;
    }
    if (num != "true" && num != "false") isValid = 0;
    if (num == "true") {
        val.boolVal = 1;
    }
    else {
        val.boolVal = 0;
    }
    return val;
}

omfl::value get_array(bool& isValid, int& i, const std::string& str) {
    omfl::value val;
    val.type = 'a';
    if (str[i] == '[') i++;
    while (i < str.size() && str[i] != ']' && isValid){
        if (str[i] == ' ' || str[i] == ',') {
            i++;
            continue;
        }
        if (str[i] == ';') {
            isValid = 0;
            break;
        }
        if (str[i] == '\n') isValid = 0;
        val.array.push_back(get_val(isValid, i, str));
    }
    if (i >= str.size()) isValid = 0;
    i++;
    return val;
}

omfl::value get_val(bool& isValid, int& i, const std::string& str) {
    omfl::value val;
    while (i < str.size() && (str[i] == ' ' || str[i] == '=' || str[i] == ',') && isValid) {
        i++;
    }
    if (str[i] == '\n') isValid = 0;
    if (str[i] == '[') {
        val = get_array(isValid, i, str);
    }else if (str[i] == '+' || str[i] == '-' || (str[i] >= '0' && str[i] <= '9')) {
        val = get_number(isValid, i, str);
    }else if (str[i] == '"') {
        val = get_string(isValid, i, str);
    }
    else if (str[i] == 't' || str[i] == 'f') {
        val = get_bool(isValid, i, str);
    }
    else isValid = 0;
    return val;
}

omfl::OMFL omfl::parse(const std::filesystem::path& path)
{
    std::string str;
    std::ifstream file(path);

    while (!file.eof()) {
        char h = file.get();
        str += h;
    }

    str.pop_back();
    omfl::OMFL output = omfl::parse(str);
    return output;
}

omfl::OMFL omfl::parse(const std::string& str)
{
    int i = 0;
    omfl::OMFL ans;
    std::string name;
    omfl::value val;
    std::string section;
    ans.isValid = 1;
    while(i < str.size() && ans.isValid) {
        if (str[i] == ' ' || str[i] == '\n') {
            i++;
            continue;
        }
        if (str[i] == '#') {
            while (i < str.size() && str[i] != '\n') i++;
            continue;
        }
        if (str[i] == '[') {
            section = get_section(ans.isValid, i, str);
            continue;
        }
        name = get_name(i, str);
        if (name.size() == 0) ans.isValid = 0;
        //std::cout << '"' << name << '"';
        for (int i = 0; i < name.size(); i++) {
            if (!(name[i] == '-' || name[i] == '_' || (name[i] >= 'a' && name[i] <= 'z') || (name[i] >= 'A' && name[i] <= 'Z') || (name[i] >= '0' && name[i] <= '9'))) {
                ans.isValid = 0;
                break;
            }
        }
        val = get_val(ans.isValid, i, str);
        if (section.size() == 0) {
            ans.flags[name] = val;
        } else {
            ans.flags[section + '.' + name] = val;
        }
    }
    return ans;
}