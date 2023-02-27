#pragma once
#include <fstream>
#include <chrono>
#include <ctime>
#include <string>
#include <stdarg.h>
#include <typeinfo>
#include <utility>

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

/*
	3 - high level
	2 - middle level
	1 - low level
*/
int log_level = 3;
bool only_one_level = false;

void LOG(int level, const string& msg){
	if (level == 3 &&
		(log_level == 3 || (!only_one_level && (level > log_level))))
		LOG_FILE << "HIGH" << '|';
	else if (level == 2 &&
		(log_level == 2 || (!only_one_level && (level > log_level))))
		LOG_FILE << "MIDDLE" << '|';
	else if (level == 1 &&
		(log_level == 1 || (!only_one_level && (level > log_level))))
		LOG_FILE << "LOW" << '|';
	else
		return;
	LOG_FILE << __FILE__ << ':' << __LINE__ << \
		"[" << get_date() << "] " << msg << '\n';
}

#define HIGH_LEVEL(arg) LOG(3, arg);
#define MIDDLE_LEVEL(arg) LOG(2, arg);
#define LOW_LEVEL(arg) LOG(1, arg);

bool is_success_out = true;
#define ASSERT_TOGGLE() is_success_out = !is_success_out;
#define ASSERT(x, y) if (x == y && is_success_out) \
	cout << "Success\t|" << __FILE__ << \
	"|" << __LINE__ << "| " << x << " == " << y << '\n'; \
	else if(x != y) cout << "Error\t|" << __FILE__ << "|" << __LINE__ \
		<< "| " << x << " != " << y << '\n';
