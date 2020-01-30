#pragma once
#include<regex>
#include<vector>
#include<algorithm>

using namespace std;

enum Type { MISSING, BOOL, INT, FLOAT, STRING };

class SorData {
    public:
    
    vector<Type> types;
    vector<vector<void*>> data; //2-d array of void pointers

    SorData(char* input) {
        vector<vector<const char*>>* raw = parse_delims_(input);
        infer_schema_(*raw);
        store_parsed_(*raw);
        for (size_t row = 0; row < raw->size(); row++) {
            for (size_t col = 0; col < (*raw)[row].size(); col++) {
                delete (*raw)[row][col];
            }
        }

        delete raw;
    }

    ~SorData() {
        for (size_t col = 0; col < data.size(); col++) {
            for (size_t row = 0; row < data[col].size(); row++) {
                switch (types[col]) {
                    case BOOL:
                        delete (bool*)(data[col][row]);
                        break;
                    case INT:
                        delete (int*)(data[col][row]);
                        break;
                    case FLOAT:
                        delete (double*)(data[col][row]);
                        break;
                    case STRING:
                        delete (char*)(data[col][row]);
                        break;
                    default:
                        break;
                }
            }
        }
    }

    vector<vector<const char*>>* parse_delims_(const char* input) {
        string rest (input);
        vector<vector<const char*>>* out = new vector<vector<const char*>>();
        regex r ("<\\s*(\\S*?|\".*?\")\\s*>|\\n");
        smatch match;
        
        while (regex_search (rest, match, r)) {
            if (match[0].str()[0] == '\n') {
                out->push_back(vector<const char*>());
            } else {
                if (out->size() != 0) {
                    size_t size = match[1].str().size();
                    char* val = new char[size+1];
                    match[1].str().copy(val, size);
                    val[size] = '\0';
                    out->back().push_back(val);
                }
            }
            rest = match.suffix().str();
        }
        out->pop_back();
        return out;
    }

    void store_parsed_(vector<vector<const char*>>& parsed){
        data = vector<vector<void*>>(types.size());
        for (size_t col = 0; col < types.size(); col++) {
            data.push_back(vector<void*>(parsed.size()));
            for (size_t row = 0; row < parsed.size(); row++) {
                if (parsed[row].size() <= col || strlen(parsed[row][col]) == 0 || get_type_(parsed[row][col]) > types[col]) {
                    data[col].push_back(NULL);
                } else {
                    size_t len;
                    const char* start;
                    switch (types[col]) {
                        case BOOL:
                            data[col].push_back(new bool(atoi(parsed[row][col])));
                            break;
                        case INT:
                            data[col].push_back(new int(atoi(parsed[row][col])));
                            break;
                        case FLOAT:
                            data[col].push_back(new double(atof(parsed[row][col])));
                            break;
                        case STRING:
                            start = parsed[row][col];
                            len = strlen(start);
                            if (start[0] == '"') {
                                len -= 2;
                                start ++;
                            }
                            data[col].push_back(new char[len+1]);
                            memcpy(data[col][row], start, len);
                            ((char*) data[col][row])[len] = 0;
                            break;
                        case MISSING:
                            data[col].push_back(NULL);
                            break;
                    }
                }
            }
        }
    }

    Type get_type_(const char* val) {
        if (strlen(val) == 0) return MISSING;
        regex rbool ("0*[01]");
        regex rint ("[+-]?\\d+");
        regex rfloat ("[+-]?\\d*\\.\\d+");
        if (regex_match(val, rbool)) return BOOL;
        if (regex_match(val, rint)) return INT;
        if (regex_match(val, rfloat)) return FLOAT;
        return STRING;
    }

    void infer_schema_(vector<vector<const char*>>& data) {
        for(size_t row = 0; row < min(500ul, data.size()); row++) {
            for(size_t col = 0; col < data[row].size(); col++) {
                if (col >= types.size()) types.push_back(BOOL);
                Type ctype = get_type_(data[row][col]);
                if (types[col] < ctype) types[col] = ctype;
            }
        }
    }

    bool is_valid_idx_(size_t col, size_t row) {
        if (col >= data.size() || row >= data[col].size()) return false;
        return true;
    }

    Type get_col_type(size_t index){
        if (index >= types.size()) return MISSING;
        return types[index];
    }

    bool is_missing(size_t col, size_t row) {
        if (!is_valid_idx_(col, row)) return true;
        return data[col][row] == NULL;
    }

    void* get_val(size_t col, size_t row) {
        if (!is_valid_idx_(col, row)) return NULL;
        return data[col][row];
    }
};
