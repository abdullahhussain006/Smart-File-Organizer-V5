#include <iostream>
#include <string>

using namespace std;

void showWelcomeMessage();
string getFolderPath();
bool isValidFolder(const string& folderPath);
void showFiles(const string& folderPath);

int main()
{
    showWelcomeMessage();

    string folderPath = getFolderPath();

    if (!isValidFolder(folderPath))
    {
        cout << "\nInvalid folder path.\n";
        return 1;
    }

    cout << "\nFolder found successfully.\n";

    showFiles(folderPath);

    return 0;
}