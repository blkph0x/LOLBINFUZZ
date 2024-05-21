#include <iostream>
#include <vector>
#include <string>
#include <windows.h>

// Function to enumerate all .exe files starting from the root drive and store their file paths
void enumerate_exe_files_recursive(const std::wstring& directory, std::vector<std::wstring>& exe_files) {
    std::wstring searchPath = directory + L"\\*.*";
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(searchPath.c_str(), &findFileData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                (wcscmp(findFileData.cFileName, L".") != 0) && (wcscmp(findFileData.cFileName, L"..") != 0)) {
                // If it's a directory and not '.' or '..', recursively enumerate files in it
                std::wstring subDirectory = directory + L"\\" + findFileData.cFileName;
                enumerate_exe_files_recursive(subDirectory, exe_files);
            }
            else if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) &&
                (wcsstr(findFileData.cFileName, L".exe") != nullptr)) {
                // If it's a normal file and ends with ".exe", add its full path to the list
                std::wstring filePath = directory + L"\\" + findFileData.cFileName;
                exe_files.push_back(filePath);
            }
        } while (FindNextFile(hFind, &findFileData) != 0);
        FindClose(hFind);
    }
}

// Function to execute a command with a URL
void execute_command(const std::wstring& command) {
    // Use ShellExecute to execute the command
    ShellExecuteW(NULL, L"open", L"cmd.exe", (L"/c " + command).c_str(), NULL, SW_HIDE);
}

// Function to generate fuzzing commands
std::vector<std::wstring> generate_fuzzing_commands(const std::wstring& exe_file, const std::wstring& base_url) {
    std::vector<std::wstring> commands;
    std::wstring filename = exe_file.substr(exe_file.find_last_of(L"\\") + 1);
    std::wstring url = base_url + L"?Filename=" + filename;

    for (wchar_t c = L'a'; c <= L'z'; ++c) {
        commands.push_back(exe_file + L" /" + c + L" " + url + L"+" + c);
    }
    for (wchar_t c = L'A'; c <= L'Z'; ++c) {
        commands.push_back(exe_file + L" /" + c + L" " + url + L"+" + c);
    }

    return commands;
}
std::vector<std::wstring> generate_fuzzing_commands_1(const std::wstring& exe_file, const std::wstring& base_url) {
    std::vector<std::wstring> commands;
    std::wstring filename = exe_file.substr(exe_file.find_last_of(L"\\") + 1);
    std::wstring url = base_url + L"?Filename=" + filename;

    for (wchar_t c = L'a'; c <= L'z'; ++c) {
        commands.push_back(exe_file + L" -" + c + L" " + url + L"+" + c);
    }
    for (wchar_t c = L'A'; c <= L'Z'; ++c) {
        commands.push_back(exe_file + L" -" + c + L" " + url + L"+" + c);
    }

    return commands;
}
int main(int argc, char* argv[]) {
    std::wstring root = L"C:\\"; // Start at the system root drive
    std::wstring base_url = L"http://127.0.0.1:8080";

    std::vector<std::wstring> exe_files;
    enumerate_exe_files_recursive(root, exe_files);

    for (const auto& exe_file : exe_files) {
        std::wcout << L"Fuzzing file: " << exe_file << std::endl;

        // Start fuzzing process
        std::vector<std::wstring> commands = generate_fuzzing_commands(exe_file, base_url);

        for (const auto& command : commands) {
            std::wcout << L"Sending command: " << command << std::endl;
            execute_command(command);
            // Sleep to avoid overwhelming the system
            Sleep(100);
        }
    }
    for (const auto& exe_file : exe_files) {
        std::wcout << L"Fuzzing file: " << exe_file << std::endl;

        // Start fuzzing process
        std::vector<std::wstring> commands = generate_fuzzing_commands_1(exe_file, base_url);

        for (const auto& command : commands) {
            std::wcout << L"Sending command: " << command << std::endl;
            execute_command(command);
            // Sleep to avoid overwhelming the system
            Sleep(100);
        }
    }
    return 0;
}
