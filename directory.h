#pragma once
#include <map>
#include <string>
using namespace std;

class directory
{
public:
	bool add_dir(string name, int address);
	bool add_file(string name, int address);
	int delete_dir(string name);
	int delete_file(string name);
	bool has_dir(string name);
	bool has_file(string name);
	void list_all_dir();
	void list_all_file();
	void list_all();
	map<string, int>& get_dir();
	map<string, int>& get_file();
private:
	map<string, int> dir;
	map<string, int> file;
};

