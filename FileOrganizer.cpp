#include <iostream>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cctype>

using namespace std;
namespace fs = filesystem;

struct OrganizerStats
{
    int totalFiles = 0;
    int movedFiles = 0;
    int skippedFiles = 0;
    int failedFiles = 0;
};

string toLower(string text)
{
    transform(text.begin(), text.end(), text.begin(),
              [](unsigned char c)
              {
                  return static_cast<char>(tolower(c));
              });

    return text;
}

void showWelcomeMessage()
{
    cout << "============================================\n";
    cout << "          SMART FILE ORGANIZER V5\n";
    cout << "============================================\n";
}

string getFolderPath()
{
    string folderPath;

    cout << "\nEnter folder path: ";
    getline(cin, folderPath);

    if (!folderPath.empty() && folderPath.front() == '"')
        folderPath.erase(folderPath.begin());

    if (!folderPath.empty() && folderPath.back() == '"')
        folderPath.pop_back();

    return folderPath;
}

bool isValidFolder(const string& folderPath)
{
    try
    {
        return fs::exists(folderPath) &&
               fs::is_directory(folderPath);
    }
    catch (...)
    {
        return false;
    }
}

string getExtension(const fs::path& filePath)
{
    string extension = filePath.extension().string();

    if (!extension.empty() && extension.front() == '.')
        extension.erase(extension.begin());

    return toLower(extension);
}

string getCategory(const string& extension)
{
    static const unordered_map<string, string> categories =
    {
        {"jpg", "Images"},
        {"jpeg", "Images"},
        {"png", "Images"},
        {"gif", "Images"},
        {"bmp", "Images"},
        {"webp", "Images"},
        {"svg", "Images"},

        {"mp4", "Videos"},
        {"mkv", "Videos"},
        {"avi", "Videos"},
        {"mov", "Videos"},
        {"wmv", "Videos"},
        {"webm", "Videos"},

        {"mp3", "Audio"},
        {"wav", "Audio"},
        {"flac", "Audio"},
        {"aac", "Audio"},
        {"ogg", "Audio"},

        {"pdf", "Documents"},
        {"doc", "Documents"},
        {"docx", "Documents"},
        {"txt", "Documents"},
        {"rtf", "Documents"},
        {"odt", "Documents"},

        {"xls", "Spreadsheets"},
        {"xlsx", "Spreadsheets"},
        {"csv", "Spreadsheets"},
        {"ods", "Spreadsheets"},

        {"ppt", "Presentations"},
        {"pptx", "Presentations"},
        {"odp", "Presentations"},

        {"zip", "Archives"},
        {"rar", "Archives"},
        {"7z", "Archives"},
        {"tar", "Archives"},
        {"gz", "Archives"},

        {"cpp", "Code"},
        {"c", "Code"},
        {"h", "Code"},
        {"hpp", "Code"},
        {"java", "Code"},
        {"py", "Code"},
        {"js", "Code"},
        {"html", "Code"},
        {"css", "Code"},
        {"php", "Code"},

        {"exe", "Applications"},
        {"msi", "Applications"}
    };

    auto it = categories.find(extension);

    if (it != categories.end())
        return it->second;

    return "Others";
}

vector<fs::path> scanFolder(const fs::path& folderPath, bool recursive)
{
    vector<fs::path> files;

    try
    {
        if (recursive)
        {
            for (const auto& entry :
                 fs::recursive_directory_iterator(folderPath))
            {
                if (entry.is_regular_file())
                    files.push_back(entry.path());
            }
        }
        else
        {
            for (const auto& entry :
                 fs::directory_iterator(folderPath))
            {
                if (entry.is_regular_file())
                    files.push_back(entry.path());
            }
        }
    }
    catch (const fs::filesystem_error& error)
    {
        cerr << "Scan error: " << error.what() << '\n';
    }

    return files;
}

void displayFileList(const vector<fs::path>& files)
{
    cout << "\nFiles found: " << files.size() << "\n";
    cout << "--------------------------------------------\n";

    for (size_t i = 0; i < files.size(); ++i)
    {
        cout << i + 1 << ". "
             << files[i].filename().string()
             << " [" << getCategory(getExtension(files[i])) << "]\n";
    }

    cout << "--------------------------------------------\n";
}

void displayCategorySummary(const vector<fs::path>& files)
{
    unordered_map<string, int> summary;

    for (const auto& file : files)
    {
        summary[getCategory(getExtension(file))]++;
    }

    vector<pair<string, int>> categories(
        summary.begin(),
        summary.end()
    );

    sort(categories.begin(), categories.end());

    cout << "\nCategory Summary\n";
    cout << "--------------------------------------------\n";

    for (const auto& category : categories)
    {
        cout << category.first
             << ": "
             << category.second
             << " file(s)\n";
    }

    cout << "--------------------------------------------\n";
}

fs::path createUniquePath(const fs::path& destination)
{
    if (!fs::exists(destination))
        return destination;

    string stem = destination.stem().string();
    string extension = destination.extension().string();

    fs::path parent = destination.parent_path();

    int counter = 1;

    while (true)
    {
        fs::path newPath =
            parent /
            (stem + "_" + to_string(counter) + extension);

        if (!fs::exists(newPath))
            return newPath;

        counter++;
    }
}

bool isInsideOrganizerFolder(
    const fs::path& file,
    const fs::path& organizerFolder)
{
    try
    {
        fs::path fileAbsolute = fs::absolute(file);
        fs::path folderAbsolute = fs::absolute(organizerFolder);

        return fileAbsolute.string().find(
                   folderAbsolute.string() + fs::path::preferred_separator
               ) == 0;
    }
    catch (...)
    {
        return false;
    }
}

void previewOrganization(
    const vector<fs::path>& files,
    const fs::path& rootFolder)
{
    cout << "\nOrganization Preview\n";
    cout << "============================================\n";

    for (const auto& file : files)
    {
        string category =
            getCategory(getExtension(file));

        fs::path destinationFolder =
            rootFolder / category;

        fs::path destination =
            destinationFolder / file.filename();

        destination = createUniquePath(destination);

        cout << file.filename().string()
             << "  -->  "
             << category
             << "/"
             << destination.filename().string()
             << '\n';
    }

    cout << "============================================\n";
}

void organizeFiles(
    const vector<fs::path>& files,
    const fs::path& rootFolder,
    OrganizerStats& stats)
{
    for (const auto& file : files)
    {
        stats.totalFiles++;

        try
        {
            string category =
                getCategory(getExtension(file));

            fs::path categoryFolder =
                rootFolder / category;

            fs::create_directories(categoryFolder);

            fs::path destination =
                categoryFolder / file.filename();

            destination = createUniquePath(destination);

            if (fs::equivalent(file, destination))
            {
                stats.skippedFiles++;
                continue;
            }

            fs::rename(file, destination);

            stats.movedFiles++;

            cout << "[MOVED] "
                 << file.filename().string()
                 << " -> "
                 << category
                 << "/"
                 << destination.filename().string()
                 << '\n';
        }
        catch (const fs::filesystem_error& error)
        {
            stats.failedFiles++;

            cerr << "[FAILED] "
                 << file.filename().string()
                 << " : "
                 << error.what()
                 << '\n';
        }
    }
}

void displayStatistics(const OrganizerStats& stats)
{
    cout << "\n============================================\n";
    cout << "              ORGANIZATION REPORT\n";
    cout << "============================================\n";

    cout << "Files processed : " << stats.totalFiles << '\n';
    cout << "Files moved     : " << stats.movedFiles << '\n';
    cout << "Files skipped   : " << stats.skippedFiles << '\n';
    cout << "Files failed    : " << stats.failedFiles << '\n';

    cout << "============================================\n";
}