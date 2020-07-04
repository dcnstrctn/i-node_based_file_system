#pragma once
#include<string>
#include<iostream>
using namespace std;

class file
{
public:
	file(int size);
	file(string c);
	bool set_random_content(int size);
	bool set_content(string c);
	string get_content();
private:
	string content;
};

