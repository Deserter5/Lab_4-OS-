#include <windows.h>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>

#define ARRAY_SIZE 15
#define SHM_NAME L"Local\\MySharedMemory"
#define EVENT_NAME L"Local\\DataReadyEvent"
#define MUTEX_NAME L"Local\\FileMutex"

int main() {
    std::cout << "Consumer 2 started.\n";

    HANDLE hEvent = OpenEvent(SYNCHRONIZE, FALSE, EVENT_NAME);
    while (!hEvent) {
        Sleep(200);
        hEvent = OpenEvent(SYNCHRONIZE, FALSE, EVENT_NAME);
    }

    WaitForSingleObject(hEvent, INFINITE);
    CloseHandle(hEvent);

    HANDLE hMap = OpenFileMapping(FILE_MAP_READ, FALSE, SHM_NAME);
    if (!hMap) return 1;

    int* pData = (int*)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, ARRAY_SIZE * sizeof(int));
    if (!pData) {
        CloseHandle(hMap);
        return 1;
    }

    std::vector<int> initialData(pData, pData + ARRAY_SIZE);
    UnmapViewOfFile(pData);
    CloseHandle(hMap);

    std::map<int, int> freqMap;
    for (int x : initialData) {
        if (x >= 0) {
            freqMap[x]++;
        }
    }

    HANDLE hMutex = CreateMutex(NULL, FALSE, MUTEX_NAME);
    WaitForSingleObject(hMutex, INFINITE);

    std::ofstream out("..\\results.txt", std::ios::app);
    if (out.is_open()) {
        out << "Consumer 2 Results\n";
        out << "Non-negative elements frequency:\n";
        for (const auto& pair : freqMap) {
            out << "Value: " << pair.first << " | Count: " << pair.second << "\n";
        }
        out << "\n";
        out.close();
        std::cout << "Consumer 2 wrote to results.txt\n";
    }

    ReleaseMutex(hMutex);
    CloseHandle(hMutex);

    std::cout << "Press ENTER to exit.\n";
    std::cin.get();
    return 0;
}