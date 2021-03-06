// addon.cc
#include <node.h>

#include "frame_util.h"
#include "pseudorandom_sequence.h"

namespace demo {

using v8::Function;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Null;
using v8::Object;
using v8::String;
using v8::Value;

void RunCallback(const FunctionCallbackInfo<Value>& args) {
  clock_t start, finish;
  start = clock();

  wzd::cpptest::FrameUtil a(216);
  a.load("C:\\github\\cpptest\\data\\111.dat");
  a.FindFrame(std::vector<uint8_t>{0xB1, 0x98, 0x92, 0x07});
  wzd::cpptest::PseudoRandomSequence d(23);
  d.SetSequenceHeader({0xFF, 0xFF, 0xFE});
  auto f = d.GenarateSequence(18);

  /*auto k = a.PseudorandomSeqErgodic(f, 23);
        auto l = f.get();

        for (auto& a : k) {
        for (int b = 0; b < 3; ++b) {
        std::cout << std::hex
        << (((l[a.first / 8 + b] << (a.first % 8)) + (l[a.first / 8 + b + 1] >>
     (8 - a.first % 8))) & 0x00ff)
        << " ";
        }
        std::cout << std::dec << " position: " << a.first << "   frequency:  "
     << a.second << std::endl;
        }
        */

  a.SequenceScramble(f, 8388479);
  a.ExtractRow(192, 192);
  a.Matrix(24, 16);
  a.golayDecode();
  a.GenarateFile("C:\\github\\cpptest\\data\\112.dat");

  finish = clock();
  std::cout << std::dec << static_cast<double>(finish - start) / CLOCKS_PER_SEC
            << "s" << std::endl;

  Isolate* isolate = args.GetIsolate();
  Local<Function> cb = Local<Function>::Cast(args[0]);
  const unsigned argc = 1;
  Local<Value> argv[argc] = {String::NewFromUtf8(isolate, "hello world")};
  cb->Call(Null(isolate), argc, argv);
}

void Init(Local<Object> exports, Local<Object> module) {
  NODE_SET_METHOD(module, "exports", RunCallback);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Init)

}  // namespace demo