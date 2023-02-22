#include <iostream>
#include <fstream>
#include <typeinfo>
#include <time.h>

#include "peeler.hpp"
#include "logger.hpp"

using namespace std;

int main(int argc, char *argv[]){
    string FORWARD_PARSE, BACKWARD_PARSE;

    // for (size_t i = 1; i < argc; i++){
    //     string current_arg = string(argv[i]);
    //     if (current_arg == "-f"){
    //         FORWARD_PARSE = argv[i + 1];
    //         i++;
    //     }
    //     else if (current_arg == "-b"){
    //         BACKWARD_PARSE = argv[i + 1];
    //         i++;
    //     }
    // }

    Peeler peeler = Peeler();

    NEW_SESSION()

    // peeler.test();
    // peeler.out_all();

    // return 0;

    // if (!FORWARD_PARSE.empty()){
        // forward_parse(FORWARD_PARSE);
        clock_t start = clock();
        for (int i = 1; i < 500; ++i)
        {
            if (i == 0){
                peeler.peel(i + 1, true);
            }
            else
                peeler.peel(i + 1);
        }
        cout << "\tTime passed: " << (double)((clock() - start)/CLOCKS_PER_SEC) << "s\n";
    // }
    peeler.out_all();
    // string s;
    // cin >> s;
    // peeler.test();
    return 0;
}


/*
        // char32_t current_symbol = tolower(line[i]);
        // u32string dout = U"";
        // dout += current_symbol;
        // wstring_convert<codecvt_utf8_utf16<char32_t>,char32_t> codecvt;
        // cout << "Status: " << codecvt.to_bytes(dout) << ' ' << int(current_symbol) << '\n';

        string test = "Español";

        // char16_t y = u'ñ';

        u16string j = u"简体中文ñ";
        cout << (j == u"简体中文ñ") << '\n';
        // j += y;

        wstring_convert<codecvt_utf8_utf16<char16_t>,char16_t> codecvt;

        // for char use this
        wstring_convert<codecvt_utf8_utf16<wchar_t> > myconv;
        myconv.to_bytes(ws);

        cout << codecvt.to_bytes(j) << '\n';
*/