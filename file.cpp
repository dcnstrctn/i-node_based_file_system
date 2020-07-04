#include "file.h"

file::file(int size)
{
	set_random_content(size);
}

file::file(string c)
{
	set_content(c);
}

bool file::set_random_content(int size)
{
	if (size > 266 || size < 0)
	{
		return false;
	}
	else
	{
		content.clear();
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < 1024; j++)
			{
				content += char(rand() % 94 + 34);
			}
		}
	}
}

bool file::set_content(string c)
{
	if (c.length() > 266 * 1024)
	{
		return false;
	}
	else
	{
		content = c;
	}
}

string file::get_content()
{
	return content;
}
