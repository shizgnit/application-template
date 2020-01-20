#include "engine.hpp"

#if defined __PLATFORM_WINDOWS

bool implementation::windows::filesystem::cp(std::string srcfile, std::string dest) {
    DWORD dwAttrs;

#if defined WIDE
    if (CopyFile(utilities::type_cast<std::wstring>(srcfile).c_str(), utilities::type_cast<std::wstring>(dest).c_str(), FALSE)) {
        dwAttrs = GetFileAttributes(utilities::type_cast<std::wstring>(srcfile).c_str());
        if (!(dwAttrs & FILE_ATTRIBUTE_READONLY)) {
            SetFileAttributes(utilities::type_cast<std::wstring>(dest).c_str(), dwAttrs | FILE_ATTRIBUTE_READONLY);
        }
    }
#else
    if (CopyFile(utilities::type_cast<std::string>(srcfile).c_str(), utilities::type_cast<std::string>(dest).c_str(), FALSE)) {
        dwAttrs = GetFileAttributes(utilities::type_cast<std::string>(srcfile).c_str());
        if (!(dwAttrs & FILE_ATTRIBUTE_READONLY)) {
            SetFileAttributes(utilities::type_cast<std::string>(dest).c_str(), dwAttrs | FILE_ATTRIBUTE_READONLY);
        }
    }
#endif
    else {
        return(false);
    }
    return(true);
}

bool implementation::windows::filesystem::rm(std::string filename) {
#if defined WIDE
    if (DeleteFile(utilities::type_cast<std::wstring>(filename).c_str()) == false) {
        auto error = GetLastError();
        return(false);
    }
    return(true);
#else
    return(DeleteFile(utilities::type_cast<std::string>(filename).c_str()));
#endif
}

bool implementation::windows::filesystem::mv(std::string srcfile, std::string dest) {
#if defined WIDE
    return(MoveFile(utilities::type_cast<std::wstring>(srcfile).c_str(), utilities::type_cast<std::wstring>(dest).c_str()));
#else
    return(MoveFile(utilities::type_cast<std::string>(srcfile).c_str(), utilities::type_cast<std::string>(dest).c_str()));
#endif
}

bool implementation::windows::filesystem::mkdir(std::string path, std::string mask) {
    return(CreateDirectory(utilities::type_cast<std::wstring>(path).c_str(), NULL) ? true : false);
    //return(CreateDirectory(utilities::type_cast<std::string>(path).c_str(), NULL) ? true : false);
}

bool implementation::windows::filesystem::rmdir(std::string path) {
#if defined WIDE
    return(RemoveDirectory(utilities::type_cast<std::wstring>(path).c_str()) ? true : false);
#else
    return(RemoveDirectory(utilities::type_cast<std::string>(path).c_str()) ? true : false);
#endif
}

std::string implementation::windows::filesystem::pwd(std::string path) {
    static wchar_t current[2048];
    //static char current[2048];

    if (!path.empty()) {
        SetCurrentDirectory(utilities::type_cast<std::wstring>(path).c_str());
        //SetCurrentDirectory(utilities::type_cast<std::string>(path).c_str());
    }

    GetCurrentDirectory(2048, current);
    auto tmp = std::wstring(current);

    return(std::string(tmp.begin(), tmp.end()));
}


std::vector<unsigned long> implementation::windows::filesystem::stat(std::string path) {
    std::vector<unsigned long> results;

    struct _stat sst;

    if (_stat(path.c_str(), &sst) != 0) {
        return(results);
    }

    results.reserve(13);

    results[12] = 0;
    results[11] = 0;
    results[10] = (unsigned long)sst.st_ctime;
    results[9] = (unsigned long)sst.st_mtime;
    results[8] = (unsigned long)sst.st_atime;
    results[7] = sst.st_size;
    results[6] = 0;
    results[5] = 0;
    results[4] = 0;
    results[3] = 0;
    results[2] = sst.st_mode;
    results[1] = sst.st_dev + 'A';
    results[0] = 0;

    return(results);
}

std::vector<unsigned long> implementation::windows::filesystem::lstat(std::string path) {
    return(stat(path));
}

bool implementation::windows::filesystem::exists(std::string path) {
    struct _stat sst;
    return(_stat(path.c_str(), &sst) == 0);
}

std::string implementation::windows::filesystem::filetype(std::string path) {
    std::string result = "unknown";

    std::vector<unsigned long> stats = stat(path);
    if (stats.size()) {
        unsigned long mode = stats[2];

        if (mode & _S_IFREG) { result = "regular"; }
        if (mode & _S_IFDIR) { result = "directory"; }
    }

    return(result);
}


std::pair<int, std::string> implementation::windows::filesystem::error() {

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    return std::pair<int, std::string>(dw, utilities::type_cast<std::string>((LPTSTR)lpMsgBuf));
}

std::vector<std::string> implementation::windows::filesystem::read_directory(std::string path) {
    std::vector<std::string> results;

    if (path[path.length() - 1] == '\\') {
        path.append("*");
    }

    auto last = new WIN32_FIND_DATA;

    auto handle = FindFirstFile(utilities::type_cast<std::wstring>(path).c_str(), last);

    if(handle == INVALID_HANDLE_VALUE) {
        return results;
    }
    do
    {
        results.push_back(utilities::type_cast<std::string>(last->cFileName));
    } while (FindNextFile(handle, last) != 0);

    return results;
}

bool implementation::windows::filesystem::is_directory(std::string path) {
    return filetype(path) == "directory";
}

void implementation::windows::assets::init(void* ref) {
    base = (char*)ref;
}

std::vector<std::string> implementation::windows::assets::list(std::string path) {
    return filesystem().read_directory(path);
}

std::istream& implementation::windows::assets::retrieve(std::string path) {
    static std::ifstream file;
    std::string asset = base + path;
    if (file.is_open()) {
        file.close();
    }
    file.clear();
    file.open(asset.c_str(), std::ios::in | std::ios::binary);
    return file;
}

#endif