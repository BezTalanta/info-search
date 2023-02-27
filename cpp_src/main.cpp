#include <iostream>
#include <fstream>
#include <typeinfo>
#include <time.h>

#include "peeler.hpp"
#include "logger.hpp"
// #include "texter.hpp"

using namespace std;

int main(int argc, char *argv[]){
    bool PARSE_ARG = false, SAVE_ARG = false, RUN_ARG = false;

    for (size_t i = 1; i < argc; i++){
        string current_arg = string(argv[i]);
        if (current_arg == "-p") // Parse, so that means that program will parse txt
            PARSE_ARG = true;
        else if (current_arg == "-s") // Save to a file, that means program will save parsed info to a files
            SAVE_ARG = true;
        else if (current_arg == "--run") // Means that program will be listening, and searching
            RUN_ARG = true;
        else
            cout << current_arg << " isn't available argument!\n";
    }

    Peeler peeler = Peeler();

    // NEW_SESSION()

    // peeler.test();
    // peeler.out_all();

    peeler.load_indexes();
    peeler.search();

    // peeler.out_all();
    return 0;

    clock_t start = clock();
    for (int i = 1; i < 72501; ++i)
        peeler.peel(i);
    clock_t parse_end = clock();

    HIGH_LEVEL("Parse time: " + to_string((double)(parse_end - start)/CLOCKS_PER_SEC))

    peeler.write_all();

    HIGH_LEVEL("File generation time: "
        + to_string((double)(clock() - parse_end)/CLOCKS_PER_SEC))

    // peeler.out_all();
    // cout << "\tTime passed:\n";
    // cout << "Parse time: " <<
    //     (double)(parse_end - start)/CLOCKS_PER_SEC << "s\n";
    // cout << "Out time: " <<
    //     (double)(clock() - parse_end)/CLOCKS_PER_SEC << "s\n";
    // cout << "Full time: " <<
    //     (double)(clock() - start)/CLOCKS_PER_SEC << "s\n";

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

/*
    Stats:
        72500 games required : ./g.sh  43,37s user 8,28s system 36% cpu 2:23,39 total
            Log: 49s for parsing, 0.55s for files
*/