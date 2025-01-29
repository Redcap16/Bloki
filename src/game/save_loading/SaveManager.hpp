#pragma once

#include <game/save_loading/WholeSaveLoader.hpp>

#include <windows.h>

#undef CreateDirectory
#undef CreateFile
#undef RemoveDirectory

#define FILEAPI_VERBOSE

class FileAPI {
public:
	static bool DirectoryExists(const std::string& path);
	static bool FileExists(const std::string& path);
	static void CreateDirectory(const std::string& path);
	static void CreateFile(const std::string& path);
	static void RemoveDirectory(const std::string& path, bool recursively = true);
	static void RemoveFile(const std::string& path);
	static std::vector<std::string> ListFiles(const std::string& path);
	static bool IsDirectory(const std::string& path);

private:
	static void removeEmptyDirectory(const std::string& path);
	static void removeContents(const std::string& path);
	static bool isEmptyDirectory(const std::string& path);
};

class SaveInstance {
public:
	SaveInstance(const std::string& name); //If save with that name doesnt exist than create it

	std::unique_ptr<WholeSaveLoader> LoadSave();
	void RemoveSave();
	std::string GetName() { return m_Name; }

	static bool Exists(const std::string& name);
private:
	std::string m_Name;

	void create();
};

class SaveManager {
public:
	SaveManager();

	std::unique_ptr<SaveInstance> CreateSave(const std::string& name);
	std::vector<SaveInstance> GetSaves();
	std::unique_ptr<SaveInstance> GetSaveByName(const std::string& name);
	bool IsSavePresent(const std::string& name);
};