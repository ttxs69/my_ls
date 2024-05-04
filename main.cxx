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
std::string colorOfMode(const std::string &mode) {
  if (mode[0] == 'd') {
    return "\033[1;34m";
  }
  if (mode[0] == '-' && mode[3] == 'x') {
    return "\033[0;31m";
  }
  return "\033[0m";
}

bool isPathExist(const std::string &path) {
  struct stat st;
  return (stat(path.c_str(), &st) == 0);
}

/**
 * @brief convert file size to human readable string
 *
 * @param size file size
 * @return std::string human readable string of file size
 */
std::string convertSize2HumanReadable(off_t size) {
  std::stringstream result;
  if (size < 1024) {
    result << size << 'B';
  } else if (size > 1024LL && size < 1024LL * 1024) {
    double num = size / 1024.0;
    result << std::fixed << std::setprecision(1) << num << 'K';
  } else if (size > 1024LL * 1024 && size < 1024LL * 1024 * 1024) {
    double num = static_cast<double>(size) / (1024 * 1024);
    result << std::fixed << std::setprecision(1) << num << 'M';
  } else if (size > 1024LL * 1024 * 1024) {
    double num = static_cast<double>(size) / (1024LL * 1024 * 1024);
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
std::string getTimeStrOfUnixTime(long time) {
  std::time_t time_tmp = time;
  std::stringstream ss;
  ss << std::put_time(std::localtime(&time_tmp), "%b %d %H:%M");
  return ss.str();
}

/**
 * @brief list files of <path>, <path> must be existing
 *
 * @param path path to list
 */
void doListDir(const std::string &path) {
  DIR *dp = opendir(path.c_str());
  if (dp == nullptr) {
    std::cerr << "Could not open directory " << path << std::endl;
    exit(-2);
  }
  struct dirent *ent = nullptr;
  std::vector<std::pair<long, std::string>> container;

  while ((ent = readdir(dp)) != nullptr) {
    std::string full_path = path + PATH_SEP + ent->d_name;
    struct stat st;
    if (stat(full_path.c_str(), &st) == 0) {
      char s[256];
      // get str mode
      strmode(st.st_mode, s);
      // get extended attributes
      int buf_len = listxattr(full_path.c_str(), nullptr, 0, XATTR_NOFOLLOW);
      if (buf_len > 0) {
        s[10] = '@';
      }
      // get acl flags
      acl_t acl = acl_get_file(full_path.c_str(), ACL_TYPE_EXTENDED);
      if (acl != nullptr) {
        s[10] = '+';
      }
      // get user name
      struct passwd *pw = getpwuid(st.st_uid);
      if (pw == nullptr) {
        std::cerr << "Couldn't get user name" << std::endl;
        return;
      }
      // get group name
      struct group *gr = getgrgid(st.st_gid);
      if (gr == nullptr) {
        std::cerr << "Couldn't get group name" << std::endl;
        return;
      }
      // get file size
      std::string size = convertSize2HumanReadable(st.st_size);
      // get date string
      std::string time = getTimeStrOfUnixTime(st.st_mtimespec.tv_sec);

      std::stringstream ss;
      ss << s << "  " << std::setw(2) << st.st_nlink << " " << pw->pw_name
         << "  " << gr->gr_name << " " << std::setw(6) << size << " " << time
         << " " << colorOfMode(s) << ent->d_name << "\033[0m" << std::endl;
      container.emplace_back(st.st_mtimespec.tv_sec, ss.str());
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

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << "<dir>" << std::endl;
    return 1;
  }
  char *path = argv[1];
  if (path != NULL) {
    listDir(path);
  }
  return 0;
}
