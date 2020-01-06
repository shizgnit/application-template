#include "engine.hpp"

#if defined __PLATFORM_LINUX

bool posix::filesystem::cp(std::string src, std::string dest) {
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

bool posix::filesystem::rm(std::string filename) {
    remove(filename.c_str());
}

bool posix::filesystem::mv(std::string src, std::string dest) {
    rename(src.c_str(), dest.c_str());
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

bool posix::filesystem::mkdir(std::string path, std::string mask) {
    return(::mkdir(path.c_str(), _mask(mask)));
}

bool posix::filesystem::rmdir(std::string path) {
    return(::rmdir(path.c_str()));
}

std::string posix::filesystem::pwd(std::string path) {
    std::string result;
    static char current[2048];

    if (path.empty() == false) {
        chdir(path.c_str());
    }
    getcwd(current, 2048);
    result = current;
    return(result);
}

std::vector<unsigned long> posix::filesystem::stat(std::string path) {
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

std::vector<unsigned long> posix::filesystem::lstat(std::string path) {
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

bool posix::filesystem::exists(std::string path) {
    struct stat sst;
    return(::stat(path.c_str(), &sst) == 0);
}


std::string posix::filesystem::filetype(std::string path) {
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

std::pair<int, std::string> posix::filesystem::error() {
    // TODO: need implementation
    return std::pair<int, std::string>(0, "");
}

#endif