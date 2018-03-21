#include "SharedContext.h"

SharedContext::PathsMap SharedContext::m_paths_map;

SharedContext::SharedContext(const std::string & paths_file) : m_wind(nullptr), m_eventManager(nullptr) {
  LoadConfigFiles(paths_file);
}


int SharedContext::getStageNum(int num) const {
  int cnt = abs(num % (int)map_list.size());
  if (cnt >= map_list.size()) {
    std::cout << __FUNCTION__ << std::endl;
    std::cout << " owerflow : " << std::endl;
    getchar();
    terminate();
  }
  return cnt;
}


const std::string & SharedContext::getFilePath(const std::string& name) {
  try {
    return m_paths_map.at(name);
  }
  catch (...) {
    std::cout << __FUNCTION__  
      << "\n requested file  does not exist : " << name << std::endl;
    getchar();
    terminate();
  }
}


void SharedContext::LoadConfigFiles(const std::string & filename) {
  using namespace std;

  ifstream ifile(filename);
  if (!ifile.is_open()) {
    cout << __FUNCTION__ << "\t Can't\t open :" << filename << endl;
    getchar();
    terminate();
  }
  string line, file, path;

  while (getline(ifile, line)) {
    if (line[0] == '#') continue;
    istringstream is(line);
    if (is >> file >> path) {
      m_paths_map.emplace(file, path);
    }
    else {
      cout << __FUNCTION__ << " : can't read line : "  << line;
      getchar();
      terminate();
    }
  }
  for (const auto& item : m_paths_map) {
    cout << "name : " << item.first
      << ";\t\t file : " << item.second << endl;
  }
  if (m_paths_map.empty()) {
    cout << __FUNCTION__ << endl;
    cout << "config list is empty : " << endl;
    getchar();
    terminate();
  }
}

void SharedContext::LoadStageList(const std::string & filename) {
  using namespace std;
  
  ifstream ifile(filename);
  if (!ifile.is_open()) {
    cout << __FUNCTION__ << "\t Can\t open :" << filename << endl;
    getchar();
    terminate();
  }
  string line, stage_name, stage_file;

  while (getline(ifile, line)) {
    if (line[0] == '#') continue;
    istringstream is(line);
    if (is >> stage_name >> stage_file) {
      map_list.push_back(make_pair(
        std::move(stage_name), std::move(stage_file)));
    }
    else {
      cout << __FUNCTION__ << " : can't read line : "
        << line;
      getchar();
      terminate();
    }
  }
  for (const auto& item : map_list) {
    cout << "name\t: " << item.first
      << "; file\t: " << item.second << endl;
  }
  if (map_list.empty()) {
    cout << __FUNCTION__ << endl;
    cout << "stage list is empty : "<< endl;
    getchar();
    terminate();
  }
}


