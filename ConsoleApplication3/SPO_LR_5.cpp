#include <iostream>
#include <windows.h>
#include <tlhelp32.h>

void OneExemple();

void TwoExemple();

void Print(int mem);

MEMORY_BASIC_INFORMATION mbi;
DWORD pb;
int count;
int memory;

int main()
{
	setlocale(LC_CTYPE, "rus");
	OneExemple();
	TwoExemple();
}

void OneExemple() {
	MEMORYSTATUSEX MemInfo;
	MemInfo.dwLength = sizeof(MemInfo);
	GlobalMemoryStatusEx(&MemInfo);
	printf_s(" Размер страничного файла %I64d Гбайт\n", MemInfo.ullTotalPageFile / 1073741824);
	count = 0;
	memory = 0;
	pb = 0;
	Print(0);
}

void TwoExemple() {
	int size;
	std::cout << " Введите размер региона (байт): ";
	std::cin >> size;
	pb = 0;
	LPVOID pMEM = NULL;
	Print(1);
	pb = 0;
	while (VirtualQuery(LPCVOID(pb), &mbi, sizeof(MEMORY_BASIC_INFORMATION)) == sizeof(mbi))
	{
		if (mbi.RegionSize == size && mbi.State == MEM_FREE)
		{
			printf_s(" Адрес выделенного региона: %08x\n", mbi.BaseAddress);
			VirtualAlloc(NULL, mbi.RegionSize, MEM_RESERVE, PAGE_READWRITE);
			Sleep(10000);
			pMEM = VirtualAlloc(NULL, mbi.RegionSize, MEM_COMMIT, PAGE_READWRITE);
			break;
		}
		pb = pb + mbi.RegionSize;
	}

	Print(2);
	if (VirtualFree(pMEM, 0, MEM_RELEASE))
	{
		printf_s(" Возврат физической памяти произошел успешно!\n");
	}
}

void Print(int mem) {
	count = 0;
	memory = 0;
	pb = 0;
	bool flag = false;
	printf_s(" %8s %20s(байты)\t%16s\n", "Адрес", "Размер региона", "Атрибут защиты");
	while (VirtualQuery(LPCVOID(pb), &mbi, sizeof(MEMORY_BASIC_INFORMATION)) == sizeof(mbi))
	{
		if (mem == 1 && mbi.State == MEM_RESERVE)
		{
			printf_s(" %08x %16d %16d\n", mbi.BaseAddress, mbi.RegionSize, mbi.State);

			count++;
			memory += mbi.RegionSize;
		}
		else if (mem == 0 && mbi.State == MEM_FREE)
		{
			if (pb > 0 && pb < pow(1024, 3))
				printf_s(" %08x %16d %16d\n", mbi.BaseAddress, mbi.RegionSize, mbi.State);

			count++;
			memory += mbi.RegionSize;
		}
		else if (mem == 2 && mbi.State == MEM_RESERVE) 
		{
			count++;
			memory += mbi.RegionSize;
			flag = true;
		}
		pb = pb + mbi.RegionSize;
	}

	switch (mem)
	{
	case 0:
		printf_s(" Количество свободных регионов выполняющегося процесса: % 8d\n", count);
		printf_s(" Суммарный объем всех свободных регионов в адресном пространстве процесса: % 8d Кбайт\n", memory / 1024);
		break;
	case 1:
		printf_s(" Количество зарезервированных регионов выполняющегося процесса: % 8d\n", count);
		printf_s(" Суммарный объем всех зарезервированных регионов в адресном пространстве процесса: % 8d Кбайт\n", memory / 1024);
		break;
	case 2:
		printf_s(" После резервирования\n Суммарный объем всех зарезервированных регионов в адресном пространстве процесса: % 8d Кбайт\n Количество зарезервированных регионов выполняющегося процесса: % 8d\n", memory / 1024, count);
		break;
	default: break;
	}
}
