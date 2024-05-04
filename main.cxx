/**
 * @file main.cxx
 * @author sarace (sarace@huisa.win)
 * @brief my solution for ls command of
 * [missing-semester](https://missing.csail.mit.edu/2020/shell-tools/)
 * @version 0.1
 * @date 2024-05-04
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <functional>
#include <iomanip>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>

#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <sys/acl.h>
#include <sys/stat.h>
#include <sys/xattr.h>
#include <utility>
#include <vector>

#if defined(_WIN32)
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif

/**
 * @brief return the terminal color control string according to the file mode
 *
 * @param mode file mode
 * @return std::string terminal color control string
 */
auto colorOfMode(const std::string &mode) -> std::string {
  if (mode[0] == 'd') {
    return "\033[1;34m";
  }
  if (mode[0] == '-' && mode[3] == 'x') {
    return "\033[0;31m";
  }
  return "\033[0m";
}

auto isPathExist(const std::string &path) -> bool {
  struct stat pathStat;
  return (stat(path.c_str(), &pathStat) == 0);
}

/**
 * @brief convert file size to human readable string
 *
 * @param size file size
 * @return std::string human readable string of file size
 */
auto convertSize2HumanReadable(off_t size) -> std::string {
  std::stringstream result;
  const std::uint64_t constexpr KB_BORDER = 1024LL;
  const std::uint64_t constexpr MB_BORDER = KB_BORDER * KB_BORDER;
  const std::uint64_t constexpr GB_BORDER = MB_BORDER * KB_BORDER;
  const std::double_t constexpr DIV_MAGIC = 1024.0;
  if (size < KB_BORDER) {
    result << size << 'B';
  } else if (size > KB_BORDER && size < MB_BORDER) {
    double num = size / DIV_MAGIC;
    result << std::fixed << std::setprecision(1) << num << 'K';
  } else if (size > MB_BORDER && size < GB_BORDER) {
    double num = static_cast<double>(size) / (MB_BORDER);
    result << std::fixed << std::setprecision(1) << num << 'M';
  } else if (size > GB_BORDER) {
    double num = static_cast<double>(size) / (GB_BORDER);
    result << std::fixed << std::setprecision(1) << num << 'G';
  }
  return result.str();
}

/**
 * @brief Get the Time Str Of Unix timestamp
 *
 * @param time unix timestamp
 * @return std::string time string
 */
auto getTimeStrOfUnixTime(long time) -> std::string {
  std::time_t time_tmp = time;
  std::stringstream stringstream;
  stringstream << std::put_time(std::localtime(&time_tmp), "%b %d %H:%M");
  return stringstream.str();
}

/**
 * @brief list files of <path>, <path> must be existing
 *
 * @param path path to list
 */
void doListDir(const std::string &path) {
  DIR *dirPtr = opendir(path.c_str());
  if (dirPtr == nullptr) {
    std::cerr << "Could not open directory " << path << std::endl;
    exit(-2);
  }
  struct dirent *ent = nullptr;
  std::vector<std::pair<long, std::string>> container;

  while ((ent = readdir(dirPtr)) != nullptr) {
    std::string fullPath = path + PATH_SEP + ent->d_name;
    struct stat fullPathStat;
    if (stat(fullPath.c_str(), &fullPathStat) == 0) {
      const std::int32_t constexpr MODE_LEN = 12;
      const std::int32_t constexpr EXTRA_POS = 10;
      std::array<char, MODE_LEN> modeStr;
      // get str mode
      strmode(fullPathStat.st_mode, modeStr.data());
      // get extended attributes
      int buf_len = listxattr(fullPath.c_str(), nullptr, 0, XATTR_NOFOLLOW);
      if (buf_len > 0) {
        modeStr[EXTRA_POS] = '@';
      }
      // get acl flags
      acl_t acl = acl_get_file(fullPath.c_str(), ACL_TYPE_EXTENDED);
      if (acl != nullptr) {
        modeStr[EXTRA_POS] = '+';
      }
      // get user name
      struct passwd *passwdStruct = getpwuid(fullPathStat.st_uid);
      if (passwdStruct == nullptr) {
        std::cerr << "Couldn't get user name" << std::endl;
        return;
      }
      // get group name
      struct group *groupStruct = getgrgid(fullPathStat.st_gid);
      if (groupStruct == nullptr) {
        std::cerr << "Couldn't get group name" << std::endl;
        return;
      }
      // get file size
      std::string size = convertSize2HumanReadable(fullPathStat.st_size);
      // get date string
      std::string time = getTimeStrOfUnixTime(fullPathStat.st_mtimespec.tv_sec);
      const std::uint32_t constexpr SIZE_WIDTH = 6;
      std::stringstream stringstream;
      stringstream << modeStr.data() << "  " << std::setw(2)
                   << fullPathStat.st_nlink << " " << passwdStruct->pw_name
                   << "  " << groupStruct->gr_name << " "
                   << std::setw(SIZE_WIDTH) << size << " " << time << " "
                   << colorOfMode(modeStr.data()) << ent->d_name << "\033[0m"
                   << std::endl;
      container.emplace_back(fullPathStat.st_mtimespec.tv_sec,
                             stringstream.str());
    }
  }
  // sort by last modify time
  std::sort(container.begin(), container.end(), std::greater<>());
  for (auto item : container) {
    std::cout << item.second;
  }
}

void listDir(const std::string &path) {
  // pre condition, path must exists
  if (!isPathExist(path)) {
    std::cerr << "Error: " << path << "not exist or inaccessible!" << std::endl;
    exit(-1);
  }
  // list directory
  doListDir(path);
}

auto main(int argc, char *argv[]) -> int {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << "<dir>" << std::endl;
    return 1;
  }
  char *path = argv[1];
  if (path != nullptr) {
    listDir(path);
  }
  return 0;
}
