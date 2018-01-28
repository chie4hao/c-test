#include "frame_util.h"

namespace wzd {
namespace cpptest {
int64_t FrameUtil::FindFrame(const std::vector<uint8_t>& head) {
  const size_t file_length = file_buffer_->size() - frame_length_ - 1;
  const auto file_array = *file_buffer_;
  for (size_t pos = 0; pos < file_length; ++pos) {
    for (uint8_t offset = 0; offset < 8; ++offset) {
      size_t head_pos = 0;

      for (; head_pos < head.size(); ++head_pos) {
        if (static_cast<uint8_t>((file_array[pos + head_pos] << offset) +
                                 (file_array[pos + head_pos + 1] >>
                                  (8 - offset))) != head[head_pos]) {
          break;
        }
      }
      if (head_pos == head.size()) {
        frame_head_pos_.push_back(std::pair<size_t, uint8_t>(pos, offset));
        pos += frame_length_;
      }
    }
  }

  frame_count_ = frame_head_pos_.size();
  frame_buffer_.reset(new uint8_t[frame_count_ * frame_length_]);
  auto frame_array = frame_buffer_.get();

  for (size_t pos = 0; pos < frame_count_; ++pos) {
    for (size_t data_p = 0; data_p < frame_length_; ++data_p) {
      frame_array[data_p + pos * frame_length_] =
          (file_array[frame_head_pos_[pos].first + data_p]
           << frame_head_pos_[pos].second) +
          (file_array[frame_head_pos_[pos].first + data_p + 1] >>
           (8 - frame_head_pos_[pos].second));
    }
  }
  std::cout << frame_count_ << std::endl;
  return frame_count_;
}

std::vector<std::pair<size_t, uint64_t>> FrameUtil::PseudorandomSeqErgodic(
    std::shared_ptr<uint8_t> sequence_buffer_ptr_, uint32_t series) const {
  size_t seq_length = static_cast<size_t>(pow(2, series - 3));
  auto seq = sequence_buffer_ptr_.get();
  // std::shared_ptr<uint8_t> frame_buffer_temp(new
  // uint8_t[frame_count_*frame_length_]); auto temp = frame_buffer_temp.get();
  auto frame_array = frame_buffer_.get();

  std::vector<std::pair<size_t, uint64_t>> accept_sequence{};

  const double_t frame_accept_gate =
      frame_detect_count_ * frame_length_ * frame_account_;

  for (size_t seq_pos = 0; seq_pos < seq_length; ++seq_pos) {
    for (int8_t offset = 0; offset < 8; ++offset) {
      uint64_t acceptCount = 0;
      for (size_t pos = 0; pos < frame_detect_count_; ++pos) {
        for (size_t data_p = 0; data_p < frame_length_; ++data_p) {
          uint8_t a = frame_array[data_p + pos * frame_length_] ^
                      ((seq[seq_pos + data_p] << offset) +
                       (seq[seq_pos + data_p + 1] >> (8 - offset)));

          // TODO(analyze temp)
          if (a == 0xFF || a == 0x00) ++acceptCount;
        }
      }

      // detect current pseudorandom sequence acception
      if (acceptCount > frame_accept_gate) {
        accept_sequence.push_back(
            std::pair<size_t, uint64_t>(seq_pos * 8 + offset, acceptCount));
      }
    }
  }

  std::sort(
      accept_sequence.begin(), accept_sequence.end(),
      [](const std::pair<size_t, uint64_t>& a,
         const std::pair<size_t, uint64_t>& b) { return a.second > b.second; });
  return accept_sequence;
}

void FrameUtil::SequenceScramble(std::shared_ptr<uint8_t> sequence_buffer_ptr_,
                                 uint64_t pos) const {
  auto seq = sequence_buffer_ptr_.get();
  auto frame_array = frame_buffer_.get();
  uint64_t seq_pos = pos / 8;
  int8_t offset = pos % 8;

  for (size_t pos = 0; pos < frame_count_; ++pos) {
    for (size_t data_p = 0; data_p < frame_length_; ++data_p) {
      frame_array[data_p + pos * frame_length_] ^=
          (((seq[seq_pos + data_p] << offset) +
            (seq[seq_pos + data_p + 1] >> (8 - offset))) &
           0x00FF);
    }
  }
}

void FrameUtil::ExtractRow(uint64_t begin /*bit*/,
                           size_t frame_length /*byte*/) {
  auto frame_array = frame_buffer_.get();
  size_t frame_count =
      frame_count_ - static_cast<size_t>(begin) / 8 / frame_length_;
  int8_t offset = begin % 8;

  for (uint64_t i = 0; i < frame_count * frame_length; ++i) {
    frame_array[i] = (frame_array[i / frame_length * frame_length_ +
                                  i % frame_length + begin / 8]
                      << offset) +
                     (frame_array[i / frame_length * frame_length_ +
                                  i % frame_length + begin / 8 + 1] >>
                      (8 - offset));
  }
  frame_count_ = frame_count;
  frame_length_ = frame_length;
}

void FrameUtil::Matrix(const int32_t a, const int32_t b) {
  auto frame_array = frame_buffer_.get();

  const int matrix_length = a * b / 8;
  std::shared_ptr<uint8_t> matrix_buffer(new uint8_t[matrix_length]);
  auto matrix_array = matrix_buffer.get();

  for (size_t frame = 0; frame < frame_count_ * frame_length_ / matrix_length;
       ++frame) {
    memset(matrix_array, 0, matrix_length);

    for (int32_t i = 0; i < a; ++i) {
      for (int32_t j = 0; j < b; ++j) {
        matrix_array[(j * a + i) / 8] |=
            (frame_array[frame * matrix_length + (i * b + j) / 8] &
             (1 << (7 - (i * b + j) % 8))) >>
            (7 - (i * b + j) % 8) << (7 - (j * a + i) % 8);
      }
    }
    memcpy(frame_array + frame * matrix_length, matrix_array, matrix_length);
  }
}

void FrameUtil::golayDecode() {
  auto frame_array = frame_buffer_.get();
  uint32_t golay_map[2048]{0};

  uint16_t golay_array[23]{0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020,
                           0x0040, 0x0080, 0x0100, 0x0200, 0x0400, 0x0475,
                           0x049F, 0x054B, 0x06E3, 0x01B3, 0x0366, 0x06CC,
                           0x01ED, 0x03DA, 0x07B4, 0x031D, 0x063A};
  for (int i = 0; i < 21; ++i) {
    for (int j = i + 1; j < 22; ++j) {
      for (int k = j + 1; k < 23; ++k) {
        golay_map[golay_array[i] ^ golay_array[j] ^ golay_array[k]] =
            (1 << i) + (1 << j) + (1 << k);
      }
    }
  }

  for (int i = 0; i < 22; ++i) {
    for (int j = i + 1; j < 23; ++j) {
      golay_map[golay_array[i] ^ golay_array[j]] = (1 << i) + (1 << j);
    }
  }

  for (int i = 0; i < 23; ++i) {
    golay_map[golay_array[i]] = 1 << i;
  }

  // uint8_t frame_array[6] = { 0x0, 0x18, 0xea, 0x80, 0x0c, 0x74 };
  // for (int i = 0; i < 6; ++i) {
  // std::cout << std::hex << int(frame_array[i]) << std::endl;
  //}
  // uint8_t* pos = frame_array;
  int length = frame_count_ * frame_length_ / 3;
  uint8_t buffer[4]{0, 0, 0, 0};

  bool isOffset = false;
  auto write_ptr = frame_array;

  while (length-- > 0) {
    buffer[3] = 0;
    buffer[2] = *frame_array;
    buffer[1] = *(frame_array + 1);
    buffer[0] = *(frame_array + 2);

    uint32_t a = *reinterpret_cast<uint32_t*>(buffer);

    for (int i = 12; i > 0; --i) {
      if (a >> (i + 11) == 1) {
        a = a ^ (0x0c75 << i);
      }
    }

    uint32_t golay_code = golay_map[a >> 1];
    if (isOffset) {
      *(write_ptr + 1) = (*(write_ptr + 1) & 0xF0) +
                         (((*frame_array ^ (golay_code >> 15)) >> 4) & 0x0F);
      *(write_ptr + 2) =
          (((*frame_array ^ (golay_code >> 15)) << 4) & 0xF0) +
          (((*(frame_array + 1) ^ (golay_code >> 7)) >> 4) & 0x0F);
      write_ptr += 3;
    } else {
      *write_ptr = *frame_array ^ (golay_code >> 15);
      *(write_ptr + 1) = *(frame_array + 1) ^ (golay_code >> 7);
    }

    isOffset = !isOffset;
    frame_array += 3;
  }
  frame_length_ /= 2;
}

void FrameUtil::GenarateFile(std::string output_path) const {
  auto frame_array = frame_buffer_.get();

  std::ofstream out(output_path, std::ifstream::binary);
  out.write(reinterpret_cast<char*>(&(frame_array[0])),
            frame_count_ * frame_length_);
}
}  // namespace cpptest
}  // namespace wzd
