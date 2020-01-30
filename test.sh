
#!/bin/sh

program=$1
file_prefix="_test"
file_in_postfix=".in"
file_out_postfix=".out"
file_expect_postfix=".expect"

function gen_in {
    printf "$2" > "$file_prefix$1$file_in_postfix"
}

function gen_out {
    eval "$program -f $file_prefix$1$file_in_postfix $2" > "$file_prefix$1$file_out_postfix"
}

function gen_expect {
    printf "$2" > "$file_prefix$1$file_expect_postfix"
}

function test {
    if cmp -s "$file_prefix$1$file_out_postfix" "$file_prefix$1$file_expect_postfix"; then
        printf "\033[0;32m[test $1 succeeded]\033[0m\n"
    else
        printf "\033[0;31m[test $1 failed]\n  expected:\033[0m\n"
        cat "$file_prefix$1$file_expect_postfix"
        printf "\n\033[0;31m  but recieved:\033[0m\n"
        cat "$file_prefix$1$file_out_postfix"
    fi
}

function clean {
    rm "$file_prefix$1$file_in_postfix"
    rm "$file_prefix$1$file_out_postfix"
    rm "$file_prefix$1$file_expect_postfix"
}



gen_in 0 "<>"
gen_in 1 "\n<hello>\n<there>"
gen_in 2_1 "\n<1> <1> <1>\n <2> <1.1> <0>\n <\\\"2\\\"> <5.123> <1>\n <2>\n"
gen_in 2_2 "\n<1> <1> <1>\n <2> <1.1> <0>\n <\\\"2\\\"> <5.123> <1>\n <2>\n"
gen_in 2_3 "\n<1> <1> <1>\n <2> <1.1> <0>\n <\\\"2\\\"> <5.123> <1>\n <2>\n"
gen_in 3 "<\\\"this is one string\\\">"
gen_in 4 "<4>\n<0><3>\n<4>"
gen_in 5 "<1><2><3><4><5>"

gen_out 0 "-is_missing_idx 0 0"
gen_out 1 "-from 3 -print_col_idx 0 0"
gen_out 2_1 "-print_col_type 0"
gen_out 2_2 "-print_col_type 1"
gen_out 2_3 "-print_col_type 2"
gen_out 3 "-print_col_idx 0 0"
gen_out 4 "-from 1 -len 12 -print_col_type 0"
gen_out 5 "-print_col_idx 2 0"

gen_expect 0 "1\n"
gen_expect 1 "there\n"
gen_expect 2_1 "STRING\n"
gen_expect 2_2 "FLOAT\n"
gen_expect 2_3 "BOOL\n"
gen_expect 3 "this is one string\n"
gen_expect 4 "BOOL\n"
gen_expect 5 "3\n"


test 0
test 1
test 2_1
test 2_2
test 2_3
test 3
test 4
test 5


clean 0
clean 1
clean 2_1
clean 2_2
clean 2_3
clean 3
clean 4
clean 5