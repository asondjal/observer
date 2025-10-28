#ifndef INC_UTILS_READ_DATA_HPP_
#define INC_UTILS_READ_DATA_HPP_

#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#include <tinyxml2.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>

namespace observer::read_data {
class ReadData {
 public:
  explicit ReadData(const std::string& path);
  std::string GetFileType(const std::string& location) const;
  std::string ReadFile() const;
  std::string ReadTxt() const;
  std::string ReadCSV() const;
  std::string ReadJSON() const;
  std::string ReadPDF() const;
  std::string ReadAuto(bool reload = false) const;
  std::string ReadHTML() const;
  std::string ReadXML() const;
  std::vector<std::string> DisplayData() const;
  void ObserveCache() const;

 private:
  std::string filePath_;
  mutable std::mutex mtx_;
  std::mutex& GetMutex();
  mutable std::string cache_;
  mutable bool is_cached_ = false;
};
}  // namespace observer::read_data

#endif  // INC_UTILS_READ_DATA_HPP_
