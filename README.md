# Penguin Packer v1.0
A simple C++ asset packer.

## How to use
Compile using your preferred method.

>Your asset folder structure must be:
>
>* Asset Folder/
>  * subfolder1/
>    * 1.png
>    * blob.png
>  * subfolder2/
>    * 1984.txt
>  * subfolder3/
>    * bestcookierecipes.etc
>
>**You must have at least one folder inside your asset folder!**

>**Filenames must be unique!**

Penguin Packer takes up to 4 arguments, if any is missing the defaults for those will be used:

1. *{asset folder location}* = "Asset Folder/" In above list without trailing / or \. Default = assets.

2. *{outfile name}* = Name of the output pack file in alphanumeric. Default = assets.

3. *{outfile extension}* = Extension of the output pack file without the dot and in alphanumeric. Default = pepa.

4. *{outfile destination}* = Destination of the output pack file. Default = executable folder.

Example usage: `penguinpacker "Y:\Game\assets" assets pack  "Y:\Game\build"`

This will read folders inside "Y:\Game\assets" to make an asset pack file name assets.pack at "Y:\Game\build".

If a asset pack file already exists at destination, the program will make a new file using only files that are not in it.

## Reading the file
The resulting file is formated in a simple way. The first lines give information about every asset and the rest of the file is their data. Example info lines:

```
type(subfolder the file was in) | file name with extension | file first byte in the file | file size
image | v42.png | 502520 | 502519
text | a.txt | 1005040 | 9
```

A blank line is inserted after all info lines so the reading program can know when to stop.

## License
Penguin Packer is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License, see LICENSE file for more.