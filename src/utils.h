/*
 * utils.h
 *
 *  Created on: 25 d�c. 2013
 *      Author: Bertrand
 */

#ifndef UTILS_H_
#define UTILS_H_

void init_msg(void);
void info(char* tag,char *format,...);
void warning(char* tag,char *format,...);

int get_http_msg(char* buf,int buflen);
int get_http_sts(char* bufhttp,int buflen);
int get_http_cmd(char* bufhttp,int buflen);
int parse_http_cmd(char* cmd);
int parse_http_cmd_token(char* cmd);


typedef struct {
	char 	level[16];
	char 	tag[32];
	char 	message[128];
	time_t 	date;

} message_t;

#endif /* UTILS_H_ */
