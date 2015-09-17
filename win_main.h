/*
MOTHER OF ALL TO-DOs

- General
	- TODO-9(ZET): Add error messages and argument support
	- TODO-7(ZET): Remove whitespaces from filenames
*/

#pragma once

#include <windows.h>
#include <fstream>
#include <string>
#include <vector>
#include "Shlwapi.h"

typedef uint64_t	uint64;


struct AssetType
{
	std::string		name;
	std::string		path;
};

struct Asset
{
	uint64			position;
	uint64			size;
	
	std::string		name;
	std::string		path;
	
	AssetType		type;
};