#pragma once

#include "win_main.h"

int CALLBACK wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
	uint64					assetFileCount = 0;
	
	std::vector<Asset>		assets;
	std::vector<AssetType>	assetTypes;
	
	std::string				_string;
	std::string				_line;
	HANDLE					_find;
	WIN32_FIND_DATA 		_findData;
	
	std::ofstream			_fileOutputStream;
	std::ifstream			_fileInputStream;
	
	uint64					assetInfoFileSize = 0;
	uint64 					lastAssetPosition = 0;
	
	
	SecureZeroMemory(&_findData, sizeof(WIN32_FIND_DATA));
	
	_find = FindFirstFile("assets\\*", &_findData);
	
	if(_find == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	
	while(FindNextFile(_find, &_findData) != 0)
	{
		if(_findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			assetTypes.insert(assetTypes.end(), {""});
			
			_string = _findData.cFileName;
			_string.erase(std::remove(_string.begin(), _string.end(), ' '), _string.end());
			assetTypes[assetTypes.size() - 1].name = _string;
			
			_string = "assets\\";
			_string += _findData.cFileName;
			_string += "\\*";
			assetTypes[assetTypes.size() - 1].path = _string;
		}
	}
	
	assetTypes.erase(assetTypes.begin());
	
	if(assetTypes.size() <= 0)
	{
		return 0;
	}
	
	for(uint64 i = 0; i < assetTypes.size(); ++i)
	{
		_find = FindFirstFile(assetTypes[i].path.c_str(), &_findData);
		
		assetTypes[i].path.pop_back();
	
		if(_find == INVALID_HANDLE_VALUE)
		{
			continue;
		}
		
		while(FindNextFile(_find, &_findData) != 0)
		{
			if(!(_findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				assets.insert(assets.end(), {0});
				
				assets[assets.size() - 1].size = (_findData.nFileSizeHigh * ((uint64)MAXDWORD + 1)) +
													_findData.nFileSizeLow;
													
				assets[assets.size() - 1].position = lastAssetPosition;
				
				_string = _findData.cFileName;
				_string = _string.substr(0, _string.find("."));
				_string.erase(std::remove(_string.begin(), _string.end(), ' '), _string.end());
				assets[assets.size() - 1].name = _string;
				
				assets[assets.size() - 1].type = assetTypes[i];
				
				assets[assets.size() - 1].path = assets[assets.size() - 1].type.path +
													_findData.cFileName;
													
				lastAssetPosition += assets[assets.size() - 1].size + 1;
			}
		}
	}
	
	for(;;)
	{
		_string = "assets" + std::to_string(assetFileCount) + ".pepa";
		
		if(PathFileExists(_string.c_str()))
		{
			++assetFileCount;
			
			_fileInputStream.open(_string, std::ios::binary);
			
			if(_fileInputStream.is_open())
			{
				while(std::getline(_fileInputStream, _line))
				{
					if(_line == "")
					{
						break;
					}
					
					char _type[256] = {};
					char _name[256] = {};
					uint64 _position = 0;
					uint64 _size = 0;
					
					sscanf(_line.c_str(), "%s | %s | %I64u | %I64u", &_type, &_name, &_position, &_size);
					
					for (std::vector<Asset>::iterator it = assets.begin(); it != assets.end();)
					{
						if(strcmp(it->name.c_str(), _name) == 0)
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
			
			_fileInputStream.close();
		}
		else{
			break;
		}
		
	}
	
	if(assets.size() == 0)
	{
		return 0;
	}
	
	_fileOutputStream.open("assets" + std::to_string(assetFileCount) + ".pepra", std::ios::binary);
	
	if(_fileOutputStream.is_open())
	{
		for(uint64 i = 0; i < assets.size(); ++i)
		{
			_fileInputStream.open(assets[i].path, std::ios::binary);
			
			if (_fileInputStream.is_open())
			{
				_fileOutputStream << _fileInputStream.rdbuf();
				
				_fileInputStream.close();
			}
		}
		
		_fileOutputStream.close();
	}
	
	
	_fileOutputStream.open("assets" + std::to_string(assetFileCount) + ".pepi", std::ios::binary);
	
	if(_fileOutputStream.is_open())
	{
		for(uint64 i = 0; i < assets.size(); ++i)
		{
			_fileOutputStream << assets[i].type.name;
			_fileOutputStream << " | ";
			_fileOutputStream << assets[i].name;
			_fileOutputStream << " | ";
			_fileOutputStream << assets[i].position;
			_fileOutputStream << " | ";
			_fileOutputStream << assets[i].size;
			_fileOutputStream << "\n";
		}
		
		_fileOutputStream << "\n";
		
		assetInfoFileSize = (uint64)_fileOutputStream.tellp();
	
		_fileOutputStream.close();
	}
	
	_fileOutputStream.open("assets" + std::to_string(assetFileCount) + ".pepa", std::ios::binary);
	
	if(_fileOutputStream.is_open())
	{
		_fileInputStream.open("assets" + std::to_string(assetFileCount) + ".pepi", std::ios::binary);
			
		if(_fileInputStream.is_open())
		{
			_fileOutputStream << _fileInputStream.rdbuf();
			
			_fileInputStream.close();
		}
		
		_fileInputStream.open("assets" + std::to_string(assetFileCount) + ".pepra", std::ios::binary);
			
		if(_fileInputStream.is_open())
		{
			_fileOutputStream << _fileInputStream.rdbuf();
			
			_fileInputStream.close();
		}
		
		_fileOutputStream << "\n";
		_fileOutputStream << assetInfoFileSize;
		
		_fileOutputStream.close();
	}
	
	_string = "assets" + std::to_string(assetFileCount) + ".pepra";
	DeleteFile(_string.c_str());
	_string = "assets" + std::to_string(assetFileCount) + ".pepi";
	DeleteFile(_string.c_str());
	
    return 0;
}