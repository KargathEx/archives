#include "File.h"

LytString ReadFile(LytString FileName)
{
	fpos_t pos;
	FILE* f=fopen(FileName.Buffer(),"rb");
	fseek(f,0,SEEK_END);
	fgetpos(f,&pos);
	fseek(f,0,SEEK_SET);
	char* Buffer=new char[(size_t)pos+1];
	fread(Buffer,1,(size_t)pos,f);
	Buffer[pos]='\0';
	LytString Result=Buffer;
	delete[] Buffer;
	return Result;
}

void WriteFile(LytString FileName , LytString Content)
{
	FILE* f=fopen(FileName.Buffer(),"wb");
	fwrite(Content.Buffer(),1,Content.Size(),f);
	fclose(f);
}