#include "inode.h"

inode::inode()
{
	byte_size = 0;
	occupied = false;
	memset(direct_block_address, -1, sizeof(direct_block_address));
	indirect_block_address = -1;
	created = time(0);
	modified = created;
}

void inode::set_byte_size(int byte_size)
{
	this->byte_size = byte_size;
}

void inode::set_occupied(bool occupied)
{
	this->occupied = occupied;
}

void inode::set_direct_block_address(int direct_address[])
{
	for (int i = 0; i < 10; i++)
	{
		direct_block_address[i] = direct_address[i];
	}
}

void inode::set_indirect_block_address(int indirect_address)
{
	indirect_block_address = indirect_address;
}

void inode::set_created_time(time_t c)
{
	created = c;
}

void inode::set_modified_time(time_t m)
{
	modified = m;
}

int inode::get_byte_size()
{
	return byte_size;
}

bool inode::is_occupied()
{
	return occupied;
}

int* inode::get_direct_block_address()
{
	return direct_block_address;
}

int inode::get_indirect_block_address()
{
	return indirect_block_address;
}

time_t inode::get_created_time()
{
	return created;
}

time_t inode::get_modified_time()
{
	return modified;
}
