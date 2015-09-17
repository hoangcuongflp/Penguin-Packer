# Penguin Packer v0.1
A simple C++ asset packer.

## How to use
Compile and put the resulting executable file alongside a folder called */assets/*.

>Your */assets/* folder structure must be:
>
>* /assets/
>  * /images/
>    * 1.png
>    * blob.png
>  * /texts/
>    * 1984.txt
>  * /etc/
>    * something.etc
>
>**Filenames must be unique and alphanumeric!**
	
>The only required folder name is assets, all the other ones are there just for classification purposes. You must have at least one folder inside */assets/*.

Run the executable and an *assetsX.pepa* file will be generated. If an asset file already exists, the program will create another one only with assets that are not present there.

## License
The MIT License (MIT), see LICENSE file for more.