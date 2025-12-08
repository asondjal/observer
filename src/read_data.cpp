#include "utils/read_data.hpp"

namespace observer::read_data {

ReadData::ReadData(const std::string& path) : filePath_{path} {}

std::mutex& ReadData::GetMutex() { return mtx_; }

/**
 * @brief Get the file type based on the file extension
 * @param location File location
 * @return File extension as a string
 */
std::string ReadData::GetFileType(const std::string& location) const {
  std::string fileType = std::filesystem::path(location).extension().string();
  return fileType;
}

/**
 * @brief Read the file content and return it as a string
 * @return File-content as a string
 */
std::string ReadData::ReadFile() const {
  std::lock_guard<std::mutex> lock(mtx_);  // Race Conditions are stopped
  if (!std::filesystem::exists(filePath_)) {
    throw std::runtime_error("Non existing file: " + filePath_);
  }

  std::ifstream file(filePath_);
  if (!file.is_open()) {
    throw std::runtime_error("Error when opening file: " + filePath_);
  }

  std::stringstream buffer;
  buffer << file.rdbuf();

  is_cached_ = true;
  cache_ = buffer.str();

  return buffer.str();
}

/**
 * @brief Read the text-content and return it as a string
 * @return File-content as a string
 */
std::string ReadData::ReadTxt() const {
  if (GetFileType(filePath_) != ".txt") {
    throw std::runtime_error("Wrong file type: " +
                             std::filesystem::path(filePath_).extension().string());
  }
  return ReadFile();
}

/**
 * @brief Read the csv-content and return it as a string
 * @return File-content as a string
 */
std::string ReadData::ReadCSV() const {
  if (GetFileType(filePath_) != ".csv") {
    throw std::runtime_error("Wrong file type: " +
                             std::filesystem::path(filePath_).extension().string());
  }
  return ReadFile();
}

/**
 * @brief Read the json-content and return it as a string
 * @return File-content as a string
 */
std::string ReadData::ReadJSON() const {
  if (GetFileType(filePath_) != ".json") {
    throw std::runtime_error("Wrong file type: " +
                             std::filesystem::path(filePath_).extension().string());
  }
  return ReadFile();
}

/**
 * @brief Read the pdf-content and return it as a string
 * @return File-content as a string
 */
std::string ReadData::ReadPDF() const {
  std::string content;
  std::lock_guard<std::mutex> lock(mtx_);

  if (GetFileType(filePath_) != ".pdf") {
    throw std::runtime_error("Wrong file type: " +
                             std::filesystem::path(filePath_).extension().string());
  }

  poppler::document* doc = poppler::document::load_from_file(filePath_);

  if (!doc) {
    throw std::runtime_error("pdf-file couldn't be opened " + filePath_);
  }
  for (int i = 0; i < doc->pages(); ++i) {
    poppler::page* p = doc->create_page(i);
    if (p) {
      content += p->text().to_latin1();
      delete p;
    }
  }
  delete doc;
  return content;
}

/**
 * @brief Read the html-content and return it as a string
 * @return File-content as a string
 */
std::string ReadData::ReadHTML() const {
  if (GetFileType(filePath_) != ".html") {
    throw std::runtime_error("Wrong file type: " +
                             std::filesystem::path(filePath_).extension().string());
  }
  return ReadFile();
}

/**
 * @brief Read the xml-content and return it as a string
 * @return File-content as a string
 */
std::string ReadData::ReadXML() const {
  std::lock_guard<std::mutex> lock(mtx_);
  tinyxml2::XMLDocument doc;

  if (GetFileType(filePath_) != ".xml") {
    throw std::runtime_error("Wrong file type: " +
                             std::filesystem::path(filePath_).extension().string());
  }

  if (doc.LoadFile(filePath_.c_str()) != tinyxml2::XML_SUCCESS) {
    throw std::runtime_error("Error when loading xml-file: " + filePath_);
  }

  tinyxml2::XMLPrinter printer;
  doc.Print(&printer);

  cache_ = printer.CStr();
  is_cached_ = true;
  return cache_;
}

/**
 * @brief Read the file content and return it as a string
 * @param reload default-value
 * @return File-content as a string
 */
std::string ReadData::ReadAuto(bool reload) const {
  if (!reload && is_cached_) {
    return cache_;
  }

  auto ext = std::filesystem::path(filePath_).extension().string();
  std::string content;

  if (ext == ".txt") {
    content = ReadTxt();
  } else if (ext == ".csv") {
    content = ReadCSV();
  } else if (ext == ".json") {
    content = ReadJSON();
  } else if (ext == ".pdf") {
    content = ReadPDF();
  } else if (ext == ".html") {
    content = ReadHTML();
  } else if (ext == ".xml") {
    content = ReadXML();
  }

  else {
    throw std::runtime_error("Unknown file-format: " + ext);
  }

  cache_ = content;
  is_cached_ = true;
  return cache_;
}

/**
 * @brief Saved data in the buffer
 * @param bool print, `true` per Default, else `false`
 */
std::vector<std::string> ReadData::DisplayData() const {
  std::lock_guard<std::mutex> lock(mtx_);

  if (!is_cached_) {
    throw std::runtime_error("The cache is empty. Please load data into the cache!");
  }

  std::vector<std::string> lines;
  std::istringstream stream(cache_);
  std::string line;
  int line_number = 1;

  while (std::getline(stream, line)) {
    lines.push_back(line);
    std::cout << "[Line " << line_number << ": \"" << line << "\"]" << std::endl;
    ++line_number;
  }

  return lines;
}

/**
 * @brief Memory management of the cache
 */
void ReadData::ObserveCache() const {
  std::lock_guard<std::mutex> lock(mtx_);
  cache_.clear();
  is_cached_ = false;
}

}  // namespace observer::read_data
