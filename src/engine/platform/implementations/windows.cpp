/*
================================================================================
  Copyright (c) 2023, Pandemos
  All rights reserved.
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  * Neither the name of the organization nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
================================================================================
*/

#include "engine.hpp"

#if defined __PLATFORM_WINDOWS

std::string implementation::windows::filesystem::seperator() {
    return "\\";
}

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

bool implementation::windows::filesystem::mkdir(std::string path, unsigned int mask) {
    return(CreateDirectory(utilities::type_cast<std::wstring>(path).c_str(), NULL) ? true : false);
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

    results.resize(13);

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

std::vector<std::string> implementation::windows::filesystem::read_directory(std::string path, bool hidden) {
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
        if (hidden == true || last->cFileName[0] != '.') {
            results.push_back(utilities::type_cast<std::string>(last->cFileName));
        }
    } while (FindNextFile(handle, last) != 0);

    return results;
}

bool implementation::windows::filesystem::is_directory(std::string path) {
    return filetype(path) == "directory";
}

std::string implementation::windows::filesystem::join(std::vector<std::string> arguments) {
    return utilities::join(seperator(), arguments);
}

std::string implementation::windows::filesystem::dirname(const std::string& path) {
    auto parts = utilities::tokenize(path, seperator());
    parts.pop_back();
    return join(parts);
}
std::string implementation::windows::filesystem::basename(const std::string& path) {
    auto parts = utilities::tokenize(path, seperator());
    return parts.back();
}

std::string implementation::windows::filesystem::home(const std::string& path) {
    if (path.empty() == false) {
        _home = path;
    }
    if (_home.empty() == false) {
        return _home;
    }
    WCHAR value[MAX_PATH];
    HRESULT result = SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, value);
    if (SUCCEEDED(result)) {
        return utilities::type_cast<std::string>(value);
    }
    return "";
}

std::string implementation::windows::filesystem::appdata(const std::string& path) {
    if (path.empty() == false) {
        _appdata = path;
    }
    if (_appdata.empty() == false) {
        return _appdata;
    }
    WCHAR value[MAX_PATH];
    HRESULT result = SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, value);
    if (SUCCEEDED(result)) {
        return utilities::type_cast<std::string>(value);
    }
    return "";
}

void implementation::windows::assets::init(void* ref) {
    base = (char*)ref;
}

std::vector<std::string> implementation::windows::assets::list(const std::string& path, const std::string& type) {
    if (type.empty()) {
        return filesystem().read_directory(filesystem().join({ base, path, "\\*"}));
    }
    std::vector<std::string> results;
    for (auto entry : filesystem().read_directory(filesystem().join({ base, path, "\\*" }))) {
        if (filesystem().filetype(filesystem().join({ base, path, entry })) == type) {
            results.push_back(entry);
        }
    }
    return results;
}

std::istream& implementation::windows::assets::retrieve(const std::string& path) {
    auto file = new std::ifstream();
    if (file == NULL) {
        // TODO : care about this
    }

    std::vector<std::string> directories = { base };
    for (auto path : utilities::tokenize(resolve(path), "/")) {
        directories.push_back(path);
    }
    auto asset = filesystem().join(directories);

    file->open(asset.c_str(), std::ios::in | std::ios::binary);
    if (file->is_open() == false) {
        event(asset + ", failed to retrieve asset");
    }

    // push onto the stack regardless of success or failure
    assets::source entry = { utilities::dirname(path), file };
    stack.push_back(entry);

    return *file;
}

void implementation::windows::assets::release() {
    if (stack.size() == 0) {
        return;
    }
    std::ifstream *ref = (std::ifstream *)stack.back().stream;
    if (ref != NULL) {
        ref->close();
        delete ref;
    }
    stack.pop_back();
}

std::string implementation::windows::assets::load(const std::string& type, const std::string& resource, const std::string& id) {
    if (loader == NULL) {
        loader = new implementation::universal::assets();
        loader->copy(*this);
    }
    return loader->load(this, type, resource, id);
}

std::string implementation::windows::network::hostname() {
    char hostname[128];

    WSADATA WSData;
    unsigned long version_major = 1;
    unsigned long version_minor = 1;
    if (WSAStartup(MAKEWORD(version_major, version_minor), &WSData)) {
        //std::cerr<<"ERROR: Cannot find Winsock (v"<<version_major<<"."<<version_minor<<" or later)!"<<std::endl;
    }

    if (gethostname(reinterpret_cast<char*>(hostname), 128)) {
        printf("error getting hostname\n");
    }

    return(std::string(hostname));
}

std::string implementation::windows::network::ip(std::string hostname) {
    hostent* resolv = gethostbyname(hostname.empty() ? network::hostname().c_str() : hostname.c_str());
    return utilities::type_cast<std::string>(inet_ntoa(*(LPIN_ADDR) * (resolv->h_addr_list)));
}

std::string implementation::windows::network::mac() {
    /*
      IP_ADAPTER_INFO AdapterInfo[16];
      DWORD dwBufLen = sizeof(AdapterInfo);

      DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);
      //assert(dwStatus == ERROR_SUCCESS);

      PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;

      do {
        PrintMACaddress(pAdapterInfo->Address);
        pAdapterInfo = pAdapterInfo->Next;

      }
      while(pAdapterInfo);
    */
    return(std::string());
}

unsigned long implementation::windows::network::pid() {
    return(GetCurrentProcessId());
}

implementation::windows::network::client::client() {
    WSADATA WSData;
    unsigned long version_major = 1;
    unsigned long version_minor = 1;
    if (WSAStartup(MAKEWORD(version_major, version_minor), &WSData)) {
        //std::cerr<<"ERROR: Cannot find Winsock (v"<<version_major<<"."<<version_minor<<" or later)!"<<std::endl;
    }
}

implementation::windows::network::client::~client() {
}

void implementation::windows::network::client::connect() {
    SOCKET server;
    SOCKADDR_IN addr;

    server = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5555);

    ::connect(server, (SOCKADDR*)&addr, sizeof(addr));

    char buffer[1024] = { 'h', 'e', 'l', 'l', 'o', '.' };
    ::send(server, buffer, sizeof(buffer), 0);

    //closesocket(server);
    //WSACleanup();
    //std::cout << "Socket closed." << std::endl << std::endl;
}

implementation::windows::network::server::server() {
    WSADATA WSData;
    unsigned long version_major = 1;
    unsigned long version_minor = 1;
    if (WSAStartup(MAKEWORD(version_major, version_minor), &WSData)) {
        //std::cerr<<"ERROR: Cannot find Winsock (v"<<version_major<<"."<<version_minor<<" or later)!"<<std::endl;
    }
}

implementation::windows::network::server::~server() {
}

void* start_client(void* instance) {
    auto reference = (implementation::windows::network::client*)instance;

    //char incoming[1024];

    std::vector<char> incoming(1024);

    while (1) {
        if (reference->pending) {
            std::vector<char> bytes;
            reference->sending.lock();
            reference->output.swap(bytes);
            reference->pending = false;
            reference->sending.unlock();
            ::send(reference->connection, bytes.data(), bytes.size(), 0);
        }

        recv(reference->connection, incoming.data(), incoming.size(), 0);
        reference->receive(incoming);
        for (auto handler : reference->parent->callbacks) {
            handler(reference);
        }
    }

    //closesocket(client);

    pthread_exit(NULL);
}

void* start_server(void *instance) {
    auto reference = (implementation::windows::network::server*)instance;

    SOCKET server, client;
    SOCKADDR_IN serverAddr, clientAddr;

    server = socket(AF_INET, SOCK_STREAM, 0);

    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5555);

    bind(server, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
    listen(server, 0);

    char buffer[1024];
    int clientAddrSize = sizeof(clientAddr);
    while ((client = accept(server, (SOCKADDR*)&clientAddr, &clientAddrSize)) != INVALID_SOCKET)
    {
        auto instantiated = (implementation::windows::network::client *)reference->add("something", new implementation::windows::network::client());
        instantiated->connection = client;
        instantiated->parent = reference;
        pthread_create(&instantiated->thread, NULL, start_client, (void*)instantiated);
    }

    pthread_exit(NULL);
    return NULL;
}

void implementation::windows::network::server::start() {
    pthread_create(&thread, NULL, start_server, (void*)this);
}

void implementation::windows::network::server::stop() {
}

#endif
