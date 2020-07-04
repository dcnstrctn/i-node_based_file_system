#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "indirect.h"
#include "file.h"
#include "inode.h"
#include "directory.h"
#include <iostream>
#include <fstream>
#include <stack>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <vector>
using namespace std;

class FileSystem
{
public:
	void start();
	void initialize();

private:
	int first_empty = 0;
	int byte_of_first_block = 0;
	int num_of_empty_blocks = 0;
	stack<int> dir_stack;
	string current_path = "/";
	int res_block = 16384;
	void welcome();
	void test_inode();
	bool handle_command(string command);
	void allocate_new_space();
	void allocate_new_inodes();
	inode read_inode(int inode_num);
	void save_inode(inode i, int inode_num);
	void delete_inode(int inode_num);
	void save_indirect(indirect i, int pos);
	indirect read_indirect(int pos);
	int find_empty_block();
	int calculate_num_of_empty_blocks();
	int calculate_size_of_inode(int inode_num);
	void save_directory(directory dir, int inode_num);
	directory read_directory(int inode_num);
	string directory_to_string(directory dir);
	directory string_to_directory(string content);
	void save_block(string content, int pos);
	string read_block(int pos, int size);
	bool save_string_to_inode(string s, int inode_num);
	string read_string_from_inode(int inode_num);
	void add_dir(string name, int father_inode, directory dir);
	void add_file(string name, int father_inode, string file);
	void delete_dir(string name, int father_inode);
	void delete_file(string name, int father_inode);
	string file_to_string(string filename, int begin = 0, int size = -1);
	int get_current_dir_inode();
	void list_all();
	bool go_to_next_dir(string name);
	void go_back_dir();
	void go_back_to_root();
	void show_block_space();
	vector<string> spilt_string(string command, char sign);
	bool change_absolute_path(string path);
	void add_absolute_dir(string path);
	void delete_absolute_dir(string path);
	void add_absolute_file(string path, int size);
	void delete_absolute_file(string path);
	void cat_absolute_file(string path);
	void copy_file_to_file(string path1, string path2);
	void print_time(time_t t);
};

