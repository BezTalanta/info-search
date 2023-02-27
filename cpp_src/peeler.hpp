#pragma once
#include <string>
#include <iostream>
#include <iomanip>
#include <typeinfo>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <algorithm>
#include <vector>
#include <map>
#include <time.h>
#include <locale>
#include <codecvt>

#include "logger.hpp"

using namespace std;

// Peel part
// AP - absolute path

// Pure_dump folder path
string AP_GET_PURE = "../pure_dump/";

// All indexes files
string FORWARD_NAME = "data/forward.txt";
string BANNED_NAME = "data/banned.txt";
string P1_NAME = "data/p1.txt";
string P2_NAME = "data/p2.txt";
string P3_NAME = "data/p3.txt";

// Side part
unordered_set<string> DISALLOWED_SIDE_CHARS({
	"@", ".", "?", "!", ",",
	"(", ")", ":", ";", "[",
	"]", "™", "®", "-", ">",
	"<", "&", " ", "+", "	",
	"…", "–", "'", "\"", "_",
	"*", "%", "\\", "/", "#",
	"`",
});

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
	"are", "which", "its", "that", "has",
	"with", "is", "pack", "edition",
	"where", "steam", "not", "000"
});

class Peeler{
public:
	map<size_t, string> forward_index;
	unordered_map<size_t, string> ban_index;
	unordered_map<string, set<int>> p1, p2, p3;

	// Using for search
	set<int> s_p1, s_p2, s_p3;

	unordered_map<string, set<int>> p1_mem, p2_mem, p3_mem;

	size_t current_doc_id, current_priority;

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
		return true;
	}

	bool save_to_p(const string& c){
		if (c.empty())
			return false;

		MIDDLE_LEVEL("Word to back: '" + c + "'")
		auto ban_permanent_find = banned_words_permanent.find(c);
		if (ban_permanent_find != banned_words_permanent.end()){
			save_to_ban(ban_permanent_find->second);
			throw runtime_error("This document has to be banned");
		}

		if (c.size() >= 3 &&
			banned_words_common.find(c) == banned_words_common.end()){
			if (current_priority == 1)
				p1_mem[c].insert(current_doc_id);
			else if (current_priority == 2)
				p2_mem[c].insert(current_doc_id);
			else
				p3_mem[c].insert(current_doc_id);
			return true;
		}
		return false;
	}

	size_t skip_check_fix(const vector<int>& v) const{
		if (v[0] == -30 && v[1] == -128 && v[2] == -117) // ​
			return 2;		
		if (v[0] == -30 && v[1] == -128 && v[2] == -90) // …
			return 2;		
		if (v[0] == -30 && v[1] == -128 && v[2] == -108) // —
			return 2;		
		if (v[0] == -30 && v[1] == -128 && v[2] == -103) // ’
			return 2;
		if (v[0] == -30 && v[1] == -128 && v[2] == -104) // ‘
			return 2;
		if (v[0] == -30 && v[1] == -124 && v[2] == -94) // ™
			return 2;
		if (v[0] == -30 && v[1] == -128 && v[2] == -99) // ”
			return 2;
		if (v[0] == -30 && v[1] == -128 && v[2] == -100) // “
			return 2;
		if (v[0] == -62 && v[1] == -82) // ®
			return 1;
		if (v[0] == -62 && v[1] == -78) // ²
			return 1;
		return 0;
	}

	void parse_line(const string& line){
		string word = "";
		bool left_side = false;
		for (int i = 0; i < line.size(); ++i)
		{
			char current_symbol = line[i];
			LOW_LEVEL("Char: '" + string(1, current_symbol) + "'")

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
			if (current_symbol == ' ' || is_char_disallowed(current_symbol))
			{
				MIDDLE_LEVEL("\tWord: '" + word + "'")
				save_to_p(word);
				left_side = false;
				word = "";
			}
			else{
				word += current_symbol;
				left_side = true;
			}
		}

		MIDDLE_LEVEL("\tWord: '" + word + "'")
		save_to_p(word);
	}

	/*
		Parse name to forward index and p1_mem
	*/
	void parse_name(ifstream& file){
		string s;
		getline(file, s);
		save_to_forward(s);
		current_priority = 1;
		parse_line(s);
	}

	/*
		Parse release date to p1_mem
	*/
	void parse_date(ifstream& file){
		string s;
		getline(file, s);
		current_priority = 2;
		string current_word = "";
		for (char& c: s){
			c = tolower(c);
			if (c == ' '){
				save_to_p(current_word);
				current_word = "";
			}
			else if (c != ',')
				current_word += c;
		}
		save_to_p(current_word);
	}

	/*
		Parse release date to p1_mem
	*/
	void parse_description(ifstream& file){
		string s;
		getline(file, s);
		current_priority = 3;
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

	void merge_p(unordered_map<string, set<int>>& mem,
		unordered_map<string, set<int>>& orig){
		for (auto [k, v]: mem){
			auto find_it = orig.find(k);
			if (find_it != orig.end()){
				find_it->second.merge(v);
			}
			else{
				orig[k] = v;
			}
		}
	}

	void merge_memory(bool only_clear = false){
		if (!only_clear){
			merge_p(p1_mem, p1);
			merge_p(p2_mem, p2);
			merge_p(p3_mem, p3);
		}
		p1_mem.clear();
		p2_mem.clear();
		p3_mem.clear();
	}

	/*
		Work with files
	*/
	void change_line(fstream& file, const string& msg, size_t line_num){
		file.seekp(0);
		string trash;
		for (int i = 0; i < line_num - 1; ++i)
			getline(file, trash);
		file << msg;
		file.seekp(ios_base::end);
	}


	/*
		map<size_t, string> forward_index; 				- 1
		unordered_map<size_t, string> ban_index;		- 2
		unordered_map<string, set<int>> p1, p2, p3;		- 3
	*/
	template<typename T>
	void fill_file(fstream& file, const T& item, int type){
		if (type == 1){
			int prev = 0;
			for (const auto& [k, v]: item){
				for (;prev < k - 1; ++prev)
					file << '\n';
				file << v << '\n';
				prev = k;
			}
		}
		else if (type == 2){
			for (const auto& [k, v]: item)
				file << k << ' ' << v << '\n';
		}
	}

	void fill_file_p(fstream& file,
		const unordered_map<string, set<int>>& item){
		for (const auto& [k, v]: item){
			file << k << ' ';
			for (const auto& select: v)
				file << select << ' ';
			file << '\n';
		}
	}

	void write_all(){
		fstream forward(FORWARD_NAME, ios::in|ios::out|ios::trunc);
		fstream ban(BANNED_NAME, ios::in|ios::out|ios::trunc);

		fstream p1f(P1_NAME, ios::in|ios::out|ios::trunc);
		fstream p2f(P2_NAME, ios::in|ios::out|ios::trunc);
		fstream p3f(P3_NAME, ios::in|ios::out|ios::trunc);

		if (!forward || !ban || !p1f || !p2f || !p3f)
			throw runtime_error("Some files hadn't been opened!");

		fill_file(forward, forward_index, 1);
		fill_file(ban, ban_index, 2);
		fill_file_p(p1f, p1);
		fill_file_p(p2f, p2);
		fill_file_p(p3f, p3);

		HIGH_LEVEL("All data successfully saved into a files");
	}

	void read_from_file_p(ifstream& file,
			unordered_map<string, set<int>>& container){
		string s;
		// int amount = 0;
		while(getline(file, s)){
			string current_word = "", word = "";
			for (const char& c: s)
				if (c != ' '){
					current_word += c;
				}
				else if (word.empty()){
					word = current_word;
					container[word] = {};
					current_word = "";
				}
				else if (!word.empty()){
					container[word].insert(stoi(current_word));
					current_word = "";
				}
			if (!current_word.empty())
				container[word].insert(stoi(current_word));

			// if (++amount > 200){
			// 	return;
			// }
		}
	}

	void load_indexes(){
		cout << "Loading indexes...\n";
		ifstream file("data/p1.txt", ios::in);
		ifstream file2("data/p2.txt", ios::in);
		ifstream file3("data/p3.txt", ios::in);

		read_from_file_p(file, p1);
		read_from_file_p(file2, p2);
		read_from_file_p(file3, p3);
		cout << "Indexes had been downloaded successfully!\n";
	}
	//

	// Search engine
	void search_out_to_file(){
		fstream file("search.out", ios::out);

		for (const auto& item: s_p1)
			file << item << '\n';
	}

	void search_from_indexes(const string& s){
		auto p1_it = p1.find(s);
		bool p1_found = (p1_it != p1.end());
		cout << p1_found << '\n';
		if (p1_found)
			s_p1 = p1_it->second;

		search_out_to_file();
	}

	void search(){
		ifstream file("search.in", ios::in);

		string search_line;
		getline(file, search_line);

		stringstream ss(search_line);
		string searchable_word;
		while(ss >> searchable_word){
			search_from_indexes(searchable_word);
		}
	}
	//

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
		cout << "p1_mem index:\n";
		for (auto [k, v]: p1)
		{
			cout << '\t' << k << " [";
			for(auto it = v.begin(); it != v.end(); ++it){
				cout << *it;
				if (it != --v.end())
					cout << ", ";
			}
			cout << "]\n";
		}
		cout << "p2_mem index:\n";
		for (auto [k, v]: p2)
		{
			cout << '\t' << k << " [";
			for(auto it = v.begin(); it != v.end(); ++it){
				cout << *it;
				if (it != --v.end())
					cout << ", ";
			}
			cout << "]\n";
		}
		cout << "p3_mem index:\n";
		for (auto [k, v]: p3)
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

	void peel(const size_t& num){
		current_doc_id = num;
		ifstream file_in(AP_GET_PURE + to_string(num) + ".txt");
		if (!file_in)
			HIGH_LEVEL("File " + to_string(num) + ".txt doesn't exist");

		try{
			string type_of_doc = get_line_by_num(file_in, 180);

			if (type_of_doc != "All Games" ){
				if (type_of_doc == "All Products >"){
					parse_and_skip(file_in, 0, 1);
					parse_name(file_in);
					return;
				}
			}

			parse_until(file_in, "Community Hub", false);

			parse_name(file_in);
			// + Game name in forward list
			// + Game name in p1_mem

			parse_until(file_in, "Released", false);

			parse_date(file_in);
			// + Game's released date

			// priority == 3
			parse_description(file_in);
			// + Game's description

			parse_until(file_in, "All Reviews:", false);
			parse_and_skip(file_in, 1, 0);
			// + Game's reviews

			parse_until(file_in, "Popular user-defined tags for this product:", false);
			parse_until(file_in, "Reviews");
			// + Game's tags

			// // OLD VERISON WITHOUR ZONE SEARCH
			// parse_until(file_in, "Community Hub", false);

			// parse_name(file_in);
			// // + Game name in forward list

			// parse_until(file_in, "Developer");
			// // + Game name in backward list

			// parse_until(file_in, "Release Date:", false);

			// parse_and_skip(file_in, 1, 0);
			// // + date

			// parse_until(file_in, "Popular user-defined tags for this product:", false);

			// // parse_and_skip(file_in, 1, 1);
			// // // + date
			// // // - Developer

			// // parse_and_skip(file_in, 1, 1);
			// // // + Developer name
			// // // - Publisher

			// // parse_and_skip(file_in, 0, 2);
			// // // - Tags (?)
			// // // - Popular user-defined ...

			// parse_until(file_in, "Reviews");
			// // + tags

			// parse_and_skip(file_in, 0, 1);
			// // - All Reviews:

			// parse_and_skip(file_in, 1, 0);

			// parse_until(file_in, "About This Content", false);

			// parse_and_skip(file_in, 1, 0);
		}
		catch(const runtime_error& error){
			merge_memory(true);
			return;
		}
		merge_memory();
	}

	void test(){
		string line = "²";
		// current_priority = 3;
		// parse_line(line);
		// return;

		size_t sz = line.size();
		for (int i = 0; i < sz; ++i)
			cout << line[i] << ' ' << int(line[i]) << '\n';
	}
};

/*
	🐱 - prevent it
*/