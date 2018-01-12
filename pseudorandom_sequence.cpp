#include "stdafx.h"
#include "pseudorandom_sequence.h"

namespace wzd {
	namespace cpptest {

		std::shared_ptr<uint8_t> PseudoRandomSequence::GenarateSequence(uint32_t low)
		{
			int32_t begin = static_cast<int32_t>(ceil(static_cast<double_t>(series_) / static_cast<double_t>(8)));
			int32_t cbegin = begin * 8 - series_, cend = (begin * 8 - low) % 8, cend_offest = (begin * 8 - low) / 8;
			auto seq = sequence_buffer_ptr_.get();
			int64_t size = static_cast<size_t>(pow(2, series_ - 3)) + extend_size_ - 100;
			for (int64_t b = 0; b < size; ++b) {
				seq[b + begin] = ((seq[b] << cbegin) + (seq[b + 1] >> (8 - cbegin)))
					^ ((seq[b + cend_offest] << cend) + (seq[b + cend_offest + 1] >> (8 - cend)));
			}

			return sequence_buffer_ptr_;
		}
	}  // namespace cpptest
}  // namespace wzd
