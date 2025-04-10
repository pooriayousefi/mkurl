#include <iostream>
#include <string>
#include <filesystem>
#include <exception>

int main()
{
    try
    {
        // Get the current executable's folder path
        auto cwd = std::filesystem::current_path().string();

        // Get the shortcut name from the user
        std::string shortcut_name;
        std::cout << "Enter Shortcut Name: ";
        std::getline(std::cin, shortcut_name);
        shortcut_name += ".url"; // Append .url extension

        // Get the target URL from the user
        std::string target_URL;
        std::cout << "Enter Target URL: ";
        std::getline(std::cin, target_URL);

        // Construct the PowerShell command
        std::string command = "powershell -Command \"$shortcut = (New-Object -COM WScript.Shell).CreateShortcut('"
            + cwd + "\\" + shortcut_name + "'); $shortcut.TargetPath = '"
            + target_URL + "'; $shortcut.Save();\"";

        // Execute the command
        int result = system(command.c_str());

        // Check the result
        if (result == 0)
            std::cout << "Shortcut created successfully in the current folder.\n";
        else
            std::cerr << "Failed to create the shortcut. Check the input or system permissions.\n";

        return 0;
    }
    catch (const std::exception& xxx)
    {
        std::cerr << xxx.what() << std::endl;
        return 1;
    }
}