#ifndef IMAGELODADER_H
#define IMAGELODADER_H

//#pragma once
/*
#include "../stdafx.h"

#define BMP 11000
#define TGA 11001

typedef struct
{
	WCHAR lpstrFile[MAX_PATH];
	D3DX11_IMAGE_LOAD_INFO loadInfo;
} SAVING_TEXTURE_INFO;

typedef struct {
	char  idlength;				//1
	char  colourmaptype;		//1
	char  datatypecode;			//1
	
	short int colourmaporigin;	//2
	short int colourmaplength;	//2
	
	char  colourmapdepth;		//1
	
	short int x_origin;			//2
	short int y_origin;			//2
	
	short width;			//2
	short height;			//2
	
	char  bitsperpixel;			//1
	char  imagedescriptor;		//1

	WCHAR path[MAX_PATH];

} TGA_HEADER;

class ImageLoader
{
public:
	ImageLoader()
	{

	}

	TGA_HEADER ReadImage_TGA(LPCWSTR path)
	{
		TGA_HEADER header;		
		DWORD  dwBytesRead = 0;
		char   ReadBuffer[4096] = { 0 };		
		HANDLE hFile;

		hFile = CreateFile(path, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);

		
			ReadFile(hFile, &header.idlength, 1, &dwBytesRead, NULL);
			ReadFile(hFile, &header.colourmaptype, 1, &dwBytesRead, NULL);
			ReadFile(hFile, &header.datatypecode, 1, &dwBytesRead, NULL);
			ReadFile(hFile, &header.colourmaporigin, 2, &dwBytesRead, NULL);
			ReadFile(hFile, &header.colourmaplength, 2, &dwBytesRead, NULL);
			ReadFile(hFile, &header.colourmapdepth, 1, &dwBytesRead, NULL);
			ReadFile(hFile, &header.x_origin, 2, &dwBytesRead, NULL);
			ReadFile(hFile, &header.y_origin, 2, &dwBytesRead, NULL);
			ReadFile(hFile, &header.width, 2, &dwBytesRead, NULL);
			ReadFile(hFile, &header.height, 2, &dwBytesRead, NULL);
			ReadFile(hFile, &header.bitsperpixel, 1, &dwBytesRead, NULL);
			ReadFile(hFile, &header.imagedescriptor, 1, &dwBytesRead, NULL);


			wcscpy_s(header.path, path);

		//ReadFileEx(hFile, &header, sizeof(header), &ol, NULL);

		CloseHandle(hFile);

		

		return header;
		
	}
protected:
private:



};
*/
#endif