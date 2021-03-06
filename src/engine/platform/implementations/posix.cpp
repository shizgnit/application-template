#include "engine.hpp"

#if defined __PLATFORM_POSIX

bool implementation::posix::filesystem::cp(std::string src, std::string dest) {
    long fdin, fdout;
    void* sbuf, * dbuf;
    struct stat sst;

    if ((fdin = open(src.c_str(), O_RDONLY)) < 0) {
        printf("error, unable to open source file\n");
        return(false);
    }

    if (fstat(fdin, &sst) < 0) {
        printf("error, unable to stat source file\n");
        return(false);
    }

    if ((fdout = open(dest.c_str(), O_RDWR | O_CREAT | O_TRUNC, sst.st_mode)) < 0) {
        printf("error, unable to open target file.\n");
        return(false);
    }

    if (lseek(fdout, sst.st_size - 1, SEEK_SET) == -1) {
        printf("error\n");
        return(false);
    }

    if (write(fdout, "", 1) != 1) {
        printf("error\n");
        return(false);
    }

    if ((sbuf = mmap(0, sst.st_size, PROT_READ, MAP_FILE | MAP_SHARED, fdin, 0)) == (caddr_t)-1) {
        printf("error\n");
        return(false);
    }

    if ((dbuf = mmap(0, sst.st_size, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, fdout, 0)) == (caddr_t)-1) {
        printf("error\n");
        return(false);
    }

    memcpy(dbuf, sbuf, sst.st_size);

    return(true);
}

bool implementation::posix::filesystem::rm(std::string filename) {
    return remove(filename.c_str());
}

bool implementation::posix::filesystem::mv(std::string src, std::string dest) {
    return rename(src.c_str(), dest.c_str());
}

mode_t _mask(std::string& mask) {
    mode_t bytes = 0;

    if (mask.empty()) {
        return(bytes);
    }

    const char* cmask = mask.c_str();
    int offset = strlen(cmask);

    bool valid = false;

    /* TODO: fix
    std::string octal;
    if (offset == 3 && mask <= 511 && mask > 0) {
        octal = base(mask, 8);
        cmask = octal.c_str();
        offset = strlen(cmask);
        valid = true;
    }
    if (offset == 4 && mask <= 7777 && mask > 0) {
        valid = true;
    }

    if (valid) {
        char oth = (int)(cmask[offset - 1] - 48);
        char grp = (int)(cmask[offset - 2] - 48);
        char usr = (int)(cmask[offset - 3] - 48);
        if (offset == 4) {
            char stk = (int)(cmask[offset - 4] - 48);
        }
        bytes |= (oth);
        bytes |= (grp << 3);
        bytes |= (usr << 6);
    }
    */

    return(bytes);
}

bool implementation::posix::filesystem::mkdir(std::string path, std::string mask) {
    return(::mkdir(path.c_str(), _mask(mask)));
}

bool implementation::posix::filesystem::rmdir(std::string path) {
    return(::rmdir(path.c_str()));
}

std::string implementation::posix::filesystem::pwd(std::string path) {
    std::string result;
    static char current[2048];

    if (path.empty() == false) {
        chdir(path.c_str());
    }
    getcwd(current, 2048);
    result = current;
    return(result);
}

std::vector<unsigned long> implementation::posix::filesystem::stat(std::string path) {
    std::vector<unsigned long> results;
    struct stat sst;

    if (path.empty()) {
        return(results);
    }

    if (::stat(path.c_str(), &sst) < 0) {
        return(results);
    }

    results.reserve(13);

    results[12] = sst.st_blocks;
    results[11] = sst.st_blksize;
    results[10] = sst.st_ctime;
    results[9] = sst.st_mtime;
    results[8] = sst.st_atime;
    results[7] = sst.st_size;
    results[6] = sst.st_rdev;
    results[5] = sst.st_gid;
    results[4] = sst.st_uid;
    results[3] = sst.st_nlink;
    results[2] = sst.st_mode;
    results[1] = sst.st_dev;
    results[0] = sst.st_ino;

    return(results);
}

std::vector<unsigned long> implementation::posix::filesystem::lstat(std::string path) {
    std::vector<unsigned long> results;
    struct stat sst;

    if (path.empty()) {
        return(results);
    }

    if (::lstat(path.c_str(), &sst) < 0) {
        return(results);
    }

    results.reserve(13);

    results[12] = sst.st_blocks;
    results[11] = sst.st_blksize;
    results[10] = sst.st_ctime;
    results[9] = sst.st_mtime;
    results[8] = sst.st_atime;
    results[7] = sst.st_size;
    results[6] = sst.st_rdev;
    results[5] = sst.st_gid;
    results[4] = sst.st_uid;
    results[3] = sst.st_nlink;
    results[2] = sst.st_mode;
    results[1] = sst.st_dev;
    results[0] = sst.st_ino;

    return(results);
}

bool implementation::posix::filesystem::exists(std::string path) {
    struct stat sst;
    return(::stat(path.c_str(), &sst) == 0);
}


std::string implementation::posix::filesystem::filetype(std::string path) {
    std::string result;

    std::vector<unsigned long> stats = stat(path);
    if (stats.size()) {
        unsigned long mode = stats[2];

        if (S_ISREG(mode)) { result = "regular"; }
        if (S_ISDIR(mode)) { result = "directory"; }
        if (S_ISCHR(mode)) { result = "character"; }
        if (S_ISBLK(mode)) { result = "block"; }
        if (S_ISFIFO(mode)) { result = "fifo"; }

#if defined S_ISLNK
        if (S_ISLNK(mode)) { result = "link"; }
#endif

#if defined S_ISSOCK
        if (S_ISSOCK(mode)) { result = "socket"; }
#endif
    }

    return(result);
}

std::pair<int, std::string> implementation::posix::filesystem::error() {
    // TODO: need implementation
    return std::pair<int, std::string>(0, "");
}

std::vector<std::string> implementation::posix::filesystem::read_directory(std::string path) {
    std::vector<std::string> results;

    auto handle = opendir(path.c_str());
    if (handle == NULL) {
        return results;
    }
    while(auto last = readdir(handle)) {
        results.push_back(last->d_name);
    }
    closedir(handle);

    return results;
}

bool implementation::posix::filesystem::is_directory(std::string path) {
    return filetype(path) == "directory";
}

std::string implementation::posix::network::hostname() {
    char hostname[128];

    if (gethostname(reinterpret_cast<char*>(hostname), 128)) {
        //DEBUG("Failed to obtain host name");
    }

    return(std::string(hostname));
}


std::string implementation::posix::network::ip(std::string hostname) {
    hostent* resolv = gethostbyname(hostname.c_str());
    if (resolv == NULL) {
        //DEBUG("Failed to resolve host IP, %s", hostname);
    }

    char addr[16];
    if (inet_ntop((*resolv).h_addrtype, (*resolv).h_addr_list[0], reinterpret_cast<char*>(addr), 16)) {
        //DEBUG("Failed to convert address, %s", hostname);
    }

    return(std::string(addr));
}

std::string implementation::posix::network::mac() {
    return(std::string());
}

unsigned long implementation::posix::network::pid() {
    return(getpid());
}

implementation::posix::network::client::client() {

}

implementation::posix::network::client::~client() {

}

void implementation::posix::network::client::connect() {

}

implementation::posix::network::server::server() {

}

implementation::posix::network::server::~server() {

}

void implementation::posix::network::server::start() {

}

void implementation::posix::network::server::stop() {

}

#endif