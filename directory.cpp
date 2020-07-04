#include "directory.h"
#include <iostream>

bool directory::add_dir(string name, int address)
{
	if (has_dir(name))
	{	
		cout << "The directory exists." << endl;
		return false;
	}
	else
	{
		dir[name] = address;
	}
}

bool directory::add_file(string name, int address)
{
	if (has_file(name))
	{
		cout << "The file exists." << endl;
		return false;
	}
	else
	{
		file[name] = address;
	}
}

int directory::delete_dir(string name)
{
	if (!has_dir(name)) return -1;
	else
	{
		int address = dir[name];
		dir.erase(name);
		return address;
	}
}

int directory::delete_file(string name)
{
	if (!has_file(name)) return -1;
	else
	{
		int address = file[name];
		file.erase(name);
		return address;
	}
}

bool directory::has_dir(string name)
{
	return dir.count(name);
}

bool directory::has_file(string name)
{
	return file.count(name);
}

void directory::list_all_dir()
{
	map<string, int>::iterator iter;
	cout << "Directory: ";
	for (iter = dir.begin(); iter != dir.end(); iter++)
	{
		cout << iter->first << " ";
	}
	cout << endl;
}

void directory::list_all_file()
{
	map<string, int>::iterator iter;
	cout << "File: ";
	for (iter = file.begin(); iter != file.end(); iter++)
	{
		cout << iter->first << " ";
	}
	cout << endl;
}

void directory::list_all()
{
	list_all_dir();
	list_all_file();
}

map<string, int>& directory::get_dir()
{
	return dir;
}

map<string, int>& directory::get_file()
{
	return file;
}
