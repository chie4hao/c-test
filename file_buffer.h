#pragma once
#include <vector>
#include <memory>
#include <fstream>
#include <locale>

namespace wzd {
	namespace cpptest {
		template <typename T>
		class FileBuffer {
		public:
			explicit FileBuffer() {}

			bool load(const std::wstring& filepath);

			// No copying allow
			FileBuffer(const FileBuffer&) = delete;
			void operator=(FileBuffer const&) = delete;

		protected:
			std::shared_ptr<std::vector<T>> file_buffer_;
		private:
		};

		template<typename T>
		inline bool FileBuffer<T>::load(const std::wstring& filepath)
		{
			std::ifstream file(filepath, std::ios::binary);

			// check the file exists
			if (!file.good()) {
				std::cout << "ERROR: File doesn't exist or otherwise can't load file:" << std::endl;
				std::wcout.imbue(std::locale("chs"));
				std::wcout << filepath << std::endl;
				return false;
			}

			file.seekg(0, std::ios::end);
			int64_t length = file.tellg();
			file.seekg(0, std::ios::beg);
			file_buffer_.reset(new std::vector<T>(length));
			file.read(reinterpret_cast<char*>(&((*file_buffer_)[0])), length);

			return true;
		}
	}
}