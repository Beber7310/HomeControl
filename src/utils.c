/*
 * utils.c
 *
 *  Created on: 25 d�c. 2013
 *      Author: Bertrand
 */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "utils.h"
#include "Components.h"
#include "radiateur.h"

#define NBR_MSG_MAX	10000
message_t msg_log[NBR_MSG_MAX];

int last_msg=0;

void log_msg(const char* level,char* tag,char *format,va_list args)
{

	strncpy(msg_log[last_msg].level,level,sizeof(msg_log[0].level));
	strncpy(msg_log[last_msg].tag,tag,sizeof(msg_log[0].tag));
	vsprintf(msg_log[last_msg].message,format,args);
	msg_log[last_msg].date=time(NULL);

	last_msg++;
	if(last_msg>=NBR_MSG_MAX)
		last_msg=0;


}

int get_http_msg(char* bufhttp,int buflen)
{
	int len=0;
	int ii;
 	char buf[512];
	ii=(last_msg-1);

	while(ii!=last_msg)
	{
		if(ii<0)
			ii=NBR_MSG_MAX-1;

		if(msg_log[ii].date>0)
		{
			sprintf(buf,"[%s] [%s] %s\n",msg_log[ii].level,msg_log[ii].tag,msg_log[ii].message);
			strcpy(&bufhttp[len],buf);
			len+=strlen(buf);
			printf("%i\n",len);
		}
		if(len>buflen-512)
		{
			return len;
		}
		ii--;
	}
	return len;
}

int get_http_sts(char* bufhttp,int buflen)
{
	int len=0;
	int ii,jj,kk;
 	char buf[512];
 	int index_prog;

	index_prog=get_pgm_index();

	/////////////////////////////////////////////////////////////////
	// Radiator
	sprintf(buf,"<table border=\"1\">\n");
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);

	sprintf(buf,"<tr><th>Radiator</th> <th>target temp</th> <th>Power</th> </tr>\n");
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);

	for(ii=0;ii<RD_LAST;ii++)
	{
		sprintf(buf,"<tr><td>%s</td> <td>%i</td><td>%i</td> </tr>\n",radiateur[ii].name,radiateur[ii].calculated_target_temp,radiateur[ii].expected_state);
		strcpy(&bufhttp[len],buf);
		len+=strlen(buf);
		if(len>buflen-512)
		{
			return len;
		}
	}
	sprintf(buf,"</table>\n");
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);

	/////////////////////////////////////////////////////////////////
	// radiateur program
	sprintf(buf,"\n");
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);

	for(ii=0;ii<RD_LAST;ii++)
	{
	sprintf(buf,"<table border=\"1\"><caption>%s</caption>\n",radiateur[ii].name);
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);


	sprintf(buf,"<tr><th>Day</th> \n");
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);


	for(kk=0;kk<24;kk++)
	{
	sprintf(buf,"<th colspan=\"4\">%ih00</th>\n",kk);
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);
	}

	sprintf(buf,"</tr>");
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);

	for(jj=0;jj<7*24*4;jj+=24*4)
	{
		sprintf(buf,"<tr><td>%i</td>",jj/96);
		strcpy(&bufhttp[len],buf);
		len+=strlen(buf);

		for(kk=0;kk<24*4;kk++)
		{
			if(index_prog==jj+kk)
			{
				sprintf(buf,"<td bgcolor = #FF0000>%0.0f</td>",radiateur[ii].program[jj+kk]);
			}
			else
			{
				sprintf(buf,"<td bgcolor = #%2.2xA0A0 >%0.0f</td>",(int)(100+30*((radiateur[ii].program[jj+kk]-15))),radiateur[ii].program[jj+kk]);
			}
			strcpy(&bufhttp[len],buf);
			len+=strlen(buf);
		}
		sprintf(buf,"</tr>");
		strcpy(&bufhttp[len],buf);
		len+=strlen(buf);
	}


	sprintf(buf,"</table>\n");
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);
	}

	/////////////////////////////////////////////////////////////////
	// Thermometer
	sprintf(buf,"<table border=\"1\">\n\n<p>");
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);

	sprintf(buf,"<tr> <th>Thermometer</th> <th>temp</th>  <th>age</th> </tr>");
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);
	for(ii=0;ii<TH_LAST;ii++)
	{

		sprintf(buf,"<tr> <td>%s</td> <td>%f</td>  <td>%is</td> </tr>",thermometer[ii].name,thermometer[ii].temperature,time(NULL)-thermometer[ii].mesure_date);
		strcpy(&bufhttp[len],buf);
		len+=strlen(buf);
		if(len>buflen-512)
		{
			return len;
		}
	}

	sprintf(buf,"</table>\n");
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);

	return len;
}

int parse_http_cmd(char* cmd)
{
	char* pch;
	pch = strtok(cmd,"?");
	pch = strtok(NULL,"?");
	while (pch != NULL)
	{
		printf("Command %s\n",pch);
		parse_http_cmd_token(pch);
		pch = strtok (NULL, "?");
	}
}

int parse_http_cmd_token(char* cmd)
{
	int ii;
	if(strcmp("PGM=off",cmd)==0)
	{
		info("HTTP","Commqnd receive: PGM Off");
		radiateur_init_pgm_piece(RD_CUISINE);
		radiateur_init_pgm_piece(RD_DAPHNEE);
		radiateur_init_pgm_piece(RD_VICTOR);
		radiateur_init_pgm_piece(RD_HOMECINEMA);
		radiateur_init_pgm_piece(RD_SALON);
	}

	if(strcmp("PGM=normal",cmd)==0)
	{
		info("HTTP","Command receive: PGM normal");
		radiateur_init_pgm_cuisine(RD_CUISINE);
		radiateur_init_pgm_chambre(RD_DAPHNEE);
		radiateur_init_pgm_chambre(RD_VICTOR);
		radiateur_init_pgm_homecinema(RD_HOMECINEMA);
		radiateur_init_pgm_salon(RD_SALON);

	}

	//LIGHT_Garage=on
	if(strncmp("LIGHT_",cmd,strlen("LIGHT_"))==0)
		{
		for(ii=0;ii<LI_LAST;ii++)
		{
			if(strcmp("LIGHT_",cmd)==0)
			{
				info("HTTP","Command receive: PGM normal");
				radiateur_init_pgm_cuisine(RD_CUISINE);
				radiateur_init_pgm_chambre(RD_DAPHNEE);
				radiateur_init_pgm_chambre(RD_VICTOR);
				radiateur_init_pgm_homecinema(RD_HOMECINEMA);
				radiateur_init_pgm_salon(RD_SALON);

			}
		}
	}
}

int get_http_cmd(char* bufhttp,int buflen)
{
	int len=0;
	int ii;
 	char buf[512];
	struct tm * timeinfo;

	sprintf(buf,"<html><head><title>Sample \"Hello, World\" Application</title></head>\n");
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);

	sprintf(buf,"<body bgcolor=white>\n");
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);



	// Radiator
	sprintf(buf,"[Radiator]\n");
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);

	for(ii=0;ii<RD_LAST;ii++)
	{

		sprintf(buf,"<p> %s\n",radiateur[ii].name);
		strcpy(&bufhttp[len],buf);
		len+=strlen(buf);

		sprintf(buf,"<a href=\"hc_cmd?RAD_%s=15\">15�C</a> \n",radiateur[ii].name,radiateur[ii].name);
		strcpy(&bufhttp[len],buf);
		len+=strlen(buf);

		sprintf(buf,"<a href=\"hc_cmd?RAD_%s=20\">20�C</a> \n",radiateur[ii].name,radiateur[ii].name);
		strcpy(&bufhttp[len],buf);
		len+=strlen(buf);

		sprintf(buf,"<a href=\"hc_cmd?RAD_%s=22\">22�C</a> \n",radiateur[ii].name,radiateur[ii].name);
		strcpy(&bufhttp[len],buf);
		len+=strlen(buf);
	}

	sprintf(buf,"<p>All radiators\n");
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);


	// 15�C
	sprintf(buf,"<a href=\"hc_cmd");
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);
	for(ii=0;ii<RD_LAST;ii++)
	{
		sprintf(buf,"?RAD_%s=15",radiateur[ii].name);
		strcpy(&bufhttp[len],buf);
		len+=strlen(buf);
	}
	sprintf(buf,"\">15�C</a> \n");
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);

	// 20�C
	sprintf(buf,"<a href=\"hc_cmd");
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);
	for(ii=0;ii<RD_LAST;ii++)
	{
		sprintf(buf,"?RAD_%s=20",radiateur[ii].name);
		strcpy(&bufhttp[len],buf);
		len+=strlen(buf);
	}
	sprintf(buf,"\">20�C</a> \n");
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);

	// 20�C
	sprintf(buf,"<a href=\"hc_cmd");
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);
	for(ii=0;ii<RD_LAST;ii++)
	{
		sprintf(buf,"?RAD_%s=22",radiateur[ii].name);
		strcpy(&bufhttp[len],buf);
		len+=strlen(buf);
	}
	sprintf(buf,"\">22�C</a> \n");
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);

	// Radiator program
	sprintf(buf,"<p>[Radiator program]\n");
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);


	sprintf(buf,"<p>All radiators\n");
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);


	// Off
	sprintf(buf,"<a href=\"hc_cmd?PGM=off\">Off </a>");
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);

	// Normal
	sprintf(buf,"<a href=\"hc_cmd?PGM=normal\">Normal</a>");
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);

	// Thermometer
	sprintf(buf,"<p><p><p>[Thermometer]\n");
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);

	for(ii=0;ii<TH_LAST;ii++)
	{

		sprintf(buf,"<p>%s\n  temp=%f\n  date=%s",thermometer[ii].name,thermometer[ii].temperature, ctime(&thermometer[ii].mesure_date));
		strcpy(&bufhttp[len],buf);
		len+=strlen(buf);
		if(len>buflen-512)
		{
			return len;
		}
	}

	// Light
	sprintf(buf,"<p><p><p>[Light]\n");
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);

	for(ii=0;ii<LI_LAST;ii++)
	{

		sprintf(buf,"<p>%s\n <a href=\"hc_cmd?LIGHT_%s=on\">on</a> <a href=\"hc_cmd?LIGHT_%s=off\">off</a> ",light[ii].name,light[ii].name,light[ii].name);
		strcpy(&bufhttp[len],buf);
		len+=strlen(buf);
		if(len>buflen-512)
		{
			return len;
		}
	}


	sprintf(buf,"</body></html>\n");
	strcpy(&bufhttp[len],buf);
	len+=strlen(buf);

	return len;
}

void info(char* tag,char *format,...)
{
	va_list args;
	va_start(args,format);

	printf("\e[32m[INFO] \e[0m");
	printf("[%s] ",tag);
	vprintf(format,args);
	printf("\n");
	va_end(args);

	va_start(args,format);
	log_msg("INFO",tag,format,args);
	va_end(args);

}

void warning(char* tag,char *format,...)
{
	va_list args;
	va_start(args,format);

	printf("\e[33m[WARNING] \e[0m");
	printf("[%s] ",tag);
	vprintf(format,args);
	printf("\n");
	va_end(args);

	va_start(args,format);
	log_msg("WARNING",tag,format,args);
	va_end(args);
}

void init_msg(void)
{
 int ii;

	for(ii=0;ii<NBR_MSG_MAX;ii++)
	{
		msg_log[ii].date=0;
		msg_log[ii].level[0]=0;
		msg_log[ii].message[0]=0;
		msg_log[ii].tag[0]=0;
	}

}

int get_pgm_index(void)
{
	time_t rawtime;
	struct tm * timeinfo;
	int index_prog;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	index_prog=((timeinfo->tm_wday*24*60)+(timeinfo->tm_hour*60)+timeinfo->tm_min)/15;

}
