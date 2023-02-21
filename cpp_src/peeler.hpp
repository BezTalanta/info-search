#pragma once
#include <string>
#include <iostream>
#include <iomanip>
#include <typeinfo>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <vector>
#include <map>
#include <time.h>
#include <locale>
#include <codecvt>

#include "logger.hpp"

using namespace std;

// Magic numbers
// Chars to 
// size_t ALL_GAMES_POINTER = 2644;
//

// Peel part
// AP - absolute path

// Pure_dump folder path
string AP_GET_PURE = "../pure_dump/";

// Forward index with additional information
string AP_SET_FORWARD_FILE_READABLE = "data/forward_r.txt";
string AP_SET_FORWARD_FILE = "data/forward.bin";
string AP_SET_BACKWARD_FILE_READABLE = "data/backward_r.txt";
string AP_SET_BACKWARD_FILE = "data/backward.bin";
//

// Side part
unordered_set<char> ALLOWED_SIDE_CHARS({});
// Add quotes "
unordered_set<string> DISALLOWED_SIDE_CHARS({
	"@", ".", "?", "!", ",",
	"(", ")", ":", ";", "[",
	"]", "™", "®", "-", ">",
	"<", "&", " ", "+", "	",
	"…", "–"});

string alphabet = "abcd";
//
unordered_map<string, string> banned_words_permanent({
	{"lgbt", "lgbt community"},
	{"lgbtq", "lgbt community"},
	{"hentai", "18+"},
	{"anime", "I dont like anime"}
});

unordered_set<string> banned_words_common({
	"and", "the", "this", "for", "franchise", "games", "game"
});

// Function to reverse a string
void reverseStr(string& str, int n, int i){
	if(n<=i)
		return;
	//  Swapping the character 
	swap(str[i],str[n]);
  	reverseStr(str,n-1,i+1);  
}

string to_bin(int n){
	string result = "";
	while(n > 0){
		result += to_string(n % 2);
		n /= 2;
	}
	reverseStr(result, result.size() - 1, 0);
	return result;
}

class Peeler{
private:
	// TODO: local indexes, to delete them if document will be banned

	unordered_map<size_t, string> forward_index, ban_index;
	unordered_map<string, set<int>> backward_index;

	size_t current_doc_id;

	size_t str_to_size_t(const string& s) const{
		size_t result = 0;
		for (const char& c: s)
			result = result * 10 + c - '0';
		return result;
	}

	bool is_char_disallowed(const string& c) const {
		return DISALLOWED_SIDE_CHARS.find(c) != DISALLOWED_SIDE_CHARS.end();
	}

	bool is_char_disallowed(const char& c) const {
		return is_char_disallowed(string(1, c));
	}

	void save_to_ban(const string& reason = "Unknown reason"){
		ban_index[current_doc_id] = reason;
	}

	bool save_to_forward(const string& c){
		forward_index[current_doc_id] = c;
		// TODO: save to files in order
		return true;
	}

	bool save_to_backward(const string& c){
		LOG("Word to back: '" + c + "'")
		// TODO: banned words
		auto ban_permanent_find = banned_words_permanent.find(c);
		if (ban_permanent_find != banned_words_permanent.end()){
			save_to_ban(ban_permanent_find->second);
			throw runtime_error("This document has to be banned");
		}

		if (c.size() >= 3 && banned_words_common.find(c) == banned_words_common.end()){
			backward_index[c].insert(current_doc_id);
			return true;
		}
		return false;
	}

	// TODO: apostrof check and clear after that
	void parse_line(const string& line){
		string word = "";
		bool left_side = false;
		for (int i = 0; i < line.size(); ++i)
		{
			// char32_t current_symbol = tolower(line[i]);
			// u32string dout = U"";
			// dout += current_symbol;
			// wstring_convert<codecvt_utf8_utf16<char32_t>,char32_t> codecvt;
			// cout << "Status: " << codecvt.to_bytes(dout) << ' ' << int(current_symbol) << '\n';

			const char current_symbol = tolower(line[i]);
			LOG("Char: '" + string(1, current_symbol) + "'")
			// LOG("Char: " + current_symbol + ' ' + to_string(int(current_symbol)) + ' ' + to_bin(current_symbol))
			// cout << "Status: " << current_symbol << ' ' << int(current_symbol) << ' ' << to_bin(current_symbol) << '\n';
			if (current_symbol == ' ')
			// if (current_symbol == " ")
			{
				// LOG("\tWord: '" + word + "'")
				// cout << "Word: " <<  word << '\n';
				save_to_backward(word);
				left_side = false;
				word = "";
			}
			else if (is_char_disallowed(current_symbol))
			{
				if (i + 1 < line.size() &&
					!is_char_disallowed(line[i + 1]))
					word += current_symbol;

				// LOG("2: " + left_side)
				// cout << "2: " << left_side << '\n';
				// if (!left_side)
				// 	continue;
				// else if (i + 1 < line.size() &&
				// 	!is_char_disallowed(to_string(line[i + 1])))
				// {
				// 	word += current_symbol;
				// 	left_side = true;
				// }
			}
			else{
				word += current_symbol;
				left_side = true;
			}
		}
		save_to_backward(word);
	}

	void parse_name(ifstream& file){
		string s;
		getline(file, s);
		save_to_forward(s);
	}

	/*	
		Parse lines continuosly until will be found target line
		will_parse_lines defines what will be doing with parsed lines
		true - they will be parsed
		false - they will be ignored
		returns parsed lines amount
	*/
	size_t parse_until(ifstream& file, const string& target, bool will_parse_lines = true) {
		size_t lines_amount = 0;
		string s;
		while(getline(file, s)){
			if (s == target)
				return lines_amount;

			if (will_parse_lines){
				parse_line(s);
				++lines_amount;
			}
		}

		// LOG(to_string(current_doc_id) + " stalled with something!")
		return lines_amount;
	}

	void parse_and_skip(ifstream& file, const size_t& parse_line_amount, const size_t& skip_line_amount) {
		string s;
		for (size_t i = 0; i < parse_line_amount + skip_line_amount; ++i)
		{
			getline(file, s);
			if (i < parse_line_amount)
				parse_line(s);
		}
	}

	// bool is_line_require_to_skip(const string& str) const {
	// 	if (str == "The developers describe the content like this:" ||
	// 		str == "Publisher" || str == "Developer" || ){
	// 		return true;
	// 	}
	// }

public:
	void ban_index_out() const {
		cout << "Ban index:\n";
		for (auto item: ban_index)
			cout << '\t' << item.first << " - " << item.second << '\n';
	}

	void forward_index_out() const {
		cout << "Forward index:\n";
		for (auto item: forward_index)
			cout << '\t' << item.first << ": " << item.second << '\n';
	}

	void backward_index_out() const {
		cout << "Backward index:\n";
		for (auto [k, v]: backward_index)
		{
			cout << '\t' << k << " [";
			for(auto it = v.begin(); it != v.end(); ++it){
				cout << *it;
				if (it != --v.end())
					cout << ", ";
			}
			cout << "]\n";
		}
	}

	void out_all() const {
		backward_index_out();
		forward_index_out();
		ban_index_out();
	}

	void peel(const size_t& num, const bool is_truncate_required = false){
		current_doc_id = num;
		string file_name = to_string(num) + ".txt";

		ifstream file_in(AP_GET_PURE + file_name);

		// Change THAT!
		ofstream file_out_r, file_out;
		if (is_truncate_required)
			file_out_r.open(AP_SET_BACKWARD_FILE_READABLE);
		else
			file_out_r.open(AP_SET_BACKWARD_FILE_READABLE, ofstream::app);

		if (!file_in)
			throw runtime_error("Cannot open file" + string(AP_GET_PURE + file_name));
		if (!file_out)
			throw runtime_error("Cannot open file" + string(AP_SET_BACKWARD_FILE_READABLE));

		// file_in.seekg(2644);
		try{
			parse_until(file_in, "All Games", false);

			parse_until(file_in, "Community Hub");

			parse_name(file_in);

			parse_until(file_in, "Developer");

			parse_until(file_in, "Release Date:", false);

			parse_and_skip(file_in, 1, 0);

			parse_until(file_in, "Popular user-defined tags for this product:", false);

			// parse_and_skip(file_in, 1, 1);
			// // + date
			// // - Developer

			// parse_and_skip(file_in, 1, 1);
			// // + Developer name
			// // - Publisher

			// parse_and_skip(file_in, 0, 2);
			// // - Tags (?)
			// // - Popular user-defined ...

			parse_until(file_in, "Reviews");
		}
		catch(const runtime_error& error){
			return;
		}
	}

	void test(){
		string line = "…";
		char32_t current_symbol = tolower(line[0]);
		u32string dout = U"";
		dout += current_symbol;
		wstring_convert<codecvt_utf8_utf16<char32_t>,char32_t> codecvt;
		// cout << typeid(codecvt).name() << '\n';
		// cout << "Status: " << codecvt.to_bytes(dout) << ' ' << int(current_symbol) << '\n';
		ASSERT(is_char_disallowed(codecvt.to_bytes(dout)), true)
	}
};