#include <iostream>
#include <string>
#include <filesystem>
#include <exception>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#elif __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>
#endif

class URLShortcutCreator {
public:
    static bool createShortcut(const std::string& name, const std::string& url) {
#ifdef _WIN32
        return createWindowsShortcut(name, url);
#elif __APPLE__
        return createMacOSShortcut(name, url);
#else
        return createLinuxShortcut(name, url);
#endif
    }

private:
#ifdef _WIN32
    static bool createWindowsShortcut(const std::string& name, const std::string& url) {
        try {
            auto cwd = std::filesystem::current_path().string();
            std::string shortcut_name = name + ".url";
            
            // Create .url file directly (more reliable than PowerShell)
            std::ofstream file(cwd + "\\" + shortcut_name);
            if (!file.is_open()) return false;
            
            file << "[InternetShortcut]\n";
            file << "URL=" << url << "\n";
            file << "IconFile=" << url << "\n";
            file << "IconIndex=1\n";
            file.close();
            
            return true;
        } catch (...) {
            return false;
        }
    }
#elif __APPLE__
    static bool createMacOSShortcut(const std::string& name, const std::string& url) {
        try {
            auto cwd = std::filesystem::current_path().string();
            std::string webloc_path = cwd + "/" + name + ".webloc";
            
            // Create .webloc file (macOS web location)
            std::ofstream file(webloc_path);
            if (!file.is_open()) return false;
            
            file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
            file << "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" ";
            file << "\"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n";
            file << "<plist version=\"1.0\">\n";
            file << "<dict>\n";
            file << "    <key>URL</key>\n";
            file << "    <string>" << url << "</string>\n";
            file << "</dict>\n";
            file << "</plist>\n";
            file.close();
            
            return true;
        } catch (...) {
            return false;
        }
    }
#else
    static bool createLinuxShortcut(const std::string& name, const std::string& url) {
        try {
            auto cwd = std::filesystem::current_path().string();
            std::string desktop_path = cwd + "/" + name + ".desktop";
            
            // Create .desktop file (Linux desktop entry)
            std::ofstream file(desktop_path);
            if (!file.is_open()) return false;
            
            file << "[Desktop Entry]\n";
            file << "Version=1.0\n";
            file << "Type=Link\n";
            file << "Name=" << name << "\n";
            file << "Comment=Web shortcut to " << url << "\n";
            file << "URL=" << url << "\n";
            file << "Icon=text-html\n";
            file.close();
            
            // Make executable
            std::filesystem::permissions(desktop_path, 
                std::filesystem::perms::owner_exec | 
                std::filesystem::perms::group_exec | 
                std::filesystem::perms::others_exec,
                std::filesystem::perm_options::add);
            
            return true;
        } catch (...) {
            return false;
        }
    }
#endif
};

int main()
{
    try
    {
        std::cout << "Cross-Platform URL Shortcut Creator\n";
        std::cout << "====================================\n\n";

        // Get the shortcut name from the user
        std::string shortcut_name;
        std::cout << "Enter Shortcut Name: ";
        std::getline(std::cin, shortcut_name);
        
        if (shortcut_name.empty()) {
            std::cerr << "Error: Shortcut name cannot be empty.\n";
            return 1;
        }

        // Get the target URL from the user
        std::string target_URL;
        std::cout << "Enter Target URL: ";
        std::getline(std::cin, target_URL);
        
        if (target_URL.empty()) {
            std::cerr << "Error: URL cannot be empty.\n";
            return 1;
        }
        
        // Validate URL format (basic check)
        if (target_URL.find("http://") != 0 && target_URL.find("https://") != 0) {
            std::cout << "Warning: URL doesn't start with http:// or https://\n";
            std::cout << "Proceeding anyway...\n";
        }

        // Create the shortcut
        bool success = URLShortcutCreator::createShortcut(shortcut_name, target_URL);

        if (success) {
            std::cout << "\nShortcut created successfully!\n";
#ifdef _WIN32
            std::cout << "File: " << shortcut_name << ".url\n";
#elif __APPLE__
            std::cout << "File: " << shortcut_name << ".webloc\n";
#else
            std::cout << "File: " << shortcut_name << ".desktop\n";
#endif
            std::cout << "Location: " << std::filesystem::current_path().string() << "\n";
        } else {
            std::cerr << "Failed to create shortcut. Check permissions and input.\n";
            return 1;
        }

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown error occurred.\n";
        return 1;
    }
}