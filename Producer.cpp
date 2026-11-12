#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

#define ARRAY_SIZE 15
#define SHM_NAME L"Local\\MySharedMemory"
#define EVENT_NAME L"Local\\DataReadyEvent"

int main() {
    std::cout << "Producer started.\n";

    HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, EVENT_NAME);
    if (!hEvent) return 1;

    HANDLE hMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, ARRAY_SIZE * sizeof(int), SHM_NAME);
    if (!hMap) {
        CloseHandle(hEvent);
        return 1;
    }

    int* pData = (int*)MapViewOfFile(hMap, FILE_MAP_WRITE, 0, 0, ARRAY_SIZE * sizeof(int));
    if (!pData) {
        CloseHandle(hMap);
        CloseHandle(hEvent);
        return 1;
    }

    srand(time(NULL));
    std::cout << "Generated array: ";
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        pData[i] = (rand() % 21) - 10;
        std::cout << pData[i] << " ";
    }
    std::cout << "\n";

    DeleteFile(L"..\\results.txt");

    SetEvent(hEvent);

    std::cout << "Data ready. Press ENTER to exit Producer.\n";
    std::cin.get();

    UnmapViewOfFile(pData);
    CloseHandle(hMap);
    CloseHandle(hEvent);
    return 0;
}