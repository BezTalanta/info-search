#pragma once
#include <fstream>
#include <chrono>
#include <ctime>
#include <string>

using namespace std;

string LOG_FILE_NAME = "log.txt";
ofstream LOG_FILE(LOG_FILE_NAME, ios::app);

auto get_date(){
	std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);

    return to_string(now->tm_mday) + '/' + to_string(now->tm_mon + 1) + '/'
    	+ to_string(now->tm_year + 1900) + ' ' + to_string(now->tm_hour) +
    	':' + to_string(now->tm_min);
}

#define NEW_SESSION() LOG_FILE = ofstream(LOG_FILE_NAME);

#define LOG(x) LOG_FILE << __FILE__ << ':' << __LINE__ << \
	"[" << get_date() << "] " << x << '\n';

bool is_success_out = true;
#define ASSERT_TOGGLE() is_success_out = !is_success_out;
#define ASSERT(x, y) if (x == y && is_success_out) \
	cout << "Success\t|" << __FILE__ << \
	"|" << __LINE__ << "| " << x << " == " << y << '\n'; \
	else if(x != y) cout << "Error\t|" << __FILE__ << "|" << __LINE__ \
		<< "| " << x << " != " << y << '\n';
