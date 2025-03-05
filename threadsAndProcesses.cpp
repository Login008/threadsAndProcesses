#include <windows.h>
#include <iostream>
#include <vector>
using namespace std;

HANDLE sem; 

DWORD WINAPI work(LPVOID f) {
    const char* filename = (char*)f;

    HANDLE h = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) {
        cout << "Ошибка открытия: " << filename << endl;
        return 1;
    }

    DWORD s = GetFileSize(h, NULL);
    if (s == INVALID_FILE_SIZE) {
        cout << "Ошибка чтения размера файла: " << filename << endl;
        CloseHandle(h);
        return 1;
    }

    vector<char> b(s);
    DWORD r;
    if (!ReadFile(h, b.data(), s, &r, NULL) || r == 0) {
        cout << "Ошибка чтения файла: " << filename << endl;
        CloseHandle(h);
        return 1;
    }
    CloseHandle(h);

    for (char& c : b) c = toupper(c);

    WaitForSingleObject(sem, INFINITE);
    HANDLE out = CreateFileA("result.txt", FILE_APPEND_DATA, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (out == INVALID_HANDLE_VALUE) {
        cout << "Ошибка открытия result.txt" << endl;
        ReleaseSemaphore(sem, 1, NULL);
        return 1;
    }

    DWORD written;
    if (!WriteFile(out, b.data(), r, &written, NULL) || written != r) {
        cout << "Ошибка записи в result.txt" << endl;
    }
    CloseHandle(out);
    ReleaseSemaphore(sem, 1, NULL);
    return 0;
}

int main() {
    setlocale(LC_ALL, "");
    vector<const char*> f = { "file1.txt", "file2.txt", "file3.txt" };
    HANDLE hFile = CreateFileA("result.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        cout << "Ошибка создания result.txt" << endl;
        return 1;
    }
    CloseHandle(hFile);

    sem = CreateSemaphore(NULL, 1, 1, NULL);
    vector<HANDLE> t;
    for (auto x : f) t.push_back(CreateThread(NULL, 0, work, (LPVOID)x, 0, NULL));

    for (auto x : t) WaitForSingleObject(x, INFINITE), CloseHandle(x);
    CloseHandle(sem);
    cout << "Готово!" << endl;
}
