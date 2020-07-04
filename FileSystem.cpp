#include "FileSystem.h"

void FileSystem::start()
{
	welcome();
	initialize();

	byte_of_first_block = 16384 * sizeof(inode);
	num_of_empty_blocks = calculate_num_of_empty_blocks();

	while (1)
	{
		cout << endl << current_path << ": ";
		string command;
		getline(cin, command);
		if (handle_command(command))
			break;
	}
}

void FileSystem::initialize()
{
	ifstream in("storage.dat");
	if (!in.is_open())
	{
		allocate_new_space();
		allocate_new_inodes();
		save_directory(directory(), 0);
	}
	in.close();	
}

void FileSystem::welcome()
{
	cout << "Welcome to the i-node-based file system by\n\n"
		<< "                           @name        @student_id\n"
		<< "                           HaiMing Luo  201730614127\n"
		<< "                           MingCan Geng 201736621075\n"
		<< "                           ChenShu Xu   201730614370\n\n"
		<< "Please close the system by command avoiding the loss of your work!" << endl;

	cout << endl << "                                Command helper:" << endl
		<< "                                dir --- show information of files in the current working directory" << endl
		<< "                                sum --- show the numbers of blocks used and unused" << endl
		<< "                               exit --- exit the system" << endl
		<< "                       cat filename --- show the content of a file" << endl
		<< "                  changeDir dirname --- change the current working directory" << endl
		<< "                  createDir dirname --- create a new directory" << endl
		<< "                  deleteDir dirname --- delete a directory and all files in it" << endl
		<< "                deleteFile filename --- delete a file" << endl
		<< "             cp filename1 filename2 --- copy the content of file1 to file2" << endl
		<< "createFile filename fileSize(in KB) --- create a new file" << endl;
}

void FileSystem::test_inode()
{
	string one;
	string two = one;
	int new_block = 0;
	while (one == two)
	{
		new_block = find_empty_block();
		one = file(1).get_content();
		save_string_to_inode(one, new_block);
		two = read_string_from_inode(new_block);
		cout << new_block << endl;
	}
	cout << "Problem block£º " << new_block << endl;
}

bool FileSystem::handle_command(string command)
{
	vector<string> part = spilt_string(command, ' ');
	if (part.empty())
		return false;
	else if (part[0] == "changeDir" || part[0] == "cd")
	{
		if (part.size() > 1)
		{
			if (part[1] == "..")
				go_back_dir();
			else if (part[1][0] == '/')
				change_absolute_path(part[1]);
			else
				go_to_next_dir(part[1]);
		}
	}
	else if (part[0] == "createDir"||part[0] == "mkdir")
	{
		if (part.size() > 1)
		{
			if (part[1][0] == '/')
				add_absolute_dir(part[1]);
			else
				add_dir(part[1], get_current_dir_inode(), directory());
		}
	}
	else if (part[0] == "createFile"||part[0] == "mkfile")
	{
		if (part.size() > 2)
		{
			int size = atoi(part[2].c_str());
			if (size > 266)
			{
				cout << "The file size has exceeded the max file size 266KB." << endl;
			}
			else
			{
				if (part[1][0] == '/')
					add_absolute_file(part[1], size);
				else
					add_file(part[1], get_current_dir_inode(), file(size).get_content());
			}
		}
		else cout << "Command is illegal\n";
	}
	else if (part[0] == "deleteDir")
	{
		if (part.size() > 1)
		{
			if (part[1][0] == '/')
				delete_absolute_dir(part[1]);
			else
				delete_dir(part[1], get_current_dir_inode());
		}
	}
	else if (part[0] == "deleteFile")
	{
		if (part.size() > 1)
		{
			if (part[1][0] == '/')
				delete_absolute_file(part[1]);
			else
				delete_file(part[1], get_current_dir_inode());
		}
	}
	else if (part[0] == "cat")
	{
		if (part.size() > 1)
		{
			if (part[1][0] == '/')
				cat_absolute_file(part[1]);
			else
			{
				directory dir = read_directory(get_current_dir_inode());
				if (dir.has_file(part[1]))
					cout << read_string_from_inode(dir.get_file()[part[1]]) << endl;
				else
					cout << "There is no such file" << endl;
			}
		}
	}
	else if (part[0] == "dir")
	{
		list_all();
	}
	else if (part[0] == "sum")
	{
		show_block_space();
	}
	else if (part[0] == "cp")
	{
		if (part.size() > 2)
		{
			copy_file_to_file(part[1], part[2]);
		}
	}
	else if (part[0] == "exit")
	{
		return true;
	}
	else
	{
	    cout << "Command not found!" << endl;
	}
	return false;
}

void FileSystem::allocate_new_space()
{
	ofstream out("storage.dat", ios::out | ios::binary); // ios::in | ios::out
	indirect original;
	for (int i = 0; i < 16384; i++)
	{
		out.write((char*)& original, sizeof(original));
	}
	out.close();
}

void FileSystem::allocate_new_inodes()
{
	ofstream out("storage.dat", ios::in | ios::out | ios::binary);
	inode original;
	int useless = 16384 - 16384 * sizeof(original) / 1024.0;
	for (int i = 0; i < 16384; i++)
	{
		if (i == useless) original.set_occupied(true);
		out.write((char*)& original, sizeof(original));
	}
	out.close();
}

inode FileSystem::read_inode(int inode_num)
{
	ifstream in("storage.dat", ios::in | ios::binary);
	inode temp;
	in.seekg(inode_num * sizeof(temp));
	in.read((char*)& temp, sizeof(temp));
	in.close();
	return temp;
}

void FileSystem::save_inode(inode i, int inode_num)
{
	inode before = read_inode(inode_num);
	num_of_empty_blocks += before.is_occupied() - i.is_occupied();
	i.set_modified_time(time(0));
	ofstream out("storage.dat", ios::in | ios::out | ios::binary);
	out.seekp(inode_num * sizeof(i));
	out.write((char*)& i, sizeof(i));
	out.close();
}

void FileSystem::delete_inode(int inode_num)
{
	inode first = read_inode(inode_num);
	save_inode(inode(), inode_num);
	string result;
	for (int i = 0; i < 10; i++)
	{
		int pos = first.get_direct_block_address()[i];
		if (pos < 0)
		{
			return;
		}
		save_inode(inode(), pos);
	}
	int indirect_pos = first.get_indirect_block_address();
	if (indirect_pos < 0)
		return;
	inode indirect_inode = read_inode(indirect_pos);
	indirect table = read_indirect(indirect_pos);
	save_inode(inode(), indirect_pos);
	for (int i = 0; i < indirect_inode.get_byte_size() / 4; i++)
	{
		int pos = table.i[i];
		save_inode(inode(), pos);
	}
	first_empty = 1;
	return;
}

void FileSystem::save_indirect(indirect i, int pos)
{
	ofstream out("storage.dat", ios::in | ios::out | ios::binary);
	out.seekp(byte_of_first_block + pos * 1024);
	out.write((char*)& i, sizeof(i));
	out.close();
}

indirect FileSystem::read_indirect(int pos)
{
	ifstream in("storage.dat", ios::in | ios::binary);
	indirect temp;
	in.seekg(byte_of_first_block + pos * 1024);
	in.read((char*)& temp, sizeof(temp));
	in.close();
	return temp;
}

int FileSystem::find_empty_block()
{
	inode temp;
	do
	{
		first_empty++;
		while (first_empty == 26 || first_empty == 282 || first_empty == 537 || first_empty == 538 ||
			first_empty == 639 || first_empty == 794 || first_empty == 2585)
			first_empty++;
		temp = read_inode(first_empty);
	}
	while (temp.is_occupied() && first_empty < 16383);
	if (temp.is_occupied())
	{
		cout << "There is no any empty block." << endl;
		return -1;
	}
	else
	{
		return first_empty;
	}
}

int FileSystem::calculate_num_of_empty_blocks()
{
	int num = 0;
	for (int i = 0; i < 16384; i++)
	{
		if (!read_inode(i).is_occupied())
			num++;
	}
	return num;
}

int FileSystem::calculate_size_of_inode(int inode_num)
{
	int size = 0;
	inode first = read_inode(inode_num);
	for (int i = 0; i < 10; i++)
	{
		int pos = first.get_direct_block_address()[i];
		if (pos < 0)
		{
			return size;
		}
		inode temp = read_inode(pos);
		size += temp.get_byte_size();
	}
	int indirect_pos = first.get_indirect_block_address();
	inode indirect_inode = read_inode(indirect_pos);
	indirect table = read_indirect(indirect_pos);
	for (int i = 0; i < indirect_inode.get_byte_size() / 4; i++)
	{
		int pos = table.i[i];
		inode temp = read_inode(pos);
		size += temp.get_byte_size();
	}
	return size;
}

void FileSystem::save_directory(directory dir, int inode_num)
{
	string content = directory_to_string(dir);
	if (content != read_string_from_inode(inode_num))
		save_string_to_inode(content, inode_num);
}

directory FileSystem::read_directory(int inode_num)
{
	string temp = read_string_from_inode(inode_num);
	directory dir = string_to_directory(temp);
	return dir;
}

string FileSystem::directory_to_string(directory dir)
{
	string content;
	map<string, int>::iterator iter;
	for (iter = dir.get_dir().begin(); iter != dir.get_dir().end(); iter++)
	{
		content += char(17);
		content += iter->first;
		content += char(17);
		content += to_string(iter->second);
	}
	for (iter = dir.get_file().begin(); iter != dir.get_file().end(); iter++)
	{
		content += char(18);
		content += iter->first;
		content += char(18);
		content += to_string(iter->second);
	}
	return content;
}

directory FileSystem::string_to_directory(string content)
{
	directory dir;
	int left = 0, right = 0;
	while (left < content.length())
	{
		int record = content[left];
		string key;
		int value;
		left++;
		right = left + 1;
		while (content[right] != char(17) && content[right] != char(18))
		{
			right++;
		}
		key = content.substr(left, right - left);
		left = right + 1;
		right = left + 1;
		while (right < content.length() && content[right] != char(17) && content[right] != char(18))
		{
			right++;
		}
		value = atoi(content.substr(left, right - left).c_str());
		if (record == 17)
		{
			dir.add_dir(key, value);
		}
		else
		{
			dir.add_file(key, value);
		}
		left = right;
	}
	return dir;
}

void FileSystem::save_block(string content, int pos)
{
	ofstream out("storage.dat", ios::in |ios::out | ios::binary);
	out.seekp(byte_of_first_block + pos * 1024);
	out.write(content.c_str(), content.length());
	out.close();
}

string FileSystem::read_block(int pos, int size)
{
	return file_to_string("storage.dat", byte_of_first_block + pos * 1024, size);
}

bool FileSystem::save_string_to_inode(string s, int inode_num)
{
	bool reset_created = read_inode(inode_num).is_occupied();
	time_t temp_created_time = read_inode(inode_num).get_created_time();

	delete_inode(inode_num);
	if (read_inode(inode_num).is_occupied() || s.length() > 266 * 1024 || s.length() > num_of_empty_blocks * 1024)
	{
		return false;
	}
	int section_num = s.length() / 1024;
	int res = s.length() % 1024;
	inode first;
	if (reset_created)
	{
		first.set_created_time(temp_created_time);
	}
	first.set_occupied(true);
	first.set_byte_size(section_num == 0 ? res : 1024);
	for (int i = 0; i < 10 && i < section_num; i++)
	{
		int new_block = (i == 0 ? inode_num : find_empty_block());
		first.get_direct_block_address()[i] = new_block;
		inode temp;
		temp.set_occupied(true);
		temp.get_direct_block_address()[0] = new_block;
		temp.set_byte_size(1024);
		save_block(s.substr(i * 1024, 1024), new_block);
		save_inode(temp, new_block);
	}
	if (section_num < 10)
	{
		if (res > 0)
		{
			int new_block = (section_num == 0 ? inode_num : find_empty_block());
			first.get_direct_block_address()[section_num] = new_block;
			inode temp;
			temp.set_occupied(true);
			temp.get_direct_block_address()[0] = new_block;
			temp.set_byte_size(res);
			save_block(s.substr((section_num) * 1024, res), new_block);
			if (section_num == 0 && reset_created)
			{
				temp.set_created_time(temp_created_time);
			}
			save_inode(temp, new_block);
			if (section_num == 0)
			{
				return true;
			}
		}
		save_inode(first, inode_num);
		return true;
	}
	section_num -= 10;
	int indirect_pos = find_empty_block();
	first.set_indirect_block_address(indirect_pos);
	inode indirect_inode;
	indirect_inode.set_occupied(true);
	indirect_inode.get_direct_block_address()[0] = indirect_pos;
	indirect_inode.set_byte_size(4 * (section_num + (res > 0 ? 1 : 0)));
	save_inode(indirect_inode, indirect_pos);
	indirect indirect_table;
	for (int i = 0; i < section_num; i++)
	{
		int new_block = find_empty_block();
		indirect_table.i[i] = new_block;
		inode temp;
		temp.set_occupied(true);
		temp.get_direct_block_address()[0] = new_block;
		temp.set_byte_size(1024);
		save_block(s.substr((10 + i) * 1024, 1024), new_block);
		save_inode(temp, new_block);
	}
	if (res > 0)
	{
		int new_block = find_empty_block();
		indirect_table.i[section_num] = new_block;
		inode temp;
		temp.set_occupied(true);
		temp.get_direct_block_address()[0] = new_block;
		temp.set_byte_size(res);
		save_block(s.substr((10 + section_num) * 1024, res), new_block);
		save_inode(temp, new_block);
	}
	save_inode(first, inode_num);
	save_indirect(indirect_table, indirect_pos);
	return true;
}

string FileSystem::read_string_from_inode(int inode_num)
{
	inode first = read_inode(inode_num);
	string result;
	for (int i = 0; i < 10; i++)
	{
		int pos = first.get_direct_block_address()[i];
		if (pos < 0)
		{
			return result;
		}
		inode temp = read_inode(pos);
		result += read_block(pos, temp.get_byte_size());
	}
	int indirect_pos = first.get_indirect_block_address();
	inode indirect_inode = read_inode(indirect_pos);
	indirect table = read_indirect(indirect_pos);
	for (int i = 0; i < indirect_inode.get_byte_size() / 4; i++)
	{
		int pos = table.i[i];
		inode temp = read_inode(pos);
		result += read_block(pos, temp.get_byte_size());
	}
	return result;
}

void FileSystem::add_dir(string name, int father_inode, directory dir)
{
	directory father = read_directory(father_inode);
	if (!father.has_dir(name))
	{
		int new_inode = find_empty_block();
		save_string_to_inode(directory_to_string(dir), new_inode);
		father.add_dir(name, new_inode);
		save_directory(father, father_inode);
	}
	else
		cout << "This directory exist." << endl;
}

void FileSystem::add_file(string name, int father_inode, string file)
{
	
	directory father = read_directory(father_inode);
	if (!father.has_file(name))
	{
		int new_inode = find_empty_block();
		save_string_to_inode(file, new_inode);
		father.add_file(name, new_inode);
		save_directory(father, father_inode);
	}
	else
		cout << "This file exist." << endl;
}

void FileSystem::delete_dir(string name, int father_inode)
{
	directory father = read_directory(father_inode);
	int inode_pos = father.delete_dir(name);
	if (inode_pos < 0)
	{
		cout << "There is no such directory." << endl;
	}
	else
	{
		directory this_dir = read_directory(inode_pos);
		map<string, int> son_dir = this_dir.get_dir();
		map<string, int> son_file = this_dir.get_file();
		map<string, int>::iterator iter;
		for (iter = son_dir.begin(); iter != son_dir.end(); iter++)
		{
			delete_dir(iter->first, inode_pos);
		}
		for (iter = son_file.begin(); iter != son_file.end(); iter++)
		{
			delete_file(iter->first, inode_pos);
		}
		delete_inode(inode_pos);
		save_directory(father, father_inode);
	}
}

void FileSystem::delete_file(string name, int father_inode)
{
	directory father = read_directory(father_inode);
	int inode_pos = father.delete_file(name);
	if (inode_pos < 0)
	{
		cout << "There is no such file." << endl;
	}
	else
	{

		delete_inode(inode_pos);
		save_directory(father, father_inode);
	}
}

string FileSystem::file_to_string(string filename, int begin, int size)
{
	ifstream in(filename);
	ostringstream buffer;
	char c;
	for (int i = 0; i < begin; i++)
		in.get(c);
	if (size == -1)
	{
		while (buffer && in.get(c))
			buffer.put(c);
	}
	else
	{
		for (int i = 0; i < size; i++) {
			in.get(c);
			buffer.put(c);
		}
	}
	return buffer.str();
}

int FileSystem::get_current_dir_inode()
{
	if (dir_stack.empty())
		return 0;
	else
		return dir_stack.top();
}

void FileSystem::list_all()
{
	directory current = read_directory(get_current_dir_inode());
	map<string, int>::iterator iter;
	cout << "Directory: " << endl;
	for (iter = current.get_dir().begin(); iter != current.get_dir().end(); iter++)
	{
		time_t created = read_inode(iter->second).get_created_time();
		tm* ctm = localtime(&created);
		time_t modified = read_inode(iter->second).get_modified_time();
		tm* mtm = localtime(&modified);
		cout << iter->first << " (size: " << calculate_size_of_inode(iter->second) << " bytes | created time: ";
		print_time(read_inode(iter->second).get_created_time());
		cout << " | modified time: ";
		print_time(read_inode(iter->second).get_modified_time());
		cout << ")" << endl;
	}
	cout << endl << "File: " << endl;
	for (iter = current.get_file().begin(); iter != current.get_file().end(); iter++)
	{
		time_t created = read_inode(iter->second).get_created_time();
		tm* ctm = localtime(&created);
		time_t modified = read_inode(iter->second).get_modified_time();
		tm* mtm = localtime(&modified);
		cout << iter->first << " (size: " << calculate_size_of_inode(iter->second) << " bytes | created time: ";
		print_time(read_inode(iter->second).get_created_time());
		cout << " | modified time: ";
		print_time(read_inode(iter->second).get_modified_time());
		cout << ")" << endl;
	}
}

bool FileSystem::go_to_next_dir(string name)
{
	directory current = read_directory(get_current_dir_inode());
	if (current.has_dir(name))
	{
		current_path += name + "/";
		dir_stack.push(current.get_dir()[name]);
		return true;
	}
	else
	{
		cout << "There is no such directory." << endl;
		return false;
	}
}

void FileSystem::go_back_dir()
{
	if (!dir_stack.empty())
	{
		dir_stack.pop();
		int right = current_path.length() - 2;
		while (current_path[right] != '/')
			right--;
		current_path = current_path.substr(0, right + 1);
	}
}

void FileSystem::go_back_to_root()
{
	while (!dir_stack.empty())
	{
		go_back_dir();
	}
}

void FileSystem::show_block_space()
{
	cout << 16384 - num_of_empty_blocks << " blocks are used, " << num_of_empty_blocks << " blocks are unused." << endl;
}

vector<string> FileSystem::spilt_string(string command, char sign)
{

	vector<string> result;
	int left = 0;
	while (left < command.length())
	{
		while (command[left] == sign)
			left++;
		if (left >= command.length())
			break;
		int right = left + 1;
		while (right < command.length() && command[right] != sign)
			right++;
		result.push_back(command.substr(left, right - left));
		left = right;
	}
	return result;
}

bool FileSystem::change_absolute_path(string path)
{
	string record_path = current_path;
	stack<int> record_stack = dir_stack;
	vector<string> v = spilt_string(path, '/');
	go_back_to_root();
	for (string p : v)
	{
		if (!read_directory(get_current_dir_inode()).has_dir(p))
		{
			cout << "There is no such path." << endl;
			current_path = record_path;
			dir_stack = record_stack;
			return false;
		}
		go_to_next_dir(p);
	}
	return true;
}

void FileSystem::add_absolute_dir(string path)
{
	string record_path = current_path;
	stack<int> record_stack = dir_stack;
	vector<string> v = spilt_string(path, '/');
	go_back_to_root();
	for (string p : v)
	{
		if (!read_directory(get_current_dir_inode()).has_dir(p))
			add_dir(p, get_current_dir_inode(), directory());
		go_to_next_dir(p);
	}
	current_path = record_path;
	dir_stack = record_stack;
}

void FileSystem::delete_absolute_dir(string path)
{
	if (spilt_string(path, '/') == spilt_string(current_path, '/'))
	{
		cout << "The current working directory is not allowed to be deleted." << endl;
		return;
	}
	string record_path = current_path;
	stack<int> record_stack = dir_stack;
	vector<string> v = spilt_string(path, '/');
	if (change_absolute_path(path))
	{
		go_back_dir();
		delete_dir(v[v.size() - 1], get_current_dir_inode());
	}
	current_path = record_path;
	dir_stack = record_stack;
}

void FileSystem::add_absolute_file(string path, int size)
{
	string record_path = current_path;
	stack<int> record_stack = dir_stack;
	vector<string> v = spilt_string(path, '/');
	go_back_to_root();
	for (int i = 0; i < v.size() - 1; i++)
	{
		if (!read_directory(get_current_dir_inode()).has_dir(v[i]))
			add_dir(v[i], get_current_dir_inode(), directory());
		go_to_next_dir(v[i]);
	}
	if (!read_directory(get_current_dir_inode()).has_file(v[v.size() - 1]))
		add_file(v[v.size() - 1], get_current_dir_inode(), file(size).get_content());
	else
		cout << "The file exists." << endl;
	current_path = record_path;
	dir_stack = record_stack;
}

void FileSystem::delete_absolute_file(string path)
{
	string record_path = current_path;
	stack<int> record_stack = dir_stack;
	vector<string> v = spilt_string(path, '/');
	go_back_to_root();
	for (int i = 0; i < v.size() - 1; i++)
	{
		if (!read_directory(get_current_dir_inode()).has_dir(v[i]))
		{
			cout << "Thre is no such path." << endl;
			return;
		}
		go_to_next_dir(v[i]);
	}
	if (read_directory(get_current_dir_inode()).has_file(v[v.size() - 1]))
		delete_file(v[v.size() - 1], get_current_dir_inode());
	else
		cout << "There is no such file." << endl;
	current_path = record_path;
	dir_stack = record_stack;
}

void FileSystem::cat_absolute_file(string path)
{
	string record_path = current_path;
	stack<int> record_stack = dir_stack;
	vector<string> v = spilt_string(path, '/');
	go_back_to_root();
	for (int i = 0; i < v.size() - 1; i++)
	{
		if (!read_directory(get_current_dir_inode()).has_dir(v[i]))
		{
			cout << "Thre is no such path." << endl;
			return;
		}
		go_to_next_dir(v[i]);
	}
	if (read_directory(get_current_dir_inode()).has_file(v[v.size() - 1]))
		cout << read_string_from_inode(read_directory(get_current_dir_inode()).get_file()[v[v.size() - 1]]) << endl;
	else
		cout << "There is no such file." << endl;
	current_path = record_path;
	dir_stack = record_stack;
}

void FileSystem::copy_file_to_file(string path1, string path2)
{
	int node1 = -1, node2 = -1;
	string filename = "";
	if (path1[0] == '/')
	{
		string record_path = current_path;
		stack<int> record_stack = dir_stack;
		vector<string> v = spilt_string(path1, '/');
		go_back_to_root();
		for (int i = 0; i < v.size() - 1; i++)
		{
			if (!read_directory(get_current_dir_inode()).has_dir(v[i]))
			{
				cout << "Thre is no such path." << endl;
				return;
			}
			go_to_next_dir(v[i]);
		}
		directory dir = read_directory(get_current_dir_inode());
		if (dir.has_file(v[v.size() - 1]))
			node1 = dir.get_file()[v[v.size() - 1]];
		else
			cout << "The file to be copied does not exist." << endl;
		current_path = record_path;
		dir_stack = record_stack;
		filename = v[v.size() - 1];
	}
	else
	{
		if (!read_directory(get_current_dir_inode()).has_file(path1))
		{
			cout << "The file to be copied does not exist." << endl;
		}
		else
		{
			node1 = read_directory(get_current_dir_inode()).get_file()[path1];
			filename = path1;
		}
	}
	if (node1 < 0)
		return;
	if (path2[0] == '/')
	{
		string record_path = current_path;
		stack<int> record_stack = dir_stack;
		vector<string> v = spilt_string(path2, '/');
		go_back_to_root();
		for (int i = 0; i < v.size() - 1; i++)
		{
			if (!read_directory(get_current_dir_inode()).has_dir(v[i]))
				add_dir(v[i], get_current_dir_inode(), directory());
			go_to_next_dir(v[i]);
		}
		if (read_directory(get_current_dir_inode()).has_dir(v[v.size() - 1]))
		{
		  go_to_next_dir(v[v.size() - 1]);
		  add_file(filename, get_current_dir_inode(), file(1).get_content());
		  node2 = read_directory(get_current_dir_inode()).get_file()[filename];
		  inode newfile = read_inode(node2);
		  newfile.set_created_time(read_inode(node1).get_created_time());
		  save_inode(newfile,node2);
		}
		else {
		  if (!read_directory(get_current_dir_inode()).has_file(v[v.size() - 1]))
		    add_file(v[v.size() - 1], get_current_dir_inode(), file(1).get_content());
		  node2 = read_directory(get_current_dir_inode()).get_file()[v[v.size() - 1]];
		}
		current_path = record_path;
		dir_stack = record_stack;
	}
	else
	{
	  if (read_directory(get_current_dir_inode()).has_dir(path2))
	  {
	    go_to_next_dir(path2);
	    add_file(filename, get_current_dir_inode(), file(1).get_content());		    
	    node2 = read_directory(get_current_dir_inode()).get_file()[filename];
	    inode newfile = read_inode(node2);
	    newfile.set_created_time(read_inode(node1).get_created_time());
	    save_inode(newfile,node2);
	  }
	  else{
	    if(!read_directory(get_current_dir_inode()).has_file(path2))
	    {
	      add_file(path2, get_current_dir_inode(), file(1).get_content());
	    }
	    node2 = read_directory(get_current_dir_inode()).get_file()[path2];
	  } 
	}
	save_string_to_inode(read_string_from_inode(node1), node2);
}

void FileSystem::print_time(time_t t)
{
	tm* ctm = localtime(&t);
	cout << 1900 + ctm->tm_year << "." << ctm->tm_mon << "." << ctm->tm_mday << " " << ctm->tm_hour << ":"
		<< ctm->tm_min << ":" << ctm->tm_sec;
}

