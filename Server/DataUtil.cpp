#include "DataUtil.h"

void CDataUtil::GetBytes(char* Buffer, void* Data, size_t Data_size)
{
	memcpy(Data, Buffer, Data_size);
}

int CDataUtil::GetHeader(char* MessageBuffer)
{
	char buff[4] = {};
	int header = 0;
	int t = 0;
	GetBytes(MessageBuffer, buff, sizeof(buff));
	t = buff[0];
	buff[0] = buff[3];
	buff[3] = t;
	t = buff[1];
	buff[1] = buff[2];
	buff[2] = t;
	GetBytes(buff, &header, sizeof(header));
	return header;
}


int CDataUtil::SetHeader(int iflags)
{
	int header = 0;
	char arr[4] = { iflags & 0xFF , (iflags >> 8) & 0xFF, 0x3D, 0xD9 };
	memcpy(&header, arr, sizeof(arr));
	return header;
}

int CDataUtil::ContactHeader(char* buffer, int clientID, int iflags)
{
	memset(buffer, 0, sizeof(buffer));
	int header = SetHeader(iflags);
	memcpy(buffer, &header, sizeof(header));
	memcpy(buffer + sizeof(header), &clientID, sizeof(clientID));
	return 0;
}

void CDataUtil::DumpPacket(char* buffer, int size)
{
	int* arr = new int[size];
	for (int i = 0; i < size; i++)
	{
		CDataUtil::GetBytes(buffer + i * 4, arr + i, sizeof(int));
		printf("Arr[%d]: %d, ", i, arr[i]);
	}
	printf("\n");

	delete[] arr;
}
