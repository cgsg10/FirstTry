#include <iostream>
#include <vector>
#include <list>
#include "map"

using namespace std;

template<class T>
class Handle {
    T *p;
public:
    Handle(T *pp) : p{pp} { }

    ~Handle() { delete p; }        // user-defined destructor: no implicit copy or move

    Handle(Handle &&h) : p{h.p} { h.p = nullptr; }            // transfer ownership
    Handle &operator=(Handle &&h) {
        delete p;
        p = h.p;
        h.p = nullptr;
        return *this;
    }    // transfer ownership

    Handle(const Handle &) = delete;        // no copy
    Handle &operator=(const Handle &) = delete;

    // ...
};

enum Flags { good=0, fail=1, bad=2, eof=4 };
constexpr Flags operator|(Flags f1, Flags f2) { return Flags(int(f1) | int(f2)); }

int main() {

    // __cplusplus
    {
        cout << "c++ version: " << __cplusplus << endl;
    }

    // auto -- deduction of a type from an initializer
    {
        auto iter = vector<float>().begin();
    }

    // Range-for statement
    {
        for (auto value : {1, 2, 3}) cout << value; // here array is initializer list thus is iterable
        //for (auto value : {1, 1.2}) cout << value; // Doesn't work :(
        cout << endl;
    }

    // right-angle brackets
    {
        vector<vector<double>> ololo{vector<double>()={1}}; // fun
    }

    // control of defaults: default and delete
    {
        class X {
        public:
            X(){} // If any of default operations is declared other should be declared explicitly
            X& operator=(const X&) = delete;	// Disallow copying
            X(const X&) = delete;
        };
        X x1, x2;
        // x1 = x2; // copying is prohibited

        struct Z {
            // ...

            Z(long long){}     // can initialize with an long long
            Z(long) = delete; // but not anything less
        };
        // Z z(1L); // use of deleted function 'main()::Z::Z(long int)'
    }

    // control of defaults: move and copy
    {
        //    By default, a class has 5 operations:
        //    copy assignment
        //    copy constructor
        //    move assignment
        //    move constructor
        //    destructor
        Handle<double> example(NULL);
    }

    // enum class -- scoped and strongly typed enums
    {
        enum Alert { green, yellow, orange, red }; // traditional enum

        enum class Color { red, blue };   // scoped and strongly typed enum
        // no export of enumerator names into enclosing scope
        // no implicit conversion to int
        enum class TrafficLight { red, yellow, green };

//        Alert a = 7;              // error (as ever in C++)
//        Color c = 7;              // error: no int->Color conversion

        int a2 = red;             // ok: Alert->int conversion
        int a3 = Alert::red;      // error in C++98; ok in C++11
//        int a4 = blue;            // error: blue not in scope
//        int a5 = Color::blue;     // error: not Color->int conversion

        Color a6 = Color::blue;   // ok

        enum class Color2 : char { red, blue };	// compact representation

        enum class TrafficLight2 { red, yellow, green };  // by default, the underlying type is int

        enum E { E1 = 1, E2 = 2, Ebig = 0xFFFFFFF0U };   // how big is an E?
        // (whatever the old rules say;
        // i.e. "implementation defined")

        enum EE : unsigned long { EE1 = 1, EE2 = 2, EEbig = 0xFFFFFFF0U };   // now we can be specific

        enum class Color_code : char;     // (forward) declaration
        void foobar(Color_code* p);       // use of forward declaration
        // ...
        enum class Color_code : char { red, yellow, green, blue }; // definition
    }

    // constexpr -- generalized and guaranteed constant expressions
    {
        struct A {
            void f(Flags x) {
                switch (x) {
                    case bad:         /* ... */ break;
                    case eof:         /* ... */ break;
                    case bad | eof:     /* ... */ break;
                    default:          /* ... */ break;
                }
            }
        };

        A().f(good|fail);
    }

    // decltype -- the type of an expression
    {
        struct A {
            void f(const vector<int> &a, vector<float> &b) {
                typedef decltype(a[0] * b[0]) Tmp;
                for (int i = 0; i < b.size(); ++i) {
                    Tmp *p = new Tmp(a[i] * b[i]);
                    // ...
                }
                // ...
            }
        };
    }

    // Initializer lists
    {
        vector<double> v = { 1, 2, 3.456, 99.99 };
        list<pair<string,string>> languages = {
                {"Nygaard","Simula"}, {"Richards","BCPL"}, {"Ritchie","C"}
        };
        map<vector<string>,vector<int>> years = {
                { {"Maurice","Vincent", "Wilkes"},{1913, 1945, 1951, 1967, 2000} },
                { {"Martin", "Ritchards"}, {1982, 2003, 2007} },
                { {"David", "John", "Wheeler"}, {1927, 1947, 1951, 2004} }
        };
    }

    // Preventing narrowing
    {
        int x = 7.3;		// Ouch!
        struct A {
            void f(int){}
        };
        A().f(7.3);			// Ouch!

        //int x0 {7.3};	// error: narrowing
        //int x1 = {7.3};	// error: narrowing
        double d = 7;
        //int x2{d};		// error: narrowing (double to int)
        char x3{7};		// ok: even though 7 is an int, this is not narrowing
        //vector<int> vi = { 1, 2.3, 4, 5.6 };	// error: double to int narrowing
    }

    // Delegating constructors
    {
        class X {
            int a;
        public:
            X(int x) { if (0<x && x<=100) a=x; else throw int(x); }
            X() :X{42} { }
            //X(string s) :X{lexical_cast<int>(s)} { }
            // ...
        };
    }

    // In-class member initializers
    {
        class A {
        public:
            int a = 7;
        };

//        class A {
//        public:
//            A() {}
//            A(int a_val) : a(a_val) {}
//            A(D d) : b(g(d)) {}
//            int a = 7;
//            int b = 5;
//        private:
//            HashingFunction hash_algorithm{"MD5"};  // Cryptographic hash to be applied to all A instances
//            std::string s{"Constructor run"};       // String indicating state in object lifecycle
//        };
    }

    // Inherited constructors
    {
        struct B1 {
            B1(int) { }
        };
        struct D1 : B1 {
            using B1::B1;	// implicitly declares D1(int)
            int x{0};	// note: x is initialized
        };

        D1 d{6};
    }

    //

    return 0;
}

