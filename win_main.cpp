#pragma once

#include "win_main.h"

int main(int argc, char* argv[])
{
	uint64					assetFileCount = 0;
	char					assetFileCountString[32];
	
	std::vector<Asset>		assets;
	std::vector<AssetType>	assetTypes;
	char					assetsPath[512];
	char					assetsPackFileName[256];
	char					assetsPackFileExtension[16];
	char					assetsPackFileDestinationPath[512];
	
	uint64					assetInfoFileSize = 0;
	uint64 					lastAssetPosition = 0;

	char					_string[1024];
	HANDLE					_find;
	WIN32_FIND_DATA 		_findData;
	
	char					_type[512];
	char					_name[512];
	uint64					_position = 0;
	uint64					_size = 0;
	
	std::ofstream*			_fileOutputStream = new std::ofstream();
	std::ifstream*			_fileInputStream = new std::ifstream();
	
	
	printf("\nPenguin Packer v1.0 by Jose Carlos Candido (@Zetsaika)\n");
	printf("- Usage: penguinpacker {asset folder location} {outfile name} {outfile extension} {outfile destination}\n\n");
	
	if(argc > 5)
	{
		printf("ERROR: Too many arguments!\n");
		return 0;
	}
	else
	{
		if(argc < 5)
		{
			printf("Not all arguments were detected, using defaults for those missing.\n\n");
		}
		
		if(argv[1] != 0)
		{
			strcpy(assetsPath, argv[1]);
			
			if(argv[2] != 0)
			{
				strcpy(assetsPackFileName, argv[2]);
				
				if(argv[3] != 0)
				{
					strcpy(assetsPackFileExtension, argv[3]);
					
					if(argv[4] != 0)
					{
						strcpy(assetsPackFileDestinationPath, argv[4]);
					}
					else
					{
						strcpy(assetsPackFileDestinationPath, "\0");
					}
				}
				else
				{
					strcpy(assetsPackFileExtension, "pepa");
					strcpy(assetsPackFileDestinationPath, "\0");
				}
			}
			else
			{
				strcpy(assetsPackFileName, "assets");
				strcpy(assetsPackFileExtension, "pepa");
				strcpy(assetsPackFileDestinationPath, "\0");
			}
		}
		else
		{
			strcpy(assetsPath, "assets");
			strcpy(assetsPackFileName, "assets");
			strcpy(assetsPackFileExtension, "pepa");
			strcpy(assetsPackFileDestinationPath, "\0");
		}
		
		for(uint64 i = 0; assetsPackFileName[i] != '\0';)
		{
			if (!isalpha(assetsPackFileName[i]))
			{
				for (uint64 j = i; assetsPackFileName[j] != '\0'; ++j)
				{
					assetsPackFileName[j] = assetsPackFileName[j+1];
				}
			}
			else
			{
				++i;
			}
		}
		
		for(uint64 i = 0; assetsPackFileExtension[i] != '\0';)
		{
			if (!isalpha(assetsPackFileExtension[i]))
			{
				for (uint64 j = i; assetsPackFileExtension[j] != '\0'; ++j)
				{
					assetsPackFileExtension[j] = assetsPackFileExtension[j+1];
				}
			}
			else
			{
				++i;
			}
		}
		printf("{asset folder location} = %s;\n", assetsPath); 
		printf("{outfile name} = %s;\n", assetsPackFileName); 
		printf("{outfile extension} = %s;\n", assetsPackFileExtension); 
		
		
		if(assetsPackFileDestinationPath[0] != '\0')
		{
			printf("{outfile destination} = %s;\n\n", assetsPackFileDestinationPath);
		}
		else
		{
			printf("{outfile destination} = current location;\n\n");
		}
	}
	
	if(!PathOrFileExists(assetsPath, true))
	{
		printf("ERROR: Specified asset folder not found!\n");
		return 0;
	}
	else if(assetsPackFileDestinationPath[0] != '\0' &&
			!PathOrFileExists(assetsPackFileDestinationPath, true))
	{
		printf("ERROR: Specified asset destination folder not found!\n");
		return 0;
	}
	
	SecureZeroMemory(&_findData, sizeof(WIN32_FIND_DATA));
	
	strcpy(_string, assetsPath);
	strcat(_string, "\\*");
	_find = FindFirstFile(_string, &_findData);
	
	printf("Processing folders:\n");
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
			
			printf(" - %s folder processed.\n", _findData.cFileName);
		}
	}
	
	printf("\n");
	
	if(assetTypes.size() <= 0)
	{
		printf("ERROR: No folder inside %s found!\n", assetsPath);
		return 0;
	}
	
	printf("Processing files:\n");
	
	for(uint64 i = 0; i < assetTypes.size(); ++i)
	{
		_find = FindFirstFile(assetTypes[i].path, &_findData);
		
		for(uint64 j = 0; j < sizeof(assetTypes[i].path); ++j)
		{
			if(assetTypes[i].path[j] == '*')
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
				
				for (std::vector<Asset>::iterator it = assets.begin(); it != assets.end(); ++it)
				{
					if(&it->name != &assets[assets.size() - 1].name && 
							strcmp(it->name, assets[assets.size() - 1].name) == 0)
					{
						printf("\nERROR: Filename %s repeated\n", it->name);
						return 0;
					}
				}
				
				assets[assets.size() - 1].type = assetTypes[i];
													
				strcpy(assets[assets.size() - 1].path, assets[assets.size() - 1].type.path);				
				strcat(assets[assets.size() - 1].path, assets[assets.size() - 1].name);
													
				lastAssetPosition += assets[assets.size() - 1].size + 1;
				
				printf(" - File %s processed.\n", assets[assets.size() - 1].name);
			}
		}
	}
	
	printf("\nSearching repeated files:\n");
	
	for(;;)
	{
		sprintf(assetFileCountString, "%I64u", assetFileCount);
		
		if(assetsPackFileDestinationPath[0] != '\0')
		{
			strcpy(_string, assetsPackFileDestinationPath);
			strcat(_string, "\\");
			strcat(_string, assetsPackFileName);
		}
		else
		{
			strcpy(_string, assetsPackFileName);
		}
										
		strcat(_string, assetFileCountString);
		strcat(_string, ".");
		strcat(_string, assetsPackFileExtension);
		
		if(PathOrFileExists(_string, false))
		{
			++assetFileCount;
			
			_fileInputStream->open(_string, std::ios::binary);
			
			if(_fileInputStream->good())
			{
				while(_fileInputStream->getline(_string, 1024))
				{
					if(_string[0] == '\r')
					{
						break;
					}
					
					sscanf(_string, "%s | %s | %I64u | %I64u", &_type, &_name, &_position, &_size);
					
					for (std::vector<Asset>::iterator it = assets.begin(); it != assets.end();)
					{
						if(strcmp(it->name, _name) == 0)
						{
							it = assets.erase(it);
							
							strcpy(_string, assetsPackFileName);								
							strcat(_string, assetFileCountString);
							strcat(_string, ".");
							strcat(_string, assetsPackFileExtension);
							printf(" - %s already exists in %s.\n", _name, _string);
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
		printf("\nNo new files found!\n\nDone!\n");
		return 0;
	}
	
	printf(" - No repeated files found.\n\nCreating raw asset pack file.\n");
	
	strcpy(_string, assetsPackFileName);
	strcat(_string, assetFileCountString);
	strcat(_string, ".1");
	_fileOutputStream->open(_string, std::ios::binary);
	
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
	
	printf("Creating asset pack info file.\n");
	
	strcpy(_string, assetsPackFileName);
	strcat(_string, assetFileCountString);
	strcat(_string, ".2");
	_fileOutputStream->open(_string, std::ios::binary);
	
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
	
	printf("Creating asset pack file.\n");
	
	if(assetsPackFileDestinationPath[0] != '\0')
	{
		strcpy(_string, assetsPackFileDestinationPath);
		strcat(_string, "\\");
		strcat(_string, assetsPackFileName);
	}
	else
	{
		strcpy(_string, assetsPackFileName);
	}
	
	strcat(_string, assetFileCountString);
	strcat(_string, ".");
	strcat(_string, assetsPackFileExtension);
	_fileOutputStream->open(_string, std::ios::binary);
	
	if(_fileOutputStream->good())
	{
		strcpy(_string, assetsPackFileName);
		strcat(_string, assetFileCountString);
		strcat(_string, ".2");
		_fileInputStream->open(_string, std::ios::binary);
			
		if(_fileInputStream->good())
		{
			*_fileOutputStream << _fileInputStream->rdbuf();
			
			_fileInputStream->close();
		}
		
		strcpy(_string, assetsPackFileName);
		strcat(_string, assetFileCountString);
		strcat(_string, ".1");
		_fileInputStream->open(_string, std::ios::binary);
			
		if(_fileInputStream->good())
		{
			*_fileOutputStream << _fileInputStream->rdbuf();
			
			_fileInputStream->close();
		}
		
		*_fileOutputStream << "\n";
		*_fileOutputStream << assetInfoFileSize;
	}
	
	_fileOutputStream->close();
	
	strcpy(_string, assetsPackFileName);
	strcat(_string, assetFileCountString);
	strcat(_string, ".1");
	
	DeleteFile(_string);
	
	strcpy(_string, assetsPackFileName);
	strcat(_string, assetFileCountString);
	strcat(_string, ".2");
	
	DeleteFile(_string);
	
	printf("Done!\n");
	
    return 0;
}