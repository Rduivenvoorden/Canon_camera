/*****************************************************************************/
/*  record_gps.c - opens a GPS connection and saves local time, GPS          */
/*  coordinates, altitude and heading to a csv file                          */
/*                                                                           */
/*  Rikky Duivenvoorden -- June 14, 2016                                     */
/*  UTAT UAV                                                                 */
/*                                                                           */
/*****************************************************************************/

/* compile using gcc: gcc record_gps.c -o record_gps -lm -lgps -Wall */
#include <gps.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define GPS_WAIT 1000000

int main(){

	int gps_recv;
	struct gps_data_t gps_data;
	struct timeval tv;
	struct tm *tm_info;
	FILE *gps_file_ptr;
	
	int millisec;
	
	char file_path[] = "/home/odroid/Canon_camera/Canon_gps_log.csv";
	
	/* Open GPS device (GPSD) */
	gps_recv = gps_open("localhost", "2947", &gps_data);
	
	if (gps_recv == -1) {
		printf("code: %d, reason %s\n", gps_recv, gps_errstr(gps_recv) );
	}
	
	/* Start streaming GPS data */
	gps_stream(&gps_data, WATCH_ENABLE, NULL);
	
	char lat_marker;
	char lng_marker;

	double latitude;
	double longitude;
	double latitude_min;
	double longitude_min;
	
	gps_file_ptr = fopen (file_path, "a");

	int i;
	for (i=1;i<=50;i++) {
	
		if (gps_waiting(&gps_data, GPS_WAIT)) {
			/* Then GPS data is available, so read the data */
			gps_recv = gps_read(&gps_data);
			/* printf("%d\n", gps_recv); */
			
			/* Check for error in reading data */
			if (gps_recv == -1) {
				printf("Error reading GPS. code: %d, reason %s\n", gps_recv, gps_errstr(gps_recv) );
			}
			
			else {
				/* No problems in reading data, check for GPS fix*/
	
				/* printf("No errors %d -- fix: %d\n", MODE_3D, gps_data.fix.mode); */
				
				if ( (gps_data.status == STATUS_FIX) && (gps_data.fix.mode == MODE_2D || gps_data.fix.mode == MODE_3D) && !isnan(gps_data.fix.latitude) && !isnan(gps_data.fix.longitude) ) {
					
					latitude = fabs(gps_data.fix.latitude);
					longitude = fabs(gps_data.fix.longitude);

					latitude_min = (latitude - (int) latitude)*60;
					longitude_min = (longitude - (int) longitude)*60;
	
					if (gps_data.fix.latitude >= 0 ) { lat_marker = 'N'; }
					else { lat_marker = 'S'; }
					
					if (gps_data.fix.longitude >= 0 ) { lng_marker = 'E'; }
					else { lng_marker = 'W'; }
					
					printf("%f %c, %f %c \n", fabs(gps_data.fix.latitude), lat_marker, fabs(gps_data.fix.longitude), lng_marker);
					
					/* Write timestamp in YYYY-MM-DD_HH-MM-SS-sss */
					gettimeofday(&tv, NULL);
					char time_buffer[26];
					millisec = (long) (tv.tv_usec/1000.0 + 0.5); // Round to nearest millisec
					
					tm_info = localtime(&tv.tv_sec);
					
					strftime(time_buffer, 26, "%Y-%m-%d_%H-%M-%S-", tm_info);
					
					fprintf(gps_file_ptr, "%s", time_buffer);
					fprintf(gps_file_ptr, "%03d,", millisec);
					
					/* Write latititude and longitude in DD-MM-SS.sss */
					fprintf(gps_file_ptr, "%d-%d-%f,%c,", (int) latitude, (int) latitude_min, (latitude_min - (int) latitude_min )*60, lat_marker);
					fprintf(gps_file_ptr, "%d-%d-%f,%c,", (int) longitude, (int) longitude_min, (longitude_min - (int) longitude_min )*60, lng_marker);
					/* Write Altitude, Heading */
					if (gps_data.fix.mode == MODE_3D) {
						fprintf(gps_file_ptr, "%f,%f\n", gps_data.fix.altitude, gps_data.fix.track);
					}
					else {
						/* if not 3D fix, altitude is not defined */
						fprintf(gps_file_ptr, "%d,%f\n", 0, gps_data.fix.track);
					}
					
					/* fprintf(gps_file_ptr, "%f%c,%f%c\n", fabs(gps_data.fix.latitude), lat_marker, fabs(gps_data.fix.longitude), lng_marker); */
						
				}
			}
		}
		else {
		  printf("-\n");
			/* printf("\n ----- Did not wait long enough ----- \n");*/
		}
	}
	
	/* Close logging file */
	fclose(gps_file_ptr);
	
	/* Stop streaming data and close GPS connection */
	gps_stream(&gps_data, WATCH_DISABLE, NULL);
	gps_close(&gps_data);
	
	printf("GPS -- %d\n", gps_recv);
	
	return 0;
}
