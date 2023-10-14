#pragma once

#include <fstream>
#include <string>
#include <vector>

class Storage
{
    std::string m_BasePath;

    bool MatchWildcard(const char* first, const char* second);

public:
    Storage(const std::string& basePath);

    /**
     * \returns The path to this Storage
     */
    std::string GetBasePath();
    /**
     * \brief Get a usable filesystem path for the provided incomplete path
     * \param path An incomplete path
     * \returns A usable filesystem path
     */
    std::string GetFullPath(const std::string& path);
    /**
     * \brief Check whether a file exists at the specified path
     * \param path The path to check
     * \returns Whether a file exists
     */
    bool Exists(const std::string& path);
    /**
     * \brief Check whether a directory exists at the specified path
     * \param path The path to check
     * \returns Whether a directory exists
     */
    bool ExistsDirectory(const std::string& path);
    /**
     * \brief Creates a new file at the specified path
     * \param path The path to new file
     */
    void Create(const std::string& path);
    /**
     * \brief Creates a new directory at the specified path
     * \param path The path to new directory
     */
    void CreateDir(const std::string& path);
    /**
     * \brief Delete a file
     * \param path The path of the file to delete
     */
    void Delete(const std::string& path);
    /**
     * \brief Delete a directory and all its contents recursively
     * \param path The path of the directory to delete
     */
    void DeleteDirectory(const std::string& path);
    /**
     * \brief Retrieve a list of files at the specified path
     * \param path The path to list
     * \param pattern An optional search pattern. Accepts "*" wildcard
     * \returns A list of files in the path, relative to the path of this Storage
     */
    std::vector<std::string> GetFiles(const std::string& path, const std::string& pattern = "*");
    /**
     * \brief Retrieve a list of directories at the specified path
     * \param path The path to list
     * \returns A list of directories in the path, relative to the path of this Storage
     */
    std::vector<std::string> GetDirectories(const std::string& path);
    /**
     * \brief Retrieve a stream from an underlying file inside this storage
     * \param path The path of the file
     * \param fileOpenMode The mode in which the file should be opened
     * \returns A stream associated with the requested path
     */
    std::fstream GetStream(const std::string& path, int fileOpenMode);
};
