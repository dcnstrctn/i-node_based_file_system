#pragma once
#include <cstring>
#include <ctime>
class inode
{
public:
	inode();
	void set_byte_size(int byte_size);
	void set_occupied(bool occupied);
	void set_direct_block_address(int direct_address[]);
	void set_indirect_block_address(int indirect_address);
	void set_created_time(time_t c);
	void set_modified_time(time_t m);
	int get_byte_size();
	bool is_occupied();
	int* get_direct_block_address();
	int get_indirect_block_address();
	time_t get_created_time();
	time_t get_modified_time();

private:
	time_t created;
	time_t modified;
	int byte_size;
	bool occupied = false;
	int direct_block_address[10];
	int indirect_block_address;
};

