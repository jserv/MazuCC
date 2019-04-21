#!/bin/bash

function compile {
    echo "$1" > /dev/stderr
    echo "$1" | ./mzcc > tmp.s || echo "Failed to compile $1"
    if [ $? -ne 0 ]; then
        echo "Failed to compile $1"
        exit
    fi
    gcc -no-pie -o tmp.out tmp.s
    if [ $? -ne 0 ]; then
        echo "GCC failed: $1"
        exit
    fi
}

function assert_equal {
    if [ "$1" != "$2" ]; then
        echo "Test failed: $2 expected but got $1"
        exit
    fi
}

function test_astf {
    result="$(echo "$2" | ./mzcc --dump-ast)"
    if [ $? -ne 0 ]; then
        echo "Failed to compile $2"
        exit
    fi
    assert_equal "$result" "$1"
}

function test_ast {
    test_astf "$1" "int f(){$2}"
}

function test_fail {
    expr="int f(){$1}"
    echo "$expr" | ./mzcc > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        echo "Should fail to compile, but succeded: $expr"
        exit
    fi
}

# Parser
test_ast '(int)f(){1;}' '1;'
test_ast '(int)f(){1L;}' '1L;'
test_ast '(int)f(){1152921504606846976L;}' '1152921504606846976;'
test_ast '(int)f(){(+ (- (+ 1 2) 3) 4);}' '1+2-3+4;'
test_ast '(int)f(){(+ (+ 1 (* 2 3)) 4);}' '1+2*3+4;'
test_ast '(int)f(){(+ (* 1 2) (* 3 4));}' '1*2+3*4;'
test_ast '(int)f(){(+ (/ 4 2) (/ 6 3));}' '4/2+6/3;'
test_ast '(int)f(){(/ (/ 24 2) 4);}' '24/2/4;'
test_ast '(int)f(){(decl int a 3);}' 'int a=3;'
test_ast "(int)f(){(decl char c 'a');}" "char c='a';"
test_ast '(int)f(){(decl *char s "abcd");}' 'char *s="abcd";'
test_ast '(int)f(){(decl [5]char s "asdf");}' 'char s[5]="asdf";'
test_ast '(int)f(){(decl [5]char s "asdf");}' 'char s[]="asdf";'
test_ast '(int)f(){(decl [3]int a {1,2,3});}' 'int a[3]={1,2,3};'
test_ast '(int)f(){(decl [3]int a {1,2,3});}' 'int a[]={1,2,3};'
test_ast '(int)f(){(decl [3][5]int a);}' 'int a[3][5];'
test_ast '(int)f(){(decl [5]*int a);}' 'int *a[5];'
test_ast '(int)f(){(decl int a 1);(decl int b 2);(= a (= b 3));}' 'int a=1;int b=2;a=b=3;'
test_ast '(int)f(){(decl int a 3);(addr a);}' 'int a=3;&a;'
test_ast '(int)f(){(decl int a 3);(deref (addr a));}' 'int a=3;*&a;'
test_ast '(int)f(){(decl int a 3);(decl *int b (addr a));(deref b);}' 'int a=3;int *b=&a;*b;'
test_ast '(int)f(){(if 1 {2;});}' 'if(1){2;}'
test_ast '(int)f(){(if 1 {2;} {3;});}' 'if(1){2;}else{3;}'
test_ast '(int)f(){(for (decl int a 1) 3 7 {5;});}' 'for(int a=1;3;7){5;}'
test_ast '(int)f(){"abcd";}' '"abcd";'
test_ast "(int)f(){'c';}" "'c';"
test_ast '(int)f(){(int)a();}' 'a();'
test_ast '(int)f(){(int)a(1,2,3,4,5,6);}' 'a(1,2,3,4,5,6);'
test_ast '(int)f(){(return 1);}' 'return 1;'
test_ast '(int)f(){(< 1 2);}' '1<2;'
test_ast '(int)f(){(> 1 2);}' '1>2;'
test_ast '(int)f(){(== 1 2);}' '1==2;'
test_ast '(int)f(){(deref (+ 1 2));}' '1[2];'
test_ast '(int)f(){(decl int a 1);(++ a);}' 'int a=1;a++;'
test_ast '(int)f(){(decl int a 1);(-- a);}' 'int a=1;a--;'
test_ast '(int)f(){(! 1);}' '!1;'
test_ast '(int)f(){(? 1 2 3);}' '1?2:3;'
test_ast '(int)f(){(and 1 2);}' '1&&2;'
test_ast '(int)f(){(or 1 2);}' '1||2;'
test_ast '(int)f(){(& 1 2);}' '1&2;'
test_ast '(int)f(){(| 1 2);}' '1|2;'
test_ast '(int)f(){1.200000;}' '1.2;'
test_ast '(int)f(){(+ 1.200000 1);}' '1.2+1;'

test_astf '(int)f(int c){c;}' 'int f(int c){c;}'
test_astf '(int)f(int c){c;}(int)g(int d){d;}' 'int f(int c){c;} int g(int d){d;}'
test_astf '(decl int a 3)' 'int a=3;'

test_astf '(decl (struct) a)' 'struct {} a;'
test_astf '(decl (struct (int) (char)) a)' 'struct {int x; char y;} a;'
test_astf '(decl (struct ([3]int)) a)' 'struct {int x[3];} a;'
test_ast '(int)f(){(decl (struct (int)) a);(decl *(struct (int)) p);(deref p).x;}' 'struct tag {int x;} a; struct tag *p; p->x;'
test_ast '(int)f(){(decl (struct (int)) a);a.x;}' 'struct {int x;} a; a.x;'

test_fail '0abc;'
test_fail '1+;'
test_fail '1=2;'

# & is only applicable to an lvalue
test_fail '&"a";'
test_fail '&1;'
test_fail '&a();'

echo "All tests passed"
