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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <usefull_macros.h>

#include "cmdlnopts.h"

static int help;

//            DEFAULTS
// default global parameters
glob_pars Glob_pars = {
    .captkey = -1,
};

// need this to proper work with only-long args
//#define NA  (-__COUNTER__)
// TODO: fix the bug in usefull_macros first!!!
#define NA (0)

/*
 * Define command line options by filling structure:
 *  name    has_arg flag    val     type        argptr          help
*/
myoption cmdlnopts[] = {
    {"lcplugin",NEED_ARG,   NULL,   'L',    arg_string, APTR(&Glob_pars.lcplugin),  "local corrector plugin (e.g liblcesprif.so)"},
    {"telplugin", NEED_ARG, NULL,   'T',    arg_string, APTR(&Glob_pars.telplugin), "telescope plugin (e.g. libbtaacs.so)"},
    {"help",    NO_ARGS,    NULL,   'h',    arg_int,    APTR(&help),                "show this help"},
    {"verbose", NO_ARGS,    NULL,   'V',    arg_none,   APTR(&Glob_pars.verbose),   "verbose level (-V - messages, -VV - debug, -VVV - all shit)"},
    {"logfile", NEED_ARG,   NULL,   'l',    arg_string, APTR(&Glob_pars.logfile),   "logging file name (if run as server)"},
    {"fitshead",NEED_ARG,   NULL,   'H',    arg_string, APTR(&Glob_pars.lcheader),  "output FITS header with loccorr information"},
    {"path",    NEED_ARG,   NULL,   NA,     arg_string, APTR(&Glob_pars.path),      "UNIX socket name (command socket)"},
    {"port",    NEED_ARG,   NULL,   'p',    arg_string, APTR(&Glob_pars.port),      "INET command socket port (can't be both UNIX and INET)"},
    {"imageport",NEED_ARG,  NULL,   NA,     arg_string, APTR(&Glob_pars.jpegport),  "INET JPEG socket port"},
    {"outfile", NEED_ARG,   NULL,   'o',    arg_string, APTR(&Glob_pars.jpegfile),  "output JPEG image file name"},
    {"shmkey",  NEED_ARG,   NULL,   'k',    arg_int,    APTR(&Glob_pars.captkey),   "image SHM key of capture process"},
    {"cport",   NEED_ARG,   NULL,   NA,     arg_string, APTR(&Glob_pars.captcmdport),"INET command socket port of capture process"},
    {"cpath",   NEED_ARG,   NULL,   NA,     arg_string, APTR(&Glob_pars.captpath),  "UNIX command socket path or address of image capture server"},
    {"ciport",  NEED_ARG,   NULL,   NA,     arg_string, APTR(&Glob_pars.captimport),"INET image socket port of capture process"},
    {"unix",    NO_ARGS,    NULL,   'U',    arg_int,    APTR(&Glob_pars.cisun),     "capture command channel is UNIX socket"},
    //{"",  NEED_ARG,   NULL,   '',    arg_double,   APTR(&Glob_pars.),    N_("")},
    end_option
};

/**
 * Parse command line options and return dynamically allocated structure
 *      to global parameters
 * @param argc - copy of argc from main
 * @param argv - copy of argv from main
 * @return allocated structure with global parameters
 */
void parse_args(int argc, char **argv){
    // format of help: "Usage: progname [args]\n"
    change_helpstring("Version: " PACKAGE_VERSION "\nTo restart server kill it with SIGUSR1\n\nUsage: %s [args]\n\twhere args are:\n");
    // parse arguments
    parseargs(&argc, &argv, cmdlnopts);
    if(help) showhelp(-1, cmdlnopts);
    DBG("argc=%d", argc);
    if(argc > 0){
        WARNX("%d unused parameter[s]:", argc);
        for(int i = 0; i < argc; ++i)
            fprintf(stderr, "%4d: %s\n", i, argv[i]);
        WARNX("Fix command line and run again");
        exit(-1);
    }
}

/**
 * @brief verbose - print additional messages depending of Glob_pars.verbose
 * @param levl - message level
 * @param fmt  - message
 */
void verbose(int levl, const char *fmt, ...){
    va_list ar;
    if(levl > Glob_pars.verbose) return;
    //printf("%s: ", __progname);
    va_start(ar, fmt);
    vprintf(fmt, ar);
    va_end(ar);
    printf("\n");
}
