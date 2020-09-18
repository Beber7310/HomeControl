#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "Components.h"
#include "utils.h"
#include "rrd.h"
#include "radiateur.h"
#include "SerialManagement.h"
#include "nodered.h"

/** SerialManagement.c
 *
 *  Created on: 16 d�c. 2013
 *      Author: Bertrand
 */

int fd_fil_pilote;
#define BAUDRATE B9600
#define FIL_PILOTE_DEVICE "/dev/ttyACM1"

int fd_rf;
#define BAUDRATE B9600
#define RF_DEVICE "/dev/ttyACM0"

int SerialFilPilote(void) {

	struct termios oldtio, newtio;

	/*
	 On ouvre le p�riph�rique du modem en lecture/�criture, et pas comme
	 terminal de contr�le, puisque nous ne voulons pas �tre termin� si l'on
	 re�oit un caract�re CTRL-C.
	 */
	fd_fil_pilote = open(FIL_PILOTE_DEVICE, O_RDWR | O_NOCTTY);
	if (fd_fil_pilote < 0) {
		perror(FIL_PILOTE_DEVICE);
		exit(-1);
	}

	tcgetattr(fd_fil_pilote, &oldtio); /* sauvegarde de la configuration courante */
	bzero(&newtio, sizeof(newtio)); /* on initialise la structure � z�ro */

	/*
	 BAUDRATE: Affecte la vitesse. vous pouvez �galement utiliser cfsetispeed
	 et cfsetospeed.
	 CRTSCTS : contr�le de flux mat�riel (uniquement utilis� si le c�ble a
	 les lignes n�cessaires. Voir la section 7 du Serial-HOWTO).
	 CS8     : 8n1 (8 bits,sans parit�, 1 bit d'arr�t)
	 CLOCAL  : connexion locale, pas de contr�le par le modem
	 CREAD   : permet la r�ception des caract�res
	 */
	newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;

	/*
	 IGNPAR  : ignore les octets ayant une erreur de parit�.
	 ICRNL   : transforme CR en NL (sinon un CR de l'autre c�t� de la ligne
	 ne terminera pas l'entr�e).
	 sinon, utiliser l'entr�e sans traitement (device en mode raw).
	 */
	newtio.c_iflag = IGNPAR | ICRNL;

	/*
	 Sortie sans traitement (raw).
	 */
	newtio.c_oflag = 0;

	/*
	 ICANON  : active l'entr�e en mode canonique
	 d�sactive toute fonctionnalit� d'echo, et n'envoit pas de signal au
	 programme appelant.
	 */
	newtio.c_lflag = ICANON;

	/*
	 initialise les caract�res de contr�le.
	 les valeurs par d�faut peuvent �tre trouv�es dans
	 /usr/include/termios.h, et sont donn�es dans les commentaires.
	 Elles sont inutiles ici.
	 */
	newtio.c_cc[VINTR] = 0; /* Ctrl-c */
	newtio.c_cc[VQUIT] = 0; /* Ctrl-\ */
	newtio.c_cc[VERASE] = 0; /* del */
	newtio.c_cc[VKILL] = 0; /* @ */
	newtio.c_cc[VEOF] = 4; /* Ctrl-d */
	newtio.c_cc[VTIME] = 0; /* compteur inter-caract�re non utilis� */
	newtio.c_cc[VMIN] = 1; /* read bloquant jusqu'� l'arriv�e d'1 caract�re */
	newtio.c_cc[VSWTC] = 0; /* '\0' */
	newtio.c_cc[VSTART] = 0; /* Ctrl-q */
	newtio.c_cc[VSTOP] = 0; /* Ctrl-s */
	newtio.c_cc[VSUSP] = 0; /* Ctrl-z */
	newtio.c_cc[VEOL] = 0; /* '\0' */
	newtio.c_cc[VREPRINT] = 0; /* Ctrl-r */
	newtio.c_cc[VDISCARD] = 0; /* Ctrl-u */
	newtio.c_cc[VWERASE] = 0; /* Ctrl-w */
	newtio.c_cc[VLNEXT] = 0; /* Ctrl-v */
	newtio.c_cc[VEOL2] = 0; /* '\0' */

	/*
	 � pr�sent, on vide la ligne du modem, et on active la configuration
	 pour le port
	 */
	tcflush(fd_fil_pilote, TCIFLUSH);
	tcsetattr(fd_fil_pilote, TCSANOW, &newtio);

	/*
	 la configuration du terminal est faite, � pr�sent on traite
	 les entr�es
	 Dans cet exemple, la r�ception d'un 'z' en d�but de ligne mettra
	 fin au programme.
	 */
	//while (STOP==FALSE) {     /* boucle jusqu'� condition de terminaison */
	/* read bloque l'ex�cution du programme jusqu'� ce qu'un caract�re de
	 fin de ligne soit lu, m�me si plus de 255 caract�res sont saisis.
	 Si le nombre de caract�res lus est inf�rieur au nombre de caract�res
	 disponibles, des read suivant retourneront les caract�res restants.
	 res sera positionn� au nombre de caract�res effectivement lus */
	/* res = read(fd_arduino,buf,255);
	 buf[res]=0;
	 printf(":%s", buf);
	 if (buf[0]=='z') STOP=TRUE;*/

	/* restaure les anciens param�tres du port */
	//tcsetattr(fd_arduino,TCSANOW,&oldtio);
	return 0;
}

int SerialRF(void) {

	struct termios oldtio, newtio;

	/*
	 On ouvre le p�riph�rique du modem en lecture/�criture, et pas comme
	 terminal de contr�le, puisque nous ne voulons pas �tre termin� si l'on
	 re�oit un caract�re CTRL-C.
	 */
	fd_rf = open(RF_DEVICE, O_RDWR | O_NOCTTY);
	if (fd_rf < 0) {
		perror(RF_DEVICE);
		exit(-1);
	}

	tcgetattr(fd_rf, &oldtio); /* sauvegarde de la configuration courante */
	bzero(&newtio, sizeof(newtio)); /* on initialise la structure � z�ro */

	/*
	 BAUDRATE: Affecte la vitesse. vous pouvez �galement utiliser cfsetispeed
	 et cfsetospeed.
	 CRTSCTS : contr�le de flux mat�riel (uniquement utilis� si le c�ble a
	 les lignes n�cessaires. Voir la section 7 du Serial-HOWTO).
	 CS8     : 8n1 (8 bits,sans parit�, 1 bit d'arr�t)
	 CLOCAL  : connexion locale, pas de contr�le par le modem
	 CREAD   : permet la r�ception des caract�res
	 */
	newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;

	/*
	 IGNPAR  : ignore les octets ayant une erreur de parit�.
	 ICRNL   : transforme CR en NL (sinon un CR de l'autre c�t� de la ligne
	 ne terminera pas l'entr�e).
	 sinon, utiliser l'entr�e sans traitement (device en mode raw).
	 */
	newtio.c_iflag = IGNPAR | ICRNL;

	/*
	 Sortie sans traitement (raw).
	 */
	newtio.c_oflag = 0;

	/*
	 ICANON  : active l'entr�e en mode canonique
	 d�sactive toute fonctionnalit� d'echo, et n'envoit pas de signal au
	 programme appelant.
	 */
	newtio.c_lflag = ICANON;

	/*
	 initialise les caract�res de contr�le.
	 les valeurs par d�faut peuvent �tre trouv�es dans
	 /usr/include/termios.h, et sont donn�es dans les commentaires.
	 Elles sont inutiles ici.
	 */
	newtio.c_cc[VINTR] = 0; /* Ctrl-c */
	newtio.c_cc[VQUIT] = 0; /* Ctrl-\ */
	newtio.c_cc[VERASE] = 0; /* del */
	newtio.c_cc[VKILL] = 0; /* @ */
	newtio.c_cc[VEOF] = 4; /* Ctrl-d */
	newtio.c_cc[VTIME] = 0; /* compteur inter-caract�re non utilis� */
	newtio.c_cc[VMIN] = 1; /* read bloquant jusqu'� l'arriv�e d'1 caract�re */
	newtio.c_cc[VSWTC] = 0; /* '\0' */
	newtio.c_cc[VSTART] = 0; /* Ctrl-q */
	newtio.c_cc[VSTOP] = 0; /* Ctrl-s */
	newtio.c_cc[VSUSP] = 0; /* Ctrl-z */
	newtio.c_cc[VEOL] = 0; /* '\0' */
	newtio.c_cc[VREPRINT] = 0; /* Ctrl-r */
	newtio.c_cc[VDISCARD] = 0; /* Ctrl-u */
	newtio.c_cc[VWERASE] = 0; /* Ctrl-w */
	newtio.c_cc[VLNEXT] = 0; /* Ctrl-v */
	newtio.c_cc[VEOL2] = 0; /* '\0' */

	/*
	 � pr�sent, on vide la ligne du modem, et on active la configuration
	 pour le port
	 */
	tcflush(fd_rf, TCIFLUSH);
	tcsetattr(fd_rf, TCSANOW, &newtio);

	return 0;
}

void SerialFilPiloteSendCommande(void) {
	char cmdline[1024];

	char cmd = 0;
	int ii;

	for (ii = 0; ii < RD_LAST; ii++) {
		if (radiateur[ii].type == FIL_PILOTE) {
			if (ii == RD_SALON) {
				cmd += (radiateur[ii].expected_state ? 1 : 0)
						<< (radiateur[ii].index);
			} else {
				cmd += (radiateur[ii].expected_state ? 0 : 1)
						<< (radiateur[ii].index);
			}
		}

		/* Should not be commented*/
		if (radiateur[ii].type == RF_CONTROLED) {
			if ((radiateur[ii].expected_state != radiateur[ii].mirror_state)
					|| ((time(NULL) - radiateur[ii].mirror_time) > 900)) {
				radiateur[ii].mirror_time = time(NULL);
				SendBlyssCmd(radiateur[ii].index,
						radiateur[ii].expected_state ? 0 : 1);
				SendBlyssCmd(radiateur[ii].index,
						radiateur[ii].expected_state ? 0 : 1);
				SendBlyssCmd(radiateur[ii].index,
						radiateur[ii].expected_state ? 0 : 1);
				radiateur[ii].mirror_state = radiateur[ii].expected_state;
			}
		}

		if (radiateur[ii].type == SONOFF_HTTP) {
			if ((radiateur[ii].expected_state != radiateur[ii].mirror_state)
					|| ((time(NULL) - radiateur[ii].mirror_time) > 900)) {
				char cmdline[1024];

				radiateur[ii].mirror_time = time(NULL);
				radiateur[ii].mirror_state = radiateur[ii].expected_state;

				// sprintf(cmdline, "wget \"%s&payload=%s\"", radiateur[ii].mqtt_topic, radiateur[ii].expected_state ? "OFF" : "ON"); // Inverted on purpose as 220V on pilote wire  put the heater in "sleep"
				// system(cmdline);

				nodered_publish(radiateur[ii].mqtt_topic,
						radiateur[ii].expected_state ? "OFF" : "ON"); // Inverted on purpose as 220V on pilote wire  put the heater in "sleep"

			}

		}
	}

	//write(fd_fil_pilote, &cmd, 1);
	sprintf(cmdline,
			"wget -q -O temp.filp \"http://homecontrolremote:8080/hc_cmd?FILP_DATA=%i\"",
			cmd);
	system(cmdline);
	// http://homecontrolremote:8080/hc_cmd?FILP_DATA=255
}

int SerialSendChar(char data) {
	return write(fd_rf, &data, 1);
}

void update_capteur_info(char *pBuf) {
	int ii;
	int identified = 0;

	for (ii = 0; ii < TH_LAST; ii++) {
		if (pBuf[1] == 'V' && thermometer[ii].type == 'V') {
			if (strncmp(pBuf, thermometer[ii].id, 18) == 0) {
				char str_value[128];
				char str_topic[256];

				thermometer[ii].temperature = atof(pBuf + 20);
				thermometer[ii].mesure_date = time(NULL);
				if (strlen(pBuf) == 30) {
					rain.current_rain = atoi(pBuf + 26);
					rain_calcul();
					logData("rn", thermometer[ii].name, time(NULL),
							rain.falled);
					info("RF", "Received rain %s: %i", thermometer[ii].name,
							rain.current_rain);
				}

				identified++;
				sem_post(&sem_capteur_data_available);
				info("RF", "Received thermometer %s: %f", thermometer[ii].name,
						thermometer[ii].temperature);
				logData("th", thermometer[ii].name, time(NULL),
						thermometer[ii].temperature);

				sprintf(str_value, "%f", thermometer[ii].temperature);
				sprintf(str_topic, "%s/temperature",
						thermometer[ii].mqtt_topic);
				nodered_publish(str_topic, str_value);
			}

		}
		if (pBuf[1] == 'C' && thermometer[ii].type == 'C') {
			if (strncmp(pBuf, thermometer[ii].id, 10) == 0) {
				char str_value[128];
				char str_topic[256];
				// Serial.print("Temp: ");
				// int temp = ((data[5] & 0x0F) << 4) | ((data[6] & 0xF0) >> 4);
				// Serial.println(temp/10.0f);

				//>C:65085033300F30
				pBuf[16] = 0;
				thermometer[ii].temperature = strtol(pBuf + 13, 0, 16) / 10.0f;
				thermometer[ii].mesure_date = time(NULL);
				identified++;
				sem_post(&sem_capteur_data_available);
				info("RF", "Received thermometer %s: %f", thermometer[ii].name,
						thermometer[ii].temperature);
				logData("th", thermometer[ii].name, time(NULL),
						thermometer[ii].temperature);

				sprintf(str_value, "%f", thermometer[ii].temperature);
				sprintf(str_topic, "%s/temperature",
						thermometer[ii].mqtt_topic);
				nodered_publish(str_topic, str_value);

				pBuf[12] = 0;
				thermometer[ii].hygrometrie = (float) strtol(pBuf + 10, 0, 16);
				info("RF", "Received Humidity %s: %f%%", thermometer[ii].name,
						thermometer[ii].hygrometrie);
				logData("hy", thermometer[ii].name, time(NULL),
						thermometer[ii].hygrometrie);

				sprintf(str_value, "%f", thermometer[ii].hygrometrie);
				sprintf(str_topic, "%s/hygrometrie",
						thermometer[ii].mqtt_topic);
				nodered_publish(str_topic, str_value);
			}
		}
	}

	for (ii = 0; ii < IT_LAST; ii++) {
		if (strncmp(pBuf, interrupter[ii].id, 10) == 0) {
			char str_value[128];
			char str_topic[256];

			interrupter[ii].action = (pBuf[11] == '1') ? 1 : 0;
			interrupter[ii].action_date = time(NULL);
			identified++;
			sem_post(&sem_capteur_data_available);
			info("RF", "Received interupter %i: %i", ii,
					interrupter[ii].action);

			sprintf(str_value, "%i", interrupter[ii].action);
			sprintf(str_topic, "%s/switch", interrupter[ii].mqtt_topic);
			nodered_publish(str_topic, str_value);
		}
	}

	for (ii = 0; ii < PR_LAST; ii++) {
		if (strncmp(pBuf, presence[ii].id, 10) == 0) {
			char str_value[128];
			char str_topic[256];
			time_t now;

			presence[ii].action_date = time(NULL);
			identified++;
			sem_post(&sem_capteur_data_available);
			info("RF", "Received presence : %s", presence[ii].name);
			logData("pr", presence[ii].name, time(NULL), 1.0f);

			now = time(0);
			strftime(str_value, 100, "%d-%m-%Y- %H:%M:%S.000", localtime(&now));

			sprintf(str_topic, "%s/presence", presence[ii].mqtt_topic);
			nodered_publish(str_topic, str_value);

		}
	}

	if (identified == 0 && pBuf[0] == '>') {
		warning("RF", "Unidentified rf tag %s", pBuf);
	}
}

void* uart_rf_loop(void *arg) {
	char buf[255];
	int index = 0;
	int lastTargetUpdate = 0;
	int res;

	SerialRF();

	while (1) {
		res = read(fd_rf, &buf[index], 255);
		index += res;

		for (int ii = 0; ii < sizeof(buf); ii++) {
			if ((buf[ii] == '\n') || (buf[ii] == '\r')) {
				buf[ii] = 0;
				index = 0;
				update_capteur_info(buf);
			}
		}

		if ((time(NULL) - lastTargetUpdate) > 60) {
			lastTargetUpdate = time(NULL);
			//warning("Target", "Send");
			for (int ii = 0; ii < RD_LAST; ii++) {
				logData("targ", radiateur[ii].name, time(NULL),
						radiateur[ii].calculated_target_temp);
			}
		}

	}

}

void* uart_filPilote_loop(void *arg) {
	char buf[255];
	int index = 0;
	int sampleCount = 0;
	float currentAcc = 0;

	int res;

	SerialFilPilote();

	while (1) {
		res = read(fd_fil_pilote, &buf[index], 255);
		index += res;

		for (int ii = 0; ii < sizeof(buf); ii++) {
			if ((buf[ii] == '\n') || (buf[ii] == '\r')) {
				buf[ii] = 0;

				index = 0;

				if (buf[0] == 'A') {
					// data comming from arduino is 100ma by unit.
					power.current = atof(buf + 2) / 10;
					power.power = 230 * power.current;

					currentAcc += power.current;
					sampleCount++;
					//info("AMP","%f",power.current);
					if (sampleCount > 60) {
						sampleCount = 0;
						logData("amp", "house", time(NULL), currentAcc / 60.0f);
						logData("watt", "house", time(NULL),
								230 * currentAcc / 60.0f);
						currentAcc = 0;
					}
				}
			}
		}
	}

}

int SendBlyssCmd(int id, int value) {
	char timestamp = 0;
	static char key_index = 0;
	char key[] = { 0x98, 0xDA, 0x1E, 0xE6, 0x67 };

	long ms; // Milliseconds
	time_t s;  // Seconds
	struct timespec spec;

	sem_wait(&sem_blyss);
	clock_gettime(CLOCK_REALTIME, &spec);

	s = spec.tv_sec;
	ms = spec.tv_nsec / 326; // Convert nanoseconds to milliseconds

	timestamp = (311 * ms) / 1000;

	//0x98 -> 0xDA -> 0x1E -> 0xE6 -> 0x67

	char cmd[17] = "FE6152280981C0 ";

	/*
	 cmd[3]='0';
	 cmd[4]='0';
	 cmd[5]='0';
	 */
	cmd[6] = (id % 10) + '0';

	cmd[8] = (value > 0) ? '0' : '1';

	cmd[9] = hextochar(key[key_index] >> 4);
	cmd[10] = hextochar(key[key_index] & 0xF);

	cmd[11] = hextochar(timestamp >> 4);
	cmd[12] = hextochar(timestamp & 0xF);

	info("BLYSS", "Command Send: %s", cmd);

	key_index = (key_index + 1) % sizeof(key);

	int ret = write(fd_rf, cmd, strlen(cmd));
	sem_post(&sem_blyss);
	return ret;
}

