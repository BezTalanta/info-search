#include <iostream>
#include <fstream>
#include <typeinfo>

#include "peeler.hpp"
#include "logger.hpp"

using namespace std;

int main(int argc, char *argv[]){
    string FORWARD_PARSE, BACKWARD_PARSE;

    for (size_t i = 1; i < argc; i++){
        string current_arg = string(argv[i]);
        if (current_arg == "-f"){
            FORWARD_PARSE = argv[i + 1];
            i++;
        }
        else if (current_arg == "-b"){
            BACKWARD_PARSE = argv[i + 1];
            i++;
        }
    }

    Peeler peeler = Peeler();

    NEW_SESSION()

    peeler.test();
    peeler.out_all();

    return 0;

    if (!FORWARD_PARSE.empty()){
        // forward_parse(FORWARD_PARSE);
        for (int i = 8075; i < 8175; ++i)
        {
            if (i == 0){
                peeler.peel(i + 1, true);
            }
            else
                peeler.peel(i + 1);
        }
    }
    peeler.out_all();
    return 0;
}


/*
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