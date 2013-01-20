/*
Copyright: 2012 LORDZOUGA <ozojiechikelu@gmail.com>
License: GPL-2+
 This package is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 .
 This package is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 .
 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>

*/
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
  * @abstract compare_drives() checks if the path pointed to by
  * a_path and b_path resides on the same drive
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

/**
  * @abstract trav_dirs() walks through the directories supplied in base_paths and
  * filters the contents with the filters in fil_dirs and returns the found files
  * and their destination directories in file_list
  *
  * @param file_list contains the found files and their supposed destination
  * @param base_paths contains the list of folders to be traversed
  * @param fil_dirs contains the filters string and the directory where the filtered
  * file should be copied to
  *
  * @note it uses the ntfw() system call for file system traversal
  */
void trav_dirs(CopyList& file_list, const QStringList& base_paths,
               const QList<FilterPair> &fil_dirs);

/**
  * @abstract start_trav() is the function called recursively by the nftw() api
  *
  * @param path points to the filename the system is currently processing
  * @param buf is a structure that contains info for the file
  * @param typeflag is a flag that determines the type of file referred to by path
  * @param ft_buf points to a structure that determines the current depth of the traversal
  *
  * @returns 0 on success else returns -1
  */
int start_trav(const char* path, const struct stat* buf,
               int typeflag, struct FTW *ft_buf);

/**
  * @abstract get_dirs() stores the value of p_name in a static variable if it is meant
  * to be copied
  *
  * @param p_name points to the path to be checked
  *
  * @returns a QStringList object
  *
  * @note if get_dirs() is called with a NULL value, it clears the static variable and
  * returns its content
  */
QStringList get_dirs(const char* p_name);


/**
  * @abstract remove_ast() removes the asterik infront of str
  *
  * @param str, string to be modified
  */
void remove_ast(QString& str);
/**
  * @abstract get_file_name() extracts the filename in path and puts it in f_name
  *
  * @param path: string to be searched for filename
  * @param f_name: variable to hold the found filename
  */
void get_file_name(const QString& path, QString &f_name);
#endif //Q_OS_LINUX

#endif // LINOPS_H
