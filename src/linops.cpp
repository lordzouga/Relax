#include "linops.h"

#ifdef Q_OS_LINUX

#include <sys/stat.h>
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
    link(source, dest);
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

#endif//Q_OS_LINUX
