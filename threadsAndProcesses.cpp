#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <mutex>
#include <cctype>

using namespace std;

mutex output_mutex;

void readingAndWriting(string input_file, string output_file) 
{
    ifstream in(input_file);
    if (!in) 
    {
        cout << "Ошибка открытия файла: " << input_file << endl;
    }
    else
    {
        string content((istreambuf_iterator<char>(in)), {});
        in.close();

        for (char& c : content) 
        {
            c = toupper(static_cast<unsigned char>(c));
        }

        lock_guard<mutex> lock(output_mutex);

        ofstream out(output_file, ios::app);
        if (!out) 
        {
            cout << "Ошибка открытия выходного файла" << endl;
        }
        else
        {
            out << content;
            out.close();
        }
    }
}

int main() 
{
    setlocale(LC_ALL, "");
    vector<string> input_files = { "file1.txt", "file2.txt", "file3.txt" };
    string output_file = "result.txt";

    ofstream clear(output_file, ios::trunc);
    clear.close();

    vector<thread> threads;
    for (auto& file : input_files) 
    {
        threads.emplace_back(readingAndWriting, file, output_file);
    }

    for (auto& t : threads) 
    {
        t.join();
    }

    cout << "Все файлы обработаны" << endl;
    return 0;
}