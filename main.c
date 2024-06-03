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

#include <signal.h>
#include <stdio.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <usefull_macros.h>

#include "cmdlnopts.h"
#include "loccorrbase.h"

static pid_t childpid = 0;

void signals(int signo){
    DBG("signo=%d", signo);
    if(childpid){ // master process
        if(signo == SIGUSR1){ // kill child
            kill(childpid, signo);
            signal(signo, signals);
            return;
        }
        WARNX("Master killed with sig=%d", signo);
        LOGERR("Master killed with sig=%d", signo);
        exit(signo);
    }
    // slave: make proper suicide
    if(signo){
        WARNX("Get signal %d - exit", signo);
        LOGWARN("Get signal %d - exit", signo);
    }
    // do something here
    DBG("exit(%d)", signo);
    exit(signo);
}

// check unix/net socket
static int chk(char *port, int path, const char *what){
    if(port){
        if(path){
            WARNX("Options `port` and UNIX-socket `path` can't be used together!");
            return 1;
        }
        int p = atoi(port);
        if(p < LC_PORTN_MIN || p > LC_PORTN_MAX){
            WARNX("Wrong port value: %d", p);
            return 1;
        }
    }
    if(!path && !port){
        WARNX("Point port or path of %s", what);
        return 1;
    }
    return 0;
}

int main(int argc, char **argv){
    initial_setup();
    parse_args(argc, argv);
    int r = chk(Glob_pars.port, (Glob_pars.path != NULL), "current server");
    if(r) return r;
    r = chk(Glob_pars.captcmdport, (Glob_pars.captpath && Glob_pars.cisun), "image capture server (don't forget -U for UNIX-socket)");
    if(r) return r;
    if(Glob_pars.captkey == -1 && !Glob_pars.captimport){
        WARNX("Point port or SHM key of image server");
        return 2;
    }
    if(!Glob_pars.lcplugin && !Glob_pars.telplugin){
        WARNX("Point local corrector and/or telesope plugin");
        return 3;
    }
    if(Glob_pars.logfile){
        int lvl = LOGLEVEL_WARN + Glob_pars.verbose;
        DBG("level = %d", lvl);
        if(lvl > LOGLEVEL_ANY) lvl = LOGLEVEL_ANY;
        verbose(1, "Log file %s @ level %d\n", Glob_pars.logfile, lvl);
        OPENLOG(Glob_pars.logfile, lvl, 1);
        if(!globlog) WARNX("Can't create log file");
    }
    signal(SIGINT, signals);
    signal(SIGQUIT, signals);
    signal(SIGABRT, signals);
    signal(SIGTERM, signals);
    signal(SIGHUP, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGUSR1, signals); // restart server

    LOGMSG("Started");
    DBG("Started");
#ifndef EBUG
    unsigned int pause = 5;
    while(1){
        childpid = fork();
        if(childpid){ // master
            double t0 = dtime();
            LOGMSG("Created child with pid %d", childpid);
            wait(NULL);
            LOGERR("Child %d died", childpid);
            if(dtime() - t0 < 1.) pause += 5;
            else pause = 1;
            if(pause > 900) pause = 900;
            sleep(pause); // wait a little before respawn
        }else{ // slave
            prctl(PR_SET_PDEATHSIG, SIGTERM); // send SIGTERM to child when parent dies
            break;
        }
    }
#endif
    sleep(5);
    return 0;
}

