#include <windows.h>
#include <iostream>
#include <string>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

bool copyFolderToUSB(const std::wstring& sourcePath, const std::wstring& usbDrive) {
    std::wstring destPath = usbDrive + L"\\backup\\" + fs::path(sourcePath).filename().wstring();

    try {
        if (!fs::exists(usbDrive + L"\\backup")) {
            fs::create_directory(usbDrive + L"\\backup");
        }

        fs::copy(sourcePath, destPath, fs::copy_options::recursive | fs::copy_options::update_existing);
        std::wcout << L"Successfully copied: " << sourcePath << L" to " << destPath << std::endl;
        return true;
    }
    catch (const fs::filesystem_error& e) {
        std::wcout << L"Error copying folder: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::wstring> getUSBDrives() {
    std::vector<std::wstring> usbDrives;
    WCHAR drives[MAX_PATH];
    if (GetLogicalDriveStringsW(MAX_PATH, drives)) {
        for (LPWSTR drive = drives; *drive; drive += 4) {
            if (GetDriveTypeW(drive) == DRIVE_REMOVABLE) {
                std::wstring drivePath(drive);
                drivePath = drivePath.substr(0, drivePath.length() - 1); // Remove trailing backslash
                usbDrives.push_back(drivePath);
            }
        }
    }
    return usbDrives;
}

int main() {
    // List of folders to backup
    std::vector<std::wstring> foldersToBackup = {
        L"C:\\Important_Documents",
        L"C:\\Work_Files",
        L"C:\\Projects"
    };

    while (true) {
        std::vector<std::wstring> usbDrives = getUSBDrives();

        if (!usbDrives.empty()) {
            std::wcout << L"USB drives found:" << std::endl;
            for (const auto& drive : usbDrives) {
                std::wcout << drive << std::endl;

                // Copy each folder to the USB drive
                for (const auto& folder : foldersToBackup) {
                    if (fs::exists(folder)) {
                        copyFolderToUSB(folder, drive);
                    }
                }
            }
        }
        else {
            std::wcout << L"No USB drives detected. Waiting..." << std::endl;
        }

        // Wait for 5 seconds before checking again
        Sleep(5000);
    }

    return 0;
}
