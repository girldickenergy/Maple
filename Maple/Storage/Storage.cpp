#include "Storage.h"

#include "xorstr.h"

#include <filesystem>

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
        if (!file.is_directory()) // todo: match pattern
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
