/*
 * This file is part of the loccorr project.
 * Copyright 2024 Edward V. Emelianov <edward.emelianoff@gmail.com>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include <usefull_macros.h>

/*
 * here are some typedef's for global data
 */
typedef struct{
    char *lcplugin;     // local corrector plugin
    char *telplugin;    // telescope plugin
    char *logfile;      // when run as server log here
    char *path;         // UNIX socket name
    char *port;         // INET socket port
    char *jpegport;     // port to send/receive jpeg images (by default == port+1)
    char *jpegfile;     // output file name
    char *lcheader;     // output FITS header with loccorr information
    char *captcmdport;  // INET command socket port of capture process
    char *captpath;     // UNIX command socket path of capture process or image capture server address
    char *captimport;   // INET image socket port of capture process
    int cisun;          // capture command channel is UNIX socket
    int captkey;        // image SHM key of capture process
    int verbose;        // each '-V' increases it
} glob_pars;

extern glob_pars Glob_pars;

void parse_args(int argc, char **argv);
void verbose(int levl, const char *fmt, ...);

