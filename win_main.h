/*
MOTHER OF ALL TO-DOs
- TODO-9(ZET): Address repeated filenames
- TODO-9(ZET): Add argument check for output file destination
*/

#pragma once

#include <windows.h>
#include <io.h>
#include <fstream>
#include <vector>

typedef uint64_t uint64;

struct AssetType
{
	char	name[256];
	char	path[512];
};

struct Asset
{
	uint64		position;
	uint64		size;
	
	char		name[256];
	char		path[512];
	
	AssetType	type;
};

int PathOrFileExists(char* path_, bool isFolder_)
{
	if(path_[0] != '\0')
	{
		if(_access(path_, 0) == 0)
		{
			struct stat status;
			
			stat(path_, &status);
			
			if((isFolder_ && (status.st_mode & S_IFDIR)) || !(status.st_mode & S_IFDIR))
			{
				return true;
			}
			
		}
	}

	return false;
}