#include <ocs2_core/misc/LoadStdVectorOfPair.h>

namespace ocs2 {
namespace loadData {
namespace detail {

template <>
std::string fromString<std::string>(const std::string& str) {
  return str;
}

template <>
std::size_t fromString<std::size_t>(const std::string& str) {
  return std::stoul(str);
}

template <>
int fromString<int>(const std::string& str) {
  return std::stoi(str);
}

template <>
double fromString<double>(const std::string& str) {
  return std::stod(str);
}

template <>
float fromString<float>(const std::string& str) {
  return std::stof(str);
}
}  // namespace detail
}  // namespace loadData
}  // namespace ocs2