#pragma once
#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>
#include "file_buffer.h"

namespace wzd {
namespace cpptest {
class FrameUtil : public FileBuffer<uint8_t> {
 public:
  explicit FrameUtil(size_t frame_length) : frame_length_(frame_length) {}

  int64_t FindFrame(const std::vector<uint8_t>&);
  std::vector<std::pair<size_t, uint64_t>> PseudorandomSeqErgodic(
      std::shared_ptr<uint8_t>, uint32_t) const;
  void SequenceScramble(std::shared_ptr<uint8_t>, uint64_t pos) const;
  void ExtractRow(uint64_t, size_t);
  void Matrix(const int32_t, const int32_t);
  void golayDecode();
  void GenarateFile(std::string) const;

 protected:
 private:
  static constexpr int32_t frame_detect_count_ = 2;
  static constexpr double frame_account_ = 0.05;

  std::shared_ptr<uint8_t> frame_buffer_;
  std::vector<std::pair<size_t, uint8_t>> frame_head_pos_;
  size_t frame_length_;
  size_t frame_count_;
};
}  // namespace cpptest
}  // namespace wzd
