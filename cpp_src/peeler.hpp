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
#include "texter.hpp"

using namespace std;

// Magic numbers
// Chars to 
// size_t ALL_GAMES_POINTER = 2644;
//

// Peel part
// AP - absolute path

// Pure_dump folder path
string AP_GET_PURE = "../pure_dump/";

// Side part
unordered_set<string> DISALLOWED_SIDE_CHARS({
	"@", ".", "?", "!", ",",
	"(", ")", ":", ";", "[",
	"]", "™", "®", "-", ">",
	"<", "&", " ", "+", "	",
	"…", "–", "'", "\"", "_",
	"*", "%", "\\", "/"});

//
unordered_map<string, string> banned_words_permanent({
	{"lgbt", "lgbt community"},
	{"lgbtq", "lgbt community"},
	{"hentai", "18+ Little anime girls"},
	{"anime", "I dont like anime"},
	{"nsfw", "18+ NSFW"},
});

unordered_set<string> banned_words_common({
	"and", "the", "this", "for",
	"franchise", "games", "game",
	"•", "there", "–", "than", "then",
	"are", "which",
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
		// LOG("Word to back: '" + c + "'")

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

	size_t skip_check_fix(const vector<int>& v) const{
		// cout << "Vector: ";
		// for (auto item: v)
		// {
		// 	cout << item << ' ';
		// }
		// cout << '\n';

		if (v[0] == -30 && v[1] == -128 && v[2] == -104) // ‘
			return 2;
		return 0;
	}

	// TODO: apostrof check and clear after that
	void parse_line(const string& line){
		string word = "";
		bool left_side = false;
		for (int i = 0; i < line.size(); ++i)
		{
			char current_symbol = line[i];
			// LOG("Char: '" + string(1, current_symbol) + "'")

			// cout << "Char: '" << current_symbol << "' " << int(current_symbol) << '\n';

			// Be carefully
			if (current_symbol < 0){
				vector<int> unicode_symbol;
				unicode_symbol.push_back(current_symbol);
				for (int j = i + 1; j < line.size() && j - i < 4; ++j)
					unicode_symbol.push_back(line[j]);
				size_t calc_res = skip_check_fix(unicode_symbol);
				if (calc_res){
					i += calc_res;
					continue;
				}
			}

			current_symbol = tolower(current_symbol);
			if (current_symbol == ' ')
			{
				// LOG("\tWord: '" + word + "'")
				save_to_backward(word);
				left_side = false;
				word = "";
			}
			else if (is_char_disallowed(current_symbol))
			{
				if (!(current_symbol == '\'' || current_symbol == '-')){
					LOG("2.1: This symbol cannot be in a word: '" +
						string(1, current_symbol) + "' line was: '" +
						line + "'")
					save_to_backward(word);
					left_side = false;
					word = "";
					continue;
				}

				if (left_side && i + 1 < line.size() &&
					!is_char_disallowed(line[i + 1])){
					// LOG("2: '" + string(1, current_symbol) + "' is disallowed but the next one no: '" + line[i + 1] + "'")
					word += current_symbol;
				}
				else{
					// LOG("2: '" + string(1, current_symbol) + "' is disallowed and the next one too")
				}
			}
			else{
				// cout << "Flag3 " << line.size() << " " << i << '\n';
				word += current_symbol;
				left_side = true;
			}
		}
		save_to_backward(word);
	}

	void parse_name(ifstream& file, bool will_add_backward = false){
		string s;
		getline(file, s);
		save_to_forward(s);
		if (will_add_backward)
			parse_line(s);
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

	string get_line_by_num(ifstream& file, const size_t& num) const{
		string s;
		for (size_t i = 0; i < num; ++i)
			getline(file, s);
		return s;
	}
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
			// parse_until(file_in, "All Games", false);

			string type_of_doc = get_line_by_num(file_in, 180);

			if (type_of_doc != "All Games"){
				LOG(to_string(current_doc_id) + " type is: " + type_of_doc)

				if (type_of_doc == "All Products >"){
					parse_and_skip(file_in, 0, 1);
					parse_name(file_in, true);
				}
				return;
			}

			parse_until(file_in, "Community Hub", false);

			parse_name(file_in);
			// + Game name in forward list

			parse_until(file_in, "Developer");
			// + Game name in backward list

			parse_until(file_in, "Release Date:", false);

			parse_and_skip(file_in, 1, 0);
			// + date

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
			// + tags

			parse_and_skip(file_in, 0, 1);
			// - All Reviews:

			parse_and_skip(file_in, 1, 0);

			parse_until(file_in, "About This Content", false);

			parse_and_skip(file_in, 1, 0);
		}
		catch(const runtime_error& error){
			return;
		}
	}

	void test(){
		string line = "‘test";
		parse_line(line);
		return;

		size_t sz = line.size();
		for (int i = 0; i < sz; ++i)
			cout << line[i] << ' ' << int(line[i]) << '\n';
	}
};

/*
	‘ -30 -128 -104  TODO check " pets‘ " <- it doesnot working very well
	“ TODO: make it
*/