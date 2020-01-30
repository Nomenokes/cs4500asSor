

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>

#include "sordata.h"


void fail_file(int num){
    printf("Failed reading file: ");
    
    switch(num){
        case 61:
            printf("does not exist");
            break;
        case 62:
            printf("empty");
            break;
        case 69:
            printf("too large");
            break;
        default:
            printf("unknown reason %d", num);
    }
    puts("");
    exit(num);
}
//return should be freed
//if length is negative, assumed parameter does not exist
char* read_file(const char* filename, unsigned int from, long length){
    
    struct stat filestats;
    int rv = stat(filename, &filestats);
    if(rv != 0) fail_file(61);
    
    size_t size = filestats.st_size;
    if(size == 0) fail_file(62);
    
    if(from >= size) from = 0;
    if(length <= 0) size = size - from;
    else if(size > from + (unsigned int)length) size = length;
    bool addtop = from == 0; //check if should wrap in newlines
    bool addbottom = (length < 0) || (from + (unsigned int)length > size);
    char* ret = (char*)malloc(size + 1 + (addtop ? 1 : 0) + (addbottom ? 1 : 0));
    if(ret == 0) fail_file(69);
    
    FILE* f = fopen(filename, "r");
    if(f == 0) fail_file(70);
    
    rv = fseek(f, from, SEEK_SET);
    if(rv != 0) fail_file(73);
    
    rv = fread(addtop ? &(ret[1]) : ret, 1, size, f);
    if(rv == 0) fail_file(71);
    
    int offset = 0;
    if(addtop){
        ret[0] = '\n';
        offset++;
    }
    if(addbottom){
        ret[size + offset] = '\n';
        offset++;
    }
    ret[size + offset] = '\0';
    
    rv = fclose(f);
    if(rv != 0) fail_file(72);
    return ret;
}

void print_col_idx(Type t, void* data){
    switch(t){
        case MISSING:
            printf("MISSING\n");
            break;
        case BOOL:
            printf("%d\n", *((bool*)data) ? 1 : 0);
            break;
        case INT:
            printf("%d\n", *((int*)data));
            break;
        case FLOAT:
            printf("%f\n", *((double*)data));
            break;
        case STRING:
            printf("%s\n", (char*)data);
            break;
        default:
            printf("aaaAAAAAAA\n");
            abort();
    }
}
void print_col_type(Type t){
    switch(t){
        case MISSING:
            printf("MISSING\n");
            break;
        case BOOL:
            printf("BOOL\n");
            break;
        case INT:
            printf("INT\n");
            break;
        case FLOAT:
            printf("FLOAT\n");
            break;
        case STRING:
            printf("STRING\n");
            break;
        default:
            printf("aaaAAAAAAA\n");
            abort();
    }
}
//prints usage message
void usage(const char* name){
    printf("Usage: %s <-f filename>\n\
            [-from uint] [-len uint] [-print_col_type col]\n\
            [-print_col_idx col row] [-is_missing_idx col row]\n", name);
    exit(1);
}
int main(int argc, char** argv){
    
    bool f_fil = false;
    bool f_fro = false;
    bool f_len = false;
    
    bool f_typ = false;
    bool f_idx = false;
    bool f_mis = false;
    
    char* v1_fil;
    unsigned int v1_fro;
    long v1_len;
    
    unsigned int v1_typ;
    unsigned int v1_idx;
    unsigned int v2_idx;
    unsigned int v1_mis;
    unsigned int v2_mis;
    
    
    for(int i = 1; i < argc; i++){
        
        if(strcmp(argv[i], "-f") == 0){ 
            f_fil = true;
            i++; 
            if(i >= argc) usage(argv[0]);
            v1_fil = argv[i];
        }
        
        else if(strcmp(argv[i], "-from") == 0){ 
            f_fro = true;
            i++;
            if(i >= argc) usage(argv[0]);
            v1_fro = (unsigned int)atol(argv[i]);
        }
        
        else if(strcmp(argv[i], "-len") == 0){ 
            f_len = true;
            i++;
            if(i >= argc) usage(argv[0]);
            v1_len = (unsigned int)atol(argv[i]);
        }
        
        else if(strcmp(argv[i], "-print_col_type") == 0){ 
            f_typ = true;
            i++;
            if(i >= argc) usage(argv[0]);
            v1_typ = (unsigned int)atoi(argv[i]);
        }
        
        else if(strcmp(argv[i], "-print_col_idx") == 0){ 
            f_idx = true;
            i++;
            if(i >= argc) usage(argv[0]);
            v1_idx = (unsigned int)atol(argv[i]);
            i++;
            if(i >= argc) usage(argv[0]);
            v2_idx = (unsigned int)atol(argv[i]);
        }
        
        else if(strcmp(argv[i], "-is_missing_idx") == 0){ 
            f_mis = true;
            i++;
            if(i >= argc) usage(argv[0]);
            v1_mis = (unsigned int)atol(argv[i]);
            i++;
            if(i >= argc) usage(argv[0]);
            v2_mis = (unsigned int)atol(argv[i]);
        }
        
        else{ 
            usage(argv[0]);
        }
    }
    
    if(!f_fil) usage(argv[0]);
    if(!f_fro) v1_fro = 0;
    if(!f_len) v1_len = -1;
    
    char* raw = read_file(v1_fil, v1_fro, v1_len);
    SorData* data = new SorData(raw);
    
    if(f_typ) {
        print_col_type(data->get_col_type(v1_typ));
    }
    if(f_idx) {
        print_col_idx(data->get_col_type(v1_idx), data->get_val(v1_idx, v2_idx));
    }
    if(f_mis) {
        printf("%d\n", data->is_missing(v1_mis, v2_mis) ? 1 : 0);
    }
    
    delete data;
    free(raw);
    
    return 0;
}
