#include <iostream>
#include <string>
#include <filesystem>
#include <vector>

using namespace std;
namespace fs = filesystem;

struct OrganizerStats
{
    int totalFiles = 0;
    int movedFiles = 0;
    int skippedFiles = 0;
    int failedFiles = 0;
};

void showWelcomeMessage();
string getFolderPath();
bool isValidFolder(const string &folderPath);

vector<fs::path> scanFolder(
    const fs::path &folderPath,
    bool recursive);

void displayFileList(
    const vector<fs::path> &files);

void displayCategorySummary(
    const vector<fs::path> &files);

void previewOrganization(
    const vector<fs::path> &files,
    const fs::path &rootFolder);

void organizeFiles(
    const vector<fs::path> &files,
    const fs::path &rootFolder,
    OrganizerStats &stats);

void displayStatistics(
    const OrganizerStats &stats);

int main()
{
    showWelcomeMessage();

    string folderPath = getFolderPath();

    if (!isValidFolder(folderPath))
    {
        cout << "\nInvalid folder path.\n";
        return 1;
    }

    fs::path rootFolder(folderPath);

    cout << "\nFolder found successfully.\n";

    char recursiveChoice;

    cout << "\nScan subfolders too? (y/n): ";
    cin >> recursiveChoice;

    bool recursive =
        recursiveChoice == 'y' ||
        recursiveChoice == 'Y';

    vector<fs::path> files =
        scanFolder(rootFolder, recursive);

    if (files.empty())
    {
        cout << "\nNo files found.\n";
        return 0;
    }

    displayFileList(files);

    displayCategorySummary(files);

    cout << "\nWould you like to preview the organization? (y/n): ";

    char previewChoice;
    cin >> previewChoice;

    if (previewChoice == 'y' ||
        previewChoice == 'Y')
    {
        previewOrganization(files, rootFolder);
    }

    cout << "\nProceed with organization? (y/n): ";

    char organizeChoice;
    cin >> organizeChoice;

    if (organizeChoice != 'y' &&
        organizeChoice != 'Y')
    {
        cout << "\nOperation cancelled.\n";
        return 0;
    }

    OrganizerStats stats;

    organizeFiles(
        files,
        rootFolder,
        stats);

    displayStatistics(stats);

    cout << "\nOrganization completed.\n";

    return 0;
}