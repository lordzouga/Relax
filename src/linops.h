#ifndef LINOPS_H
#define LINOPS_H

#include <QObject>
#include "src/relaxengine.h"

#ifdef Q_OS_LINUX
#include <unistd.h>

/**
  * @def prototype for the linux version of the copy function
  * it uses system calls for file transfer
  *
  * @param pair holds the absolute paths to the file be transferred.
  * source = pair.first
  * dest = pair.second
  *
  * @note if the file exists on a different drive it uses Qt genric file copy
  */
void lin_move(CopyPair &pair);

/**
  * @def prototype for the move_file() that makes the link() and unlink()
  * system call on linux
  *
  * @param source: holds the path of the file to be moved
  * @param dest: holds a valid path where the file is to be copied
  *
  * @note dest should not be referring to a valid file
  */
void move_file(const char* source, const char* dest);

/**
  * @abstract is_drive() checks if the path refered to by path exists on a logical partition
  *
  * @param path to be checked
  *
  * @return true if path exists on a partition else returns false if not
  */
bool is_drive(const char* path);

/**
  * @abstract compare_paths() checks if the path pointed to by
  * a_path and b_path resides on the same drive by using ths stat() system call
  *
  * @param a_path path to be compared
  * @param b_path path to be compared
  *
  * @return true if both files reside on the same drive else return false
  */
bool compare_drives(const char* a_path, const char* b_path);

/**
  * @abstract extract_drive_name() returns the drive of the file pointed to by path
  * in d_name
  *
  * @param path contains the drive to be extracted
  * @param d_name should hold the extracted string
  */
void extract_drive_name(const char* path, QString& d_name);
#endif //Q_OS_LINUX

#endif // LINOPS_H
