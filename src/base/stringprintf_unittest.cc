// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/basictypes.h"
#include "base/stringprintf.h"
#include "testing/gtest/include/gtest/gtest.h"
#if defined(OS_STARBOARD)
#include "starboard/string.h"
#endif

namespace base {

namespace {

// A helper for the StringAppendV test that follows.
//
// Just forwards its args to StringAppendV.
static void StringAppendVTestHelper(std::string* out, const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  StringAppendV(out, format, ap);
  va_end(ap);
}

#if defined(OS_STARBOARD)
// Same deal for SbStringFormat
static void SbStringFormatHelper(char *out_buffer,
                                 size_t buffer_size,
                                 const char *format,
                                 ...) {
  va_list ap;
  va_start(ap, format);
  SbStringFormat(out_buffer, buffer_size, format, ap);
  va_end(ap);
}
#endif

}  // namespace

TEST(StringPrintfTest, StringPrintfEmpty) {
  EXPECT_EQ("", StringPrintf("%s", ""));
}

TEST(StringPrintfTest, StringPrintfMisc) {
  EXPECT_EQ("123hello w", StringPrintf("%3d%2s %1c", 123, "hello", 'w'));
#if !defined(OS_ANDROID) && !defined(__LB_ANDROID__)
  EXPECT_EQ(L"123hello w", StringPrintf(L"%3d%2ls %1lc", 123, L"hello", 'w'));
#endif
}

TEST(StringPrintfTest, StringAppendfEmptyString) {
  std::string value("Hello");
  StringAppendF(&value, "%s", "");
  EXPECT_EQ("Hello", value);

#if !defined(OS_ANDROID) && !defined(__LB_ANDROID__)
  std::wstring valuew(L"Hello");
  StringAppendF(&valuew, L"%ls", L"");
  EXPECT_EQ(L"Hello", valuew);
#endif
}

TEST(StringPrintfTest, StringAppendfString) {
  std::string value("Hello");
  StringAppendF(&value, " %s", "World");
  EXPECT_EQ("Hello World", value);

#if !defined(OS_ANDROID) && !defined(__LB_ANDROID__)
  std::wstring valuew(L"Hello");
  StringAppendF(&valuew, L" %ls", L"World");
  EXPECT_EQ(L"Hello World", valuew);
#endif
}

TEST(StringPrintfTest, StringAppendfInt) {
  std::string value("Hello");
  StringAppendF(&value, " %d", 123);
  EXPECT_EQ("Hello 123", value);

#if !defined(OS_ANDROID) && !defined(__LB_ANDROID__)
  std::wstring valuew(L"Hello");
  StringAppendF(&valuew, L" %d", 123);
  EXPECT_EQ(L"Hello 123", valuew);
#endif
}

// Make sure that lengths exactly around the initial buffer size are handled
// correctly.
TEST(StringPrintfTest, StringPrintfBounds) {
  const int kSrcLen = 1026;
  char src[kSrcLen];
  for (size_t i = 0; i < arraysize(src); i++)
    src[i] = 'A';

  wchar_t srcw[kSrcLen];
  for (size_t i = 0; i < arraysize(srcw); i++)
    srcw[i] = 'A';

  for (int i = 1; i < 3; i++) {
    src[kSrcLen - i] = 0;
    std::string out;
    SStringPrintf(&out, "%s", src);
    EXPECT_STREQ(src, out.c_str());

#if !defined(OS_ANDROID) && !defined(__LB_ANDROID__)
    srcw[kSrcLen - i] = 0;
    std::wstring outw;
    SStringPrintf(&outw, L"%ls", srcw);
    EXPECT_STREQ(srcw, outw.c_str());
#endif
  }
}

// Test very large sprintfs that will cause the buffer to grow.
TEST(StringPrintfTest, Grow) {
  char src[1026];
  for (size_t i = 0; i < arraysize(src); i++)
    src[i] = 'A';
  src[1025] = 0;

  const char* fmt = "%sB%sB%sB%sB%sB%sB%s";

  std::string out;
  SStringPrintf(&out, fmt, src, src, src, src, src, src, src);

  const int kRefSize = 320000;
  char* ref = new char[kRefSize];
#if defined(OS_WIN)
  sprintf_s(ref, kRefSize, fmt, src, src, src, src, src, src, src);
#elif defined(OS_POSIX)
  snprintf(ref, kRefSize, fmt, src, src, src, src, src, src, src);
#elif defined(OS_STARBOARD)
  SbStringFormatHelper(ref, kRefSize, fmt, src, src, src, src, src, src, src);
#endif

  EXPECT_STREQ(ref, out.c_str());
  delete[] ref;
}

TEST(StringPrintfTest, StringAppendV) {
  std::string out;
  StringAppendVTestHelper(&out, "%d foo %s", 1, "bar");
  EXPECT_EQ("1 foo bar", out);
}

// Test the boundary condition for the size of the string_util's
// internal buffer.
TEST(StringPrintfTest, GrowBoundary) {
  const int string_util_buf_len = 1024;
  // Our buffer should be one larger than the size of StringAppendVT's stack
  // buffer.
  const int buf_len = string_util_buf_len + 1;
  char src[buf_len + 1];  // Need extra one for NULL-terminator.
  for (int i = 0; i < buf_len; ++i)
    src[i] = 'a';
  src[buf_len] = 0;

  std::string out;
  SStringPrintf(&out, "%s", src);

  EXPECT_STREQ(src, out.c_str());
}

// lbshell platforms do not support positional parameters,
// and lbshell does not use the few parts of chromium that
// leverage positional parameter support in the OS.
#if !defined(__LB_SHELL__) && !defined(OS_STARBOARD)
// Test that the positional parameters work.
TEST(StringPrintfTest, PositionalParameters) {
  std::string out;
  SStringPrintf(&out, "%1$s %1$s", "test");
  EXPECT_STREQ("test test", out.c_str());

#if defined(OS_WIN)
  std::wstring wout;
  SStringPrintf(&wout, L"%1$ls %1$ls", L"test");
  EXPECT_STREQ(L"test test", wout.c_str());
#endif
}
#endif

}  // namespace base
