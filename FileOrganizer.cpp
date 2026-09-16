#include <iostream>
#include <string>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

void showWelcomeMessage()
{
    cout << "====================================" << endl;
    cout << "      SMART FILE ORGANIZER V5       " << endl;
    cout << "====================================" << endl;
}

string getFolderPath()
{
    string folderPath;

    cout << "Enter folder path: ";
    getline(cin, folderPath);

    return folderPath;
}

bool isValidFolder(const string& folderPath)
{
    return fs::exists(folderPath) && fs::is_directory(folderPath);
}

void showFiles(const string& folderPath)
{
    cout << "\nFiles found:\n";
    cout << "------------------------------------\n";

    for (const auto& entry : fs::directory_iterator(folderPath))
    {
        if (entry.is_regular_file())
        {
            cout << entry.path().filename().string() << endl;
        }
    }

    cout << "------------------------------------\n";
}