#include <iostream>
#include <vector>
#include <windows.h>

using namespace std;
#pragma pack(push,1)
typedef struct
{
WORD header_crc;
unsigned char header_type;
WORD header_flag;
WORD header_size;
} RarHeaderStruct;

typedef struct
{
DWORD packSize;
DWORD unpSize;
unsigned char hostOS;
DWORD fileCRC;
DWORD fileTime;
unsigned char unpVer;
unsigned char method;
WORD nameSize;
DWORD fileAttr;
}FILE_HEAD;

#pragma pack (pop)

// функция для вывода оглавления архива
void PrintArchiveContents( vector<unsigned char>& dataBuffer)
{
    size_t i = 0;
    while (i < dataBuffer.size())
        {
        RarHeaderStruct* rarHeader = reinterpret_cast<RarHeaderStruct*>(&dataBuffer[i]);
        if (rarHeader->header_type == 0x74)
            {
            FILE_HEAD* fileHeader = reinterpret_cast<FILE_HEAD*>(&dataBuffer[i + sizeof(RarHeaderStruct)]);
            // название файла
            string fileName(dataBuffer.begin() + i + sizeof(RarHeaderStruct) + sizeof(FILE_HEAD),
                            dataBuffer.begin() + i + sizeof(RarHeaderStruct) + sizeof(FILE_HEAD) + fileHeader->nameSize);
            // выводим название файла
            cout << "Имя файла: " << fileName << endl;
            // переносим указатель на следующий заголовок
            i += sizeof(RarHeaderStruct) + sizeof(FILE_HEAD) + fileHeader->nameSize;
            }
        else
        {

            ++i;
        }
    }
}


int main()
{
    setlocale(LC_ALL, "Russian");

    wchar_t* fileName = L"12.rar";
    HANDLE fileHandle = CreateFileW(
        fileName,
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (fileHandle == INVALID_HANDLE_VALUE)
        {
        wcout << "Ошибка открытия!!!" << endl;
        return 1;
        }

    LARGE_INTEGER fileOffSet;
    fileOffSet.QuadPart = 0;

    DWORD currentPosition = SetFilePointer(
        fileHandle,
        fileOffSet.LowPart,
        &fileOffSet.HighPart,
        FILE_BEGIN
    );

    if (currentPosition == fileOffSet.LowPart && GetLastError() == NO_ERROR) {
        DWORD fileSize = GetFileSize(fileHandle, NULL);

        std::vector<unsigned char> dataBuffer(fileSize);

        DWORD bytesRead;

        bool readOk = ReadFile(
            fileHandle,
            dataBuffer.data(),
            fileSize,
            &bytesRead,
            NULL
        );

        if (readOk) {
            wcout << "Файл успешно прочитан!!!" << endl;
        }


        wcout << "Название архива: " << fileName << endl;


        PrintArchiveContents(dataBuffer);


    }

    CloseHandle(fileHandle);
    return 0;
}
