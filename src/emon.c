/*
 * emon.c
 *
 *  Created on: 6 f�vr. 2015
 *      Author: Bertrand
 */


#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <stdarg.h>

//
//  http://emoncms.org/input/post.json?json={power:200}&apikey=33d19b54e3e8cc180aa03bf0eeee8518
//


void emon_log_data(char* name, float value)
{
	//if(fork()==0)
	{
		char cmdline[1024];
		sprintf(cmdline,"curl  \"http://emoncms.org/input/post.json?json={%s:%f}&apikey=33d19b54e3e8cc180aa03bf0eeee8518\"",name,value);
		system(cmdline);

	}

}


