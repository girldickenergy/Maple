#include "Storage.h"

#include "xorstr.h"

#include <filesystem>
#include <fstream>

bool Storage::MatchWildcard(const char* first, const char* second)
{
    if (*first == '\0' && *second == '\0')
        return true;

    if (*first == '*')
    {
        while (*(first + 1) == '*')
            first++;
    }

    if (*first == '*' && *(first + 1) != '\0' && *second == '\0')
        return false;

    if (*first == '?' || *first == *second)
        return MatchWildcard(first + 1, second + 1);

    if (*first == '*')
        return MatchWildcard(first + 1, second) || MatchWildcard(first, second + 1);

    return false;
}

Storage::Storage(const std::string& basePath)
{
    m_BasePath = basePath;
}

std::string Storage::GetBasePath()
{
    return m_BasePath;
}

std::string Storage::GetFullPath(const std::string& path)
{
    return m_BasePath + xorstr_("\\") + path;
}

bool Storage::Exists(const std::string& path)
{
    return std::filesystem::exists(GetFullPath(path));
}

bool Storage::ExistsDirectory(const std::string& path)
{
    return std::filesystem::is_directory(GetFullPath(path));
}

void Storage::Create(const std::string& path)
{
    if (Exists(path))
        return;

    const std::string directoryPath = std::filesystem::path(path).parent_path().string();

    if (!ExistsDirectory(directoryPath))
        CreateDirectory(directoryPath);

    std::ofstream ofs(GetFullPath(path));
    ofs.close();
}

void Storage::CreateDirectory(const std::string& path)
{
    if (ExistsDirectory(path))
        return;

    std::filesystem::create_directories(GetFullPath(path));
}

void Storage::Delete(const std::string& path)
{
    if (!std::filesystem::is_directory(GetFullPath(path)))
        std::filesystem::remove(GetFullPath(path));
}

void Storage::DeleteDirectory(const std::string& path)
{
    if (std::filesystem::is_directory(GetFullPath(path)))
        std::filesystem::remove_all(GetFullPath(path));
}

std::vector<std::string> Storage::GetFiles(const std::string& path, const std::string& pattern)
{
    auto files = std::vector<std::string>();

    for (const std::filesystem::directory_entry& file : std::filesystem::directory_iterator(GetFullPath(path)))
        if (!file.is_directory() && MatchWildcard(pattern.c_str(), file.path().filename().string().c_str()))
            files.push_back(file.path().filename().string());

    return files;
}

std::vector<std::string> Storage::GetDirectories(const std::string& path)
{
    auto directories = std::vector<std::string>();

    for (const std::filesystem::directory_entry& directory : std::filesystem::directory_iterator(GetFullPath(path)))
        if (directory.is_directory())
            directories.push_back(directory.path().filename().string());

    return directories;
}

std::fstream Storage::GetStream(const std::string& path, int fileOpenMode)
{
    if (!Exists(path))
        Create(path);

    return std::fstream(path, fileOpenMode);
}
