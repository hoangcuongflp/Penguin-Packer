#pragma once

#include "win_main.h"

int main(int argc, char* argv[])
{
	uint64					assetFileCount = 0;
	
	std::vector<Asset>		assets;
	std::vector<AssetType>	assetTypes;
	char					assetsPath[512];
	char					assetsPackFileName[256];
	char					assetsPackFileExtension[16];
	
	uint64					assetInfoFileSize = 0;
	uint64 					lastAssetPosition = 0;
	
	std::string*			_line = new std::string();
	char					_string[1024];
	char					_assetFileCount[32];
	HANDLE					_find;
	WIN32_FIND_DATA 		_findData;
	
	char					_type[512];
	char					_name[512];
	uint64					_position = 0;
	uint64					_size = 0;
	
	std::ofstream*			_fileOutputStream = new std::ofstream();
	std::ifstream*			_fileInputStream = new std::ifstream();
	
	
	printf("Penguin Packer v0.2 by Jose Carlos Candido (@Zetsaika)\n");
	printf("- Usage: penguinpacker {asset folder location} {outfile name} {outfile extension}\n\n");
	
	if(argc < 4)
	{
		printf("Not all arguments were detected, using defaults.\n\n");
		
		strcpy(assetsPath, "assets");
		strcpy(assetsPackFileName, "assets");
		strcpy(assetsPackFileExtension, "pepa");
	}
	else
	{
		strcpy(assetsPath, argv[1]);
		strcpy(assetsPackFileName, argv[2]);
		strcpy(assetsPackFileExtension, argv[3]);
	}
	
	
	if(!PathFileExists(assetsPath))
	{
		printf("ERROR: Specified asset folder found!\n");
		return 0;
	}
	
	SecureZeroMemory(&_findData, sizeof(WIN32_FIND_DATA));
	
	strcpy(_string, assetsPath);
	strcat(_string, "\\*");
	_find = FindFirstFile(_string, &_findData);
	
	while(FindNextFile(_find, &_findData) != 0)
	{
		if(_findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(strcmp(_findData.cFileName, "..") == 0)
			{
				continue;
			}
			
			assetTypes.push_back({0});
			
			strcpy(assetTypes[assetTypes.size() - 1].name, _findData.cFileName);
			
			strcpy(assetTypes[assetTypes.size() - 1].path , assetsPath);
			strcat(assetTypes[assetTypes.size() - 1].path, "\\");
			strcat(assetTypes[assetTypes.size() - 1].path, _findData.cFileName);
			strcat(assetTypes[assetTypes.size() - 1].path, "\\*");
		}
	}
	
	if(assetTypes.size() <= 0)
	{
		printf("ERROR: No folder inside %s found!\n", assetsPath);
		return 0;
	}
	
	for(uint64 i = 0; i < assetTypes.size(); ++i)
	{
		_find = FindFirstFile(assetTypes[i].path, &_findData);
		
		for(uint64 j = 0; j < sizeof(assetTypes[i].path); ++j)
		{
			if(memcmp(&assetTypes[i].path[j], "*", 1) == 0)
			{
				memcpy(&assetTypes[i].path[j], "\0", 1);
				break;
			}
		}
	
		if(_find == INVALID_HANDLE_VALUE)
		{
			continue;
		}
		
		while(FindNextFile(_find, &_findData) != 0)
		{
			if(!(_findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				assets.push_back({0});
				
				assets[assets.size() - 1].size = (_findData.nFileSizeHigh * ((uint64)MAXDWORD + 1)) +
													_findData.nFileSizeLow;
													
				assets[assets.size() - 1].position = lastAssetPosition;
				
				strcpy(assets[assets.size() - 1].name, _findData.cFileName);
				
				for(uint64 j = 0; j < sizeof(assets[assets.size() - 1].name); ++j)
				{
					if(memcmp(&assets[assets.size() - 1].name[j], ".", 1) == 0)
					{
						memcpy(&assets[assets.size() - 1].name[j], "\0", 1);
						break;
					}
				}
				
				assets[assets.size() - 1].type = assetTypes[i];
													
				strcpy(assets[assets.size() - 1].path, assets[assets.size() - 1].type.path);				
				strcat(assets[assets.size() - 1].path, _findData.cFileName);
													
				lastAssetPosition += assets[assets.size() - 1].size + 1;
			}
		}
	}
	
	for(;;)
	{
		sprintf(_assetFileCount, "%I64u", assetFileCount);
		
		strcpy(_string, assetsPackFileName);								
		strcat(_string, _assetFileCount);
		strcat(_string, ".");
		strcat(_string, assetsPackFileExtension);
		
		if(PathFileExists(_string))
		{
			++assetFileCount;
			
			_fileInputStream->open(_string, std::ios::binary);
			
			if(_fileInputStream->good())
			{
				while(std::getline(*_fileInputStream, *_line))
				{
					if(*_line == "")
					{
						break;
					}
					
					sscanf(_line->c_str(), "%s | %s | %I64u | %I64u", &_type, &_name, &_position, &_size);
					
					for (std::vector<Asset>::iterator it = assets.begin(); it != assets.end();)
					{
						if(strcmp(it->name, _name) == 0)
						{
							it = assets.erase(it);
						}
						else
						{
							++it;
						}
					}
				}
			}
			
			_fileInputStream->close();
		}
		else{
			break;
		}
		
	}
	
	if(assets.size() == 0)
	{
		printf("ERROR: No new files found!\n");
		return 0;
	}
	
	_fileOutputStream->open(assetsPackFileName + std::to_string(assetFileCount) + ".1", std::ios::binary);
	
	if(_fileOutputStream->good())
	{
		for(uint64 i = 0; i < assets.size(); ++i)
		{
			_fileInputStream->open(assets[i].path, std::ios::binary);
			
			if (_fileInputStream->good())
			{
				*_fileOutputStream << _fileInputStream->rdbuf();
			}
			
			_fileInputStream->close();
		}
	}
	
	_fileOutputStream->close();
	
	
	_fileOutputStream->open(assetsPackFileName + std::to_string(assetFileCount) + ".2", std::ios::binary);
	
	if(_fileOutputStream->good())
	{
		for(uint64 i = 0; i < assets.size(); ++i)
		{
			*_fileOutputStream << assets[i].type.name;
			*_fileOutputStream << " | ";
			*_fileOutputStream << assets[i].name;
			*_fileOutputStream << " | ";
			*_fileOutputStream << assets[i].position;
			*_fileOutputStream << " | ";
			*_fileOutputStream << assets[i].size;
			*_fileOutputStream << "\n";
		}
		
		*_fileOutputStream << "\n";
		
		assetInfoFileSize = (uint64)_fileOutputStream->tellp();
	}
	
	_fileOutputStream->close();
	
	_fileOutputStream->open(assetsPackFileName + std::to_string(assetFileCount) +
								"." + assetsPackFileExtension);
	
	if(_fileOutputStream->good())
	{
		_fileInputStream->open(assetsPackFileName + std::to_string(assetFileCount) + ".2", std::ios::binary);
			
		if(_fileInputStream->good())
		{
			*_fileOutputStream << _fileInputStream->rdbuf();
			
			_fileInputStream->close();
		}
		
		_fileInputStream->open(assetsPackFileName + std::to_string(assetFileCount) + ".1", std::ios::binary);
			
		if(_fileInputStream->good())
		{
			*_fileOutputStream << _fileInputStream->rdbuf();
			
			_fileInputStream->close();
		}
		
		*_fileOutputStream << "\n";
		*_fileOutputStream << assetInfoFileSize;
	}
	
	_fileOutputStream->close();
	
	sprintf(_assetFileCount, "%I64u", assetFileCount);
	strcpy(_string, assetsPackFileName);
	strcat(_string, _assetFileCount);
	strcat(_string, ".1");
	
	DeleteFile(_string);
	
	sprintf(_assetFileCount, "%I64u", assetFileCount);
	strcpy(_string, assetsPackFileName);
	strcat(_string, _assetFileCount);
	strcat(_string, ".2");
	
	DeleteFile(_string);
	
	
    return 0;
}