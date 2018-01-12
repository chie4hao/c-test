#include "stdafx.h"
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
						if (static_cast<uint8_t>((file_array[pos + head_pos] << offset)
							+ (file_array[pos + head_pos + 1] >> (8 - offset))) != head[head_pos]) {
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
			frame_buffer_.reset(new uint8_t[frame_count_*frame_length_]);
			auto frame_array = frame_buffer_.get();

			for (size_t pos = 0; pos < frame_count_; ++pos) {
				for (size_t data_p = 0; data_p < frame_length_; ++data_p) {
					frame_array[data_p + pos*frame_length_] = (file_array[frame_head_pos_[pos].first + data_p] << frame_head_pos_[pos].second)
						+ (file_array[frame_head_pos_[pos].first + data_p + 1] >> (8 - frame_head_pos_[pos].second));
				}
			}
			std::cout << frame_count_ << std::endl;
			return frame_count_;
		}

		std::vector<std::pair<size_t, uint64_t>>
			FrameUtil::PseudorandomSeqErgodic(std::shared_ptr<uint8_t> sequence_buffer_ptr_, uint32_t series) const
		{
			size_t seq_length = static_cast<size_t>(pow(2, series - 3));
			auto seq = sequence_buffer_ptr_.get();
			// std::shared_ptr<uint8_t> frame_buffer_temp(new uint8_t[frame_count_*frame_length_]);
			// auto temp = frame_buffer_temp.get();
			auto frame_array = frame_buffer_.get();

			std::vector<std::pair<size_t, uint64_t>> accept_sequence{};

			const double_t frame_accept_gate = frame_detect_count_ * frame_length_ * frame_account_;

			for (size_t seq_pos = 0; seq_pos < seq_length; ++seq_pos) {
				for (int8_t offset = 0; offset < 8; ++offset) {

					uint64_t acceptCount = 0;
					for (size_t pos = 0; pos < frame_detect_count_; ++pos) {
						for (size_t data_p = 0; data_p < frame_length_; ++data_p) {
							uint8_t a = /* temp[data_p + pos*frame_length_] = */ frame_array[data_p + pos*frame_length_]
								^ ((seq[seq_pos + data_p] << offset) + (seq[seq_pos + data_p + 1] >> (8 - offset)));

							// TODO(analyze temp)
							if (a == 0xFF || a == 0x00) ++acceptCount;
						}
					}

					// detect current pseudorandom sequence acception
					if (acceptCount > frame_accept_gate) {
						accept_sequence.push_back(std::pair<size_t, uint64_t>(seq_pos * 8 + offset, acceptCount));
					}
				}
			}

			std::sort(accept_sequence.begin(), accept_sequence.end(),
				[](const std::pair<size_t, uint64_t> &a, const std::pair<size_t, uint64_t> &b) {
				return a.second > b.second;
			});
			return accept_sequence;
		}

		void FrameUtil::SequenceScramble(std::shared_ptr<uint8_t> sequence_buffer_ptr_, uint64_t pos, std::wstring output_path) const {
			auto seq = sequence_buffer_ptr_.get();
			auto frame_array = frame_buffer_.get();
			uint64_t seq_pos = pos / 8;
			int8_t offset = pos % 8;

			for (size_t pos = 0; pos < frame_count_; ++pos) {
				for (size_t data_p = 0; data_p < frame_length_; ++data_p) {
					frame_array[data_p + pos*frame_length_]
						^= (((seq[seq_pos + data_p] << offset) + (seq[seq_pos + data_p + 1] >> (8 - offset))) & 0x00FF);
				}
			}

			std::ofstream out(output_path, std::ifstream::binary);
			out.write(reinterpret_cast<char*>(&(frame_array[0])), frame_count_*frame_length_);
		}
	}
}
