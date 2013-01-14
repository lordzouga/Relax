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
#include "linops.h"

#ifdef Q_OS_LINUX

#include <sys/stat.h>
#include <ftw.h>
#include <sys/types.h>

/** definition of lin_move()*/
void lin_move(CopyPair &pair){
    /**
      * @pre pair contains strings to valid file paths
      * source = pair.first, dest = pair.second
      *
      * @post the file path contained in pair.second now refers to a
      * valid file
      */

    QString old_path = pair.first;
    QString new_path = pair.second;

    std::string s = old_path.toStdString();
    std::string o = new_path.toStdString();

    if(is_drive(s.c_str()) || is_drive(o.c_str())){
        if(compare_drives(s.c_str(), o.c_str()))
            move_file(s.c_str(), o.c_str());
        else
            copyFiles(pair);
    }
    else
        move_file(s.c_str(), o.c_str());
}

void move_file(const char *source, const char *dest){
    /**
      * @pre source points to a valid file name. dest doesn't exist
      *
      * @post source does not exist. dest points to a valid file name
      */
    if(link(source, dest) == 0)
        unlink(source);
}

bool is_drive(const char *path){
    /**
      * @pre path contains a valid file path
      *
      * @post path is verified
      */
    QString s(path);

    if(s.contains(QString("media")))
        return true;
    else
        return false;
}

bool compare_drives(const char *a_path, const char *b_path){
    /**
      * @pre a_path and b_path contains drives to be compared
      *
      * @post a_path and b_path have been compared. returns true if equal
      * else returns false
      */

    QString a_str, b_str;

    extract_drive_name(a_path, a_str);
    extract_drive_name(b_path, b_str);

    if(a_str == b_str)
        return true;
    else
        return false;
}

void extract_drive_name(const char *path, QString &d_name){
    /**
      * @pre path contains a file path, d_name is empty
      *
      * @post d_name contains the drive of path
      */

    QString s(path);

    QStringList a_list = s.split('/');

    d_name = a_list[2]; //since the drive name is immediately after "media"

}

#define MAX_DESC 150
void trav_dirs(CopyList &file_list, const QStringList &base_paths,
               const QList<FilterPair> &fil_dirs){
    /**
      * @pre file_list contains list of strings, base_paths contains directory paths,
      * fil_dir contains filters and directories
      *
      * @post file_list might contain more strings, base_paths should remain the same
      * fil_dirs should remain the same
      */
    file_list.empty();
    QString str;
    foreach(str, base_paths){
        std::string a_str = str.toStdString();
        const char *s = a_str.c_str();

        nftw(s, start_trav, MAX_DESC, FTW_MOUNT);

        const char* c = NULL;
        QStringList dir_list = get_dirs(c);

        process_dirs(dir_list, file_list, fil_dirs);
    }

}

int start_trav(const char *path, const struct stat *buf,
               int typeflag, FTW *ft_buf){
    /**
      * @pre
      *
      * @returns -1 on faliure else returns true
      */

    if(typeflag == FTW_F){//check if it is a file
        get_dirs(path);
    }

    return 0;
}

QStringList get_dirs(const char *p_name){
    /**
      * @pre p_name contains the string to be stored
      */
    static QStringList dir_list;
    if(p_name == NULL){
        QStringList temp = dir_list;
        dir_list.empty();

        return temp;
    }else{
        QString s(p_name);
        dir_list.append(s);
        return QStringList();
    }

    return QStringList();
}

void process_dirs(QStringList dir_list, CopyList &file_list,
                  const QList<FilterPair> &fil_pairs){
    /**
      * @pre dir_list contains list of filepaths to be processed, file_list contains
      * already processed files and fil_pairs contains filters and their directories
      */

    QString str;

    foreach(str, dir_list){
        FilterPair a_pair;

        foreach(a_pair, fil_pairs){

            QStringList filters = a_pair.second;
            QString a_str;
            foreach(a_str, filters){
                remove_ast(a_str);

                if(str.endsWith(a_str)){
                    CopyPair p;
                    QString f_name;

                    get_file_name(str, f_name);

                    p.first = str;
                    p.second = a_pair.first;
                    p.second.append("/");
                    p.second.append(f_name);

                    file_list.append(p);
                }
            }
        }
    }
}

void remove_ast(QString &str){
    /**
      * @pre str contains a non-modified string
      *
      * @post str contains a modified string
      */
    str.remove("*");

}

void get_file_name(const QString &path, QString &f_name){
    /**
      * @pre f_name is empty
      *
      * @post f_name contains a filename
      */

    QString t = path;

    QStringList t_list = t.split("/");

    f_name = t_list.back();
}

#endif//Q_OS_LINUX
