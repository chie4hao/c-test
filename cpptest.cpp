// cpptest.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "pseudorandom_sequence.h"
#include "frame_util.h"

int main()
{
	clock_t start, finish;
	start = clock();

	wzd::cpptest::FrameUtil a(216);
	a.load(L"D:\\�ҵ��ĵ�\\My RTX Files\\����Ԫ\\���¸�\\111.dat");
	a.FindFrame(std::vector<uint8_t>{0xB1, 0x98, 0x92, 0x07});
	wzd::cpptest::PseudoRandomSequence d(23);
	d.SetSequenceHeader({ 0xFF, 0xFF, 0xFE });
	auto f = d.GenarateSequence(18);

	auto k = a.PseudorandomSeqErgodic(f, 23);
	auto l = f.get();

	for (auto& a : k) {
		for (int b = 0; b < 3; ++b) {
			std::cout << std::hex
				<< (((l[a.first / 8 + b] << (a.first % 8)) + (l[a.first / 8 + b + 1] >> (8 - a.first % 8))) & 0x00ff)
				<< " ";
		}
		std::cout << std::dec << " position: " << a.first << "   frequency:  " << a.second << std::endl;
	}

	a.SequenceScramble(f, 8388479, L"D:\\�ҵ��ĵ�\\My RTX Files\\����Ԫ\\���¸�\\112.dat");

	finish = clock();
	std::cout << std::dec << static_cast<double>(finish - start) / CLOCKS_PER_SEC << "s" << std::endl;
	return 0;
}
