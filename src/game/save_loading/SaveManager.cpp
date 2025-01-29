#include <game/save_loading/SaveManager.hpp>

bool FileAPI::DirectoryExists(const std::string& path) {
#ifdef FILEAPI_VERBOSE
	DEBUG_LOG("Checking if \"" + path + "\" exists");
#endif
	DWORD dwAttrib = GetFileAttributesA(path.c_str());
	return dwAttrib != INVALID_FILE_ATTRIBUTES &&
		(dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

bool FileAPI::FileExists(const std::string& path) {
#ifdef FILEAPI_VERBOSE
	DEBUG_LOG("Checking if \"" + path + "\" exists");
#endif
	DWORD dwAttrib = GetFileAttributesA(path.c_str());
	return dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

void FileAPI::CreateDirectory(const std::string& path) {
#ifdef FILEAPI_VERBOSE
	DEBUG_LOG("Creating directory at \"" + path + "\"");
#endif
	if (!CreateDirectoryA(path.c_str(), 0))
		DEBUG_LOG("Cant create directory at \"" + path + "\". Error code : " + std::to_string(GetLastError()));
}

void FileAPI::CreateFile(const std::string& path) {
#ifdef FILEAPI_VERBOSE
	DEBUG_LOG("Creating file at \"" + path + "\"");
#endif
	HANDLE h = CreateFileA(path.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (h)
		CloseHandle(h);
	else
		DEBUG_LOG("Cant create file at \"" + path + "\". Error code: " + std::to_string(GetLastError()));
}

void FileAPI::RemoveDirectory(const std::string& path, bool recursively) {
#ifdef FILEAPI_VERBOSE
	DEBUG_LOG("Removing directory at \"" + path + "\"");
#endif
	if (!isEmptyDirectory(path)) {
		if (recursively)
			removeContents(path);
		else
			return;
	}
	
	removeEmptyDirectory(path);
}

void FileAPI::RemoveFile(const std::string& path) {
#ifdef FILEAPI_VERBOSE
	DEBUG_LOG("Removing file at \"" + path + "\"");
#endif
	if (!DeleteFileA(path.c_str()))
		DEBUG_LOG("Cant remove file at \"" + path + "\". Error code: " + std::to_string(GetLastError()));
}

std::vector<std::string> FileAPI::ListFiles(const std::string& path) {
#ifdef FILEAPI_VERBOSE
	DEBUG_LOG("Listing files at \"" + path + "\"");
#endif
	std::string correctedPath = path + "\\*";
	WIN32_FIND_DATAA ffd;
	HANDLE hFind = FindFirstFileA(correctedPath.c_str(), &ffd);
	
	if (hFind == INVALID_HANDLE_VALUE)
		return {};

	std::vector<std::string> result;
	do {
		std::string filename = ffd.cFileName;
		if (filename == "." || filename == "..")
			continue;

		result.push_back(ffd.cFileName);
	} while (FindNextFileA(hFind, &ffd) != 0);

	FindClose(hFind);

	return result;
}

bool FileAPI::IsDirectory(const std::string& path) {
	return DirectoryExists(path);
}

void FileAPI::removeEmptyDirectory(const std::string& path) {
	if (!RemoveDirectoryA(path.c_str()))
		DEBUG_LOG("Cant remove the empty directory at \"" + path + "\". Error code: " + std::to_string(GetLastError()));
}

void FileAPI::removeContents(const std::string& path) {
	auto filesInside = ListFiles(path);

	for (auto& file : filesInside) {
		std::string fullPath = path + "\\" + file;

		if (IsDirectory(fullPath))
			RemoveDirectory(fullPath);
		else
			RemoveFile(fullPath);
	}
}

bool FileAPI::isEmptyDirectory(const std::string& path) {
	return ListFiles(path).empty();
}



SaveInstance::SaveInstance(const std::string& name) :
	m_Name(name) {
	if (!Exists(m_Name))
		create();
}

std::unique_ptr<WholeSaveLoader> SaveInstance::LoadSave() {
	return std::make_unique<WholeSaveLoader>(m_Name);
}

void SaveInstance::RemoveSave() {
	if(Exists(m_Name))
		FileAPI::RemoveDirectory(WholeSaveLoader::c_SavesPath + m_Name);
}

bool SaveInstance::Exists(const std::string& name) {
	if (!FileAPI::DirectoryExists(WholeSaveLoader::c_SavesPath + name))
		return false;

	if (!FileAPI::FileExists(WholeSaveLoader::c_SavesPath + name + "\\" + WholeSaveLoader::c_GeneralFileName))
		return false;

	return true;
}

void SaveInstance::create() {
	FileAPI::CreateDirectory(WholeSaveLoader::c_SavesPath + m_Name);
	FileAPI::CreateFile(WholeSaveLoader::c_SavesPath + m_Name + "\\" + WholeSaveLoader::c_GeneralFileName);
}

SaveManager::SaveManager() {

}

std::unique_ptr<SaveInstance> SaveManager::CreateSave(const std::string& name) {
	return std::make_unique<SaveInstance>(name);
}

std::vector<SaveInstance> SaveManager::GetSaves() {
	auto files = FileAPI::ListFiles(WholeSaveLoader::c_SavesPath);
	std::vector<SaveInstance> result;

	for (auto& file : files) {
		if (FileAPI::IsDirectory(WholeSaveLoader::c_SavesPath + file) &&
			SaveInstance::Exists(file))
			result.push_back(SaveInstance(file));
	}

	return result;
}

std::unique_ptr<SaveInstance> SaveManager::GetSaveByName(const std::string& name) {
	if (IsSavePresent(name))
		return std::make_unique<SaveInstance>(name);
	return nullptr;
}

bool SaveManager::IsSavePresent(const std::string& name) {
	return SaveInstance::Exists(name);
}