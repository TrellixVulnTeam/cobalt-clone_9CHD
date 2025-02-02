// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/file_util.h"

#if defined(OS_WIN)
#include <io.h>
#endif
#include <stdio.h>

#include <fstream>

#include "base/file_path.h"
#include "base/logging.h"
#include "base/stringprintf.h"
#include "base/string_piece.h"
#include "base/string_util.h"
#include "base/utf_string_conversions.h"

namespace {

const FilePath::CharType kExtensionSeparator = FILE_PATH_LITERAL('.');

// The maximum number of 'uniquified' files we will try to create.
// This is used when the filename we're trying to download is already in use,
// so we create a new unique filename by appending " (nnn)" before the
// extension, where 1 <= nnn <= kMaxUniqueFiles.
// Also used by code that cleans up said files.
static const int kMaxUniqueFiles = 100;

}  // namespace

namespace file_util {

bool g_bug108724_debug = false;

bool EndsWithSeparator(const FilePath& path) {
  FilePath::StringType value = path.value();
  if (value.empty())
    return false;

  return FilePath::IsSeparator(value[value.size() - 1]);
}

bool EnsureEndsWithSeparator(FilePath* path) {
  if (!DirectoryExists(*path))
    return false;

  if (EndsWithSeparator(*path))
    return true;

  FilePath::StringType& path_str =
      const_cast<FilePath::StringType&>(path->value());
  path_str.append(&FilePath::kSeparators[0], 1);

  return true;
}

void InsertBeforeExtension(FilePath* path, const FilePath::StringType& suffix) {
  FilePath::StringType& value =
      const_cast<FilePath::StringType&>(path->value());

  const FilePath::StringType::size_type last_dot =
      value.rfind(kExtensionSeparator);
  const FilePath::StringType::size_type last_separator =
      value.find_last_of(FilePath::StringType(FilePath::kSeparators));

  if (last_dot == FilePath::StringType::npos ||
      (last_separator != std::wstring::npos && last_dot < last_separator)) {
    // The path looks something like "C:\pics.old\jojo" or "C:\pics\jojo".
    // We should just append the suffix to the entire path.
    value.append(suffix);
    return;
  }

  value.insert(last_dot, suffix);
}

bool ContentsEqual(const FilePath& filename1, const FilePath& filename2) {
  // We open the file in binary format even if they are text files because
  // we are just comparing that bytes are exactly same in both files and not
  // doing anything smart with text formatting.
  std::ifstream file1(filename1.value().c_str(),
                      std::ios::in | std::ios::binary);
  std::ifstream file2(filename2.value().c_str(),
                      std::ios::in | std::ios::binary);

  // Even if both files aren't openable (and thus, in some sense, "equal"),
  // any unusable file yields a result of "false".
  if (!file1.is_open() || !file2.is_open())
    return false;

  const int BUFFER_SIZE = 2056;
  char buffer1[BUFFER_SIZE], buffer2[BUFFER_SIZE];
  do {
    file1.read(buffer1, BUFFER_SIZE);
    file2.read(buffer2, BUFFER_SIZE);

    if ((file1.eof() != file2.eof()) ||
        (file1.gcount() != file2.gcount()) ||
        (memcmp(buffer1, buffer2, file1.gcount()))) {
      file1.close();
      file2.close();
      return false;
    }
  } while (!file1.eof() || !file2.eof());

  file1.close();
  file2.close();
  return true;
}

#if !defined(__LB_SHELL__) && !defined(OS_STARBOARD)
bool TextContentsEqual(const FilePath& filename1, const FilePath& filename2) {
  std::ifstream file1(filename1.value().c_str(), std::ios::in);
  std::ifstream file2(filename2.value().c_str(), std::ios::in);

  // Even if both files aren't openable (and thus, in some sense, "equal"),
  // any unusable file yields a result of "false".
  if (!file1.is_open() || !file2.is_open())
    return false;

  do {
    std::string line1, line2;
    getline(file1, line1);
    getline(file2, line2);

    // Check for mismatched EOF states, or any error state.
    if ((file1.eof() != file2.eof()) ||
        file1.bad() || file2.bad()) {
      return false;
    }

    // Trim all '\r' and '\n' characters from the end of the line.
    std::string::size_type end1 = line1.find_last_not_of("\r\n");
    if (end1 == std::string::npos)
      line1.clear();
    else if (end1 + 1 < line1.length())
      line1.erase(end1 + 1);

    std::string::size_type end2 = line2.find_last_not_of("\r\n");
    if (end2 == std::string::npos)
      line2.clear();
    else if (end2 + 1 < line2.length())
      line2.erase(end2 + 1);

    if (line1 != line2)
      return false;
  } while (!file1.eof() || !file2.eof());

  return true;
}
#endif

bool ReadFileToString(const FilePath& path, std::string* contents) {
  if (path.ReferencesParent())
    return false;

#if defined(COBALT)
  // Use a smaller buffer so we don't run out of stack space.
  const size_t kReadBufferSize = 1 << 12;
#else
  const size_t kReadBufferSize = 1 << 16;
#endif

#if defined(OS_STARBOARD)
  base::PlatformFile file = base::CreatePlatformFile(
      path, base::PLATFORM_FILE_OPEN | base::PLATFORM_FILE_READ, NULL, NULL);
  if (file == base::kInvalidPlatformFileValue) {
    return false;
  }

  char buf[kReadBufferSize];
  size_t len;
  while ((len = base::ReadPlatformFileAtCurrentPos(file, buf, sizeof(buf))) >
         0) {
    if (contents) {
      contents->append(buf, len);
    }
  }
  base::ClosePlatformFile(file);

  return true;
#else
  FILE* file = OpenFile(path, "rb");
  if (!file) {
    return false;
  }

  char buf[kReadBufferSize];
  size_t len;
  while ((len = fread(buf, 1, sizeof(buf), file)) > 0) {
    if (contents)
      contents->append(buf, len);
  }
  CloseFile(file);

  return true;
#endif
}

bool IsDirectoryEmpty(const FilePath& dir_path) {
  FileEnumerator files(dir_path, false,
      FileEnumerator::FILES | FileEnumerator::DIRECTORIES);
  if (files.Next().value().empty())
    return true;
  return false;
}

#if !defined(OS_STARBOARD)
FILE* CreateAndOpenTemporaryFile(FilePath* path) {
  FilePath directory;
  if (!GetTempDir(&directory))
    return NULL;

  return CreateAndOpenTemporaryFileInDir(directory, path);
}
#endif  // !defined(OS_STARBOARD)

bool GetFileSize(const FilePath& file_path, int64* file_size) {
  base::PlatformFileInfo info;
  if (!GetFileInfo(file_path, &info))
    return false;
  *file_size = info.size;
  return true;
}

bool IsDot(const FilePath& path) {
  return FILE_PATH_LITERAL(".") == path.BaseName().value();
}

bool IsDotDot(const FilePath& path) {
  return FILE_PATH_LITERAL("..") == path.BaseName().value();
}

#if !defined(OS_STARBOARD)
bool TouchFile(const FilePath& path,
               const base::Time& last_accessed,
               const base::Time& last_modified) {
  int flags = base::PLATFORM_FILE_OPEN | base::PLATFORM_FILE_WRITE_ATTRIBUTES;

#if defined(COBALT_WIN)
  // Ensure that this makes it through to the CRT.
  flags |= base::PLATFORM_FILE_WRITE;
#endif // COBALT_WIN

#if defined(OS_WIN)
  // On Windows, FILE_FLAG_BACKUP_SEMANTICS is needed to open a directory.
  if (DirectoryExists(path))
    flags |= base::PLATFORM_FILE_BACKUP_SEMANTICS;
#endif  // OS_WIN

  const base::PlatformFile file =
      base::CreatePlatformFile(path, flags, NULL, NULL);
  if (file != base::kInvalidPlatformFileValue) {
    bool result = base::TouchPlatformFile(file, last_accessed, last_modified);
    base::ClosePlatformFile(file);
    return result;
  }

  return false;
}

bool SetLastModifiedTime(const FilePath& path,
                         const base::Time& last_modified) {
  return TouchFile(path, last_modified, last_modified);
}

bool CloseFile(FILE* file) {
  if (file == NULL)
    return true;
  return fclose(file) == 0;
}

bool TruncateFile(FILE* file) {
  if (file == NULL)
    return false;
  long current_offset = ftell(file);
  if (current_offset == -1)
    return false;
#if defined(OS_WIN)
  int fd = _fileno(file);
  if (_chsize(fd, current_offset) != 0)
    return false;
#else
  int fd = fileno(file);
  if (ftruncate(fd, current_offset) != 0)
    return false;
#endif
  return true;
}
#endif  // !defined(OS_STARBOARD)

int GetUniquePathNumber(
    const FilePath& path,
    const FilePath::StringType& suffix) {
  bool have_suffix = !suffix.empty();
  if (!PathExists(path) &&
      (!have_suffix || !PathExists(FilePath(path.value() + suffix)))) {
    return 0;
  }

  FilePath new_path;
  for (int count = 1; count <= kMaxUniqueFiles; ++count) {
    new_path = path.InsertBeforeExtensionASCII(StringPrintf(" (%d)", count));
    if (!PathExists(new_path) &&
        (!have_suffix || !PathExists(FilePath(new_path.value() + suffix)))) {
      return count;
    }
  }

  return -1;
}

bool ContainsPath(const FilePath &parent, const FilePath& child) {
  FilePath abs_parent = FilePath(parent);
  FilePath abs_child = FilePath(child);

  if (!file_util::AbsolutePath(&abs_parent) ||
      !file_util::AbsolutePath(&abs_child))
    return false;

#if defined(OS_WIN)
  // file_util::AbsolutePath() does not flatten case on Windows, so we must do
  // a case-insensitive compare.
  if (!StartsWith(abs_child.value(), abs_parent.value(), false))
#else
  if (!StartsWithASCII(abs_child.value(), abs_parent.value(), true))
#endif
    return false;

  // file_util::AbsolutePath() normalizes '/' to '\' on Windows, so we only need
  // to check kSeparators[0].
  if (abs_child.value().length() <= abs_parent.value().length() ||
      abs_child.value()[abs_parent.value().length()] !=
          FilePath::kSeparators[0])
    return false;

  return true;
}

int64 ComputeDirectorySize(const FilePath& root_path) {
  int64 running_size = 0;
  FileEnumerator file_iter(root_path, true, FileEnumerator::FILES);
  for (FilePath current = file_iter.Next(); !current.empty();
       current = file_iter.Next()) {
    FileEnumerator::FindInfo info;
    file_iter.GetFindInfo(&info);
#if defined(OS_WIN)
    LARGE_INTEGER li = { info.nFileSizeLow, info.nFileSizeHigh };
    running_size += li.QuadPart;
#elif defined(OS_STARBOARD)
    running_size += info.sb_info.size;
#else
    running_size += info.stat.st_size;
#endif
  }
  return running_size;
}

int64 ComputeFilesSize(const FilePath& directory,
                       const FilePath::StringType& pattern) {
  int64 running_size = 0;
  FileEnumerator file_iter(directory, false, FileEnumerator::FILES, pattern);
  for (FilePath current = file_iter.Next(); !current.empty();
       current = file_iter.Next()) {
    FileEnumerator::FindInfo info;
    file_iter.GetFindInfo(&info);
#if defined(OS_WIN)
    LARGE_INTEGER li = { info.nFileSizeLow, info.nFileSizeHigh };
    running_size += li.QuadPart;
#elif defined(OS_STARBOARD)
    running_size += info.sb_info.size;
#else
    running_size += info.stat.st_size;
#endif
  }
  return running_size;
}

#if !defined(__LB_SHELL__) && !defined(OS_STARBOARD)
///////////////////////////////////////////////
// MemoryMappedFile

MemoryMappedFile::~MemoryMappedFile() {
  CloseHandles();
}

bool MemoryMappedFile::Initialize(const FilePath& file_name) {
  if (IsValid())
    return false;

  if (!MapFileToMemory(file_name)) {
    CloseHandles();
    return false;
  }

  return true;
}

bool MemoryMappedFile::Initialize(base::PlatformFile file) {
  if (IsValid())
    return false;

  file_ = file;

  if (!MapFileToMemoryInternal()) {
    CloseHandles();
    return false;
  }

  return true;
}

bool MemoryMappedFile::IsValid() const {
  return data_ != NULL;
}

bool MemoryMappedFile::MapFileToMemory(const FilePath& file_name) {
  file_ = base::CreatePlatformFile(
      file_name, base::PLATFORM_FILE_OPEN | base::PLATFORM_FILE_READ,
      NULL, NULL);

  if (file_ == base::kInvalidPlatformFileValue) {
    DLOG(ERROR) << "Couldn't open " << file_name.value();
    return false;
  }

  return MapFileToMemoryInternal();
}
#endif

///////////////////////////////////////////////
// FileEnumerator
//
// Note: the main logic is in file_util_<platform>.cc

bool FileEnumerator::ShouldSkip(const FilePath& path) {
  FilePath::StringType basename = path.BaseName().value();
  return IsDot(path) || (IsDotDot(path) && !(INCLUDE_DOT_DOT & file_type_));
}

}  // namespace
