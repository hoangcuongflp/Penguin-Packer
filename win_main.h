/*
MOTHER OF ALL TO-DOs
- TODO-9(ZET): Remove spaces in filenames
- TODO-9(ZET): Address repeated filenames
- TODO-9(ZET): Remove last use of std::string
*/

#pragma once

#include <windows.h>
#include <string>
#include <fstream>
#include <vector>
#include "Shlwapi.h"

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