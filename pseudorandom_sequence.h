#pragma once
#include <memory>
#include <cmath>

namespace wzd {
	namespace cpptest {

		class PseudoRandomSequence {
		public:
			explicit PseudoRandomSequence(const int32_t &series)
				: series_(series),
				sequence_buffer_ptr_(
					new uint8_t[static_cast<size_t>(pow(2, series - 3)) + extend_size_],
					std::default_delete<uint8_t>()) {}
			template <typename T>
			inline void SetSequenceHeader(const std::initializer_list<T>&);
			std::shared_ptr<uint8_t> GenarateSequence(uint32_t);

			// No copying allow
			PseudoRandomSequence(const PseudoRandomSequence&) = delete;
			void operator=(PseudoRandomSequence const&) = delete;

		private:
			// Extra size to prevent sequence_buffer_ptr_ overflow
			static constexpr size_t extend_size_ = 2100;
			// The pseudorandomsequence series
			int32_t series_;
			std::shared_ptr<uint8_t> sequence_buffer_ptr_;
		};

		template<typename T>
		inline void PseudoRandomSequence::SetSequenceHeader(const std::initializer_list<T>& initial_sequence)
		{
			std::shared_ptr<uint8_t>::element_type* sequence_buffer_array = sequence_buffer_ptr_.get();

			const auto begin_ptr = initial_sequence.begin();
			for (auto current_ptr = begin_ptr; current_ptr != initial_sequence.end(); ++current_ptr) {
				memcpy(sequence_buffer_array + (current_ptr - begin_ptr) * sizeof(T), current_ptr, sizeof(T));
			}
		}

	}  // namespace cpptest
}  // namespace wzd
