/* CSci4061 F2015 Assignment 4
 * Student: Zhexuan Zachary Yang &  Yuanting Shao
 * X500:     yang4442      &  shaox119
 * we did extra credits
 */

#ifndef  MAKEARGV_H
#define MAKEARGV_H

#include <errno.h>
#include <stdlib.h>
#include <string.h>

int makeargv(const char*s, const char *delimiters, char ***argvp);
void freemakeargv(char **argv);


#endif // MAKEARGV_H
