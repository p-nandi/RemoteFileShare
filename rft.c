/**
 * @pnandi_assignment1
 * @author  Priyankar Nandi <pnandi@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 *
 * References has bee taken from http://www.beej.us/guide/bgnet/output/print/bgnet_A4.pdf
 * and the recitation slides
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include "../include/global.h"
//#include "../include/host_details_struct_util.h"

//global variables
char *client_server; //defines whether client/server
int port; //port num
char *port_str;
//variable to hold the hostname
char *hostname;
//variable to hold the hostip
char *ip_addr;
//array of server ip list
//struct host_details serverip_list[20];

fd_set master_fds;

int fdmax; //maximum file descriptor number
int newfd; //newly accepted socket descriptor

//structure that contains the host details
struct host_details{
	int host_id;
	char host_name[30];
	char host_ip[30];
	char host_port[10];
	int active;
	char host_short_name[15];
	//upload parameters of a peer
	unsigned int num_u;unsigned int size_u;unsigned int time_u;unsigned int rate_u;
	//download parameters of a peer
	unsigned int num_d;unsigned int size_d;unsigned int time_d;unsigned int rate_d;
};

//local host details
struct host_details local_host;

//global list which contains the list of all the clients registered
//with the server i.e dokken
struct host_details serverip_list[SERVERIP_LIST_SIZE];
//list of clients that the current host(client) is connected to
struct host_details clientip_list[CLIENTIP_LIST_SIZE];

//global variable to store the message
char msg_array[50][100];

//global variable to maintain the server socket descriptor
int server_sd;

/**
 * defines the file block size
 */
int file_buf_length = 1024;
/**
 * input buf length
 */
int input_buf_len = 400;
/**
 * Method declarations
 */
void set_hostname();
int set_hostip();
void displayCreatorDtls();
void display_myport();
void display_help();
void display_myip();
/**
 * sets up the server socket
 */
int set_server_socket();

/**
 * register port no and ip at the server
 */
int register_client(char *server_ip, char *server_port);

/**
 * connect peer at the specified ip and port
 */
int connect_peer(char *peer_ip, char *peer_port);
/**
 * connect destination at the specified port number
 */
void display_connect(char *dest_ip, char *port_no);
/**
 * display the connections the process is part of
 */
void display_list();
/**
 * accepts connection from client and talks to it. Returns the secondary socket descriptor
 * on which communication happens
 */
int accept_client(int server_socket);
/**
 * Used by client. Creates socket and connects with server to the specified port
 * and ip. Returns the socket descriptor
 */
int connect_server(char *server_ip, char *port_num);

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa);

/*
 * send the total meassge in buffer
 */
int sendall(int s, char *buf, int *len);

struct host_details serverip_list[20];
/**
 * converts message to an array
 */
void convert_msg_to_array(char *msg_arg);

/**
 * convert message array to a single host
 */
struct host_details convert_msgarr_to_host();

/**
 * adds host to the server ip list
 */

void add_host_to_serverip_list(struct host_details host,int sock_index);
/**
 * print the host details in the list
 */
void print_host_details_arr(struct host_details ip_list[],int size);
/**
 * convert host array details to a message
 */
char * convert_host_arr_to_msg(struct host_details ip_list[],int size,char *msg);
/**
 * update serverip list
 */
void update_serverip_list(char *msg);

/**
 * finds the host details based on ip or name from the server ip list
 */
struct host_details find_host_in_serverip_list(char *peerip,char *peerport);
/**
 * finds the host details based on ip or name from the client ip list
 */
struct host_details find_host_in_clientip_list(char *peerip,char *peerport);

/**
 * update client ip list
 */
void update_clientip_list(int sock_index,struct host_details host);
/**
 * print the host details in the client ip list
 */
void print_clientip_list();
/**
 * print the host details in the server ip list
 */
void print_serverip_list();
/**
 * Terminates a peer connection
 */

/**
 * finds the host details based on connection id from the client ip list
 */
struct host_details find_host_in_clientip_list_from_conn_id(char* conn_id_str);

/**
 * terminates the connection with peer and also remove the conn id from master list
 */
int close_conn_with_peer(struct host_details host,int sock_index);

/**
 * Uploads a file to the specified connection id
 * Reference - https://www.cs.bu.edu/teaching/c/file-io/intro/
 * http://www.codecodex.com/wiki/Read_a_file_into_a_byte_array
 */
int upload_file(char *conn_id_str,char *filename);


/**
 * Called by the hosts when it closes all connections and exits
 */
void exit_process();

/**
 * removes host from server/client ip list
 */
void remove_host_from_ip_list(struct host_details ip_list[],struct host_details host);
/**
 * sets the filename in the global array and returns the index at which the
 * filename is stored
 */
int find_file_name(char *msg_arg);

/**
 * Saves the file to the disk
 */
void save_file(char *filename,char *filecontent,int block_bytes_read);

/*
 * receives the total meassge in buffer
 */
int receiveall(int s, char *buf, int *len);

/**
 * sets the host short name
 */
void set_host_short_name(struct host_details* host);
/**
 * Function called when DOWNLOAD Command is typed from the command prompt
 */

void send_request_for_download(char *conn_id_str,char *filepath);
/**
 * Checks if a given string contains only numbers
 */
int validate_integer(char *s);
/**
 * Updates the upload statistics in client ip list
 */
void update_upload_transfer_details_in_clientip_list(struct host_details host);
/**
 * Updates the download statistics in client ip list
 */
void update_download_transfer_details_in_clientip_list(struct host_details host);
/**
 * display upload / download statistics in client
 */

void display_statistics();

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
int main(int argc, char **argv) {
	//Show error when wrong no. of arguments are passed from the command line
	if (argc != 3) {
		fprintf(stderr,
				"\nPlease enter two arguments - 1. s/c (Server or client) 2. Port number\n");
		exit(1);
	}
	client_server = argv[1];
	port_str = argv[2];

	if(strcmp("s",client_server) && strcmp("c",client_server)){
		fprintf(stderr,
						"\nPlease enter s or c as the first argument\n");
		exit(1);
	}

	int is_valid_port = validate_integer(port_str);
	if(!is_valid_port){
		fprintf(stderr,
						"\nPort number is invalid. It can be only digits\n");
		exit(1);
	}
	port = atoi(argv[2]);
	set_hostname();
	int hostip_flag=set_hostip();
	if(hostip_flag==-1){
		fprintf(stderr,"\nUnable to get the IP of the host\n");
		exit(1);
	}
	//set the local host details
	strcpy(local_host.host_name,hostname);
	strcpy(local_host.host_ip,ip_addr);
	strcpy(local_host.host_port,port_str);

	set_host_short_name(&local_host);
	//update the serverip list for dokken
	if (!strcmp(client_server, "s")) {
		struct host_details host;
		host.host_id = 1;
		host.active =1;
		strcpy (host.host_name,hostname);
		strcpy (host.host_ip,ip_addr);
		strcpy (host.host_port,port_str);
		serverip_list[0]=host;
	}
	int listener = set_server_socket();
	if(listener==-1){
		exit(1);
	}
	//fd_set master_fds;//master file descriptor list
	fd_set temp_fds; //temp file descriptor list
	struct sockaddr_storage remoteaddr; //client address
	socklen_t addrlen;
	char buf[512]; //buffer for client data
	int buf_len = 512;
	int nbytes;
	char remoteIP[INET6_ADDRSTRLEN];
	int yes = 1; //for setsockopt() So that server can reuse the same address
	//clear all entries
	FD_ZERO(&master_fds);
	FD_ZERO(&temp_fds);
	//Register the server socket
	FD_SET(listener, &master_fds);
	//Register the standard  input
	FD_SET(STDIN, &master_fds);
	fdmax = listener; // keep the biggest file descriptor
	printf("\nPA1$->");
	fflush(stdout);
	char cmd[input_buf_len];
	while (1) {
		temp_fds = master_fds; //copy the master list
		int ret_select = select(fdmax + 1, &temp_fds, NULL, NULL, NULL);
		if (ret_select < 0)
			fprintf(stderr, "\nSelect operation failed");
		else {
			//Loop through the set of socket descriptors to check which one is ready
			int sock_index;
			for (sock_index = 0; sock_index <= fdmax; sock_index++) {
				if (FD_ISSET(sock_index, &temp_fds)) {
					/*Check if there are any new inputs from keyboard*/
					if (sock_index == STDIN){
						memset(cmd,'\0',input_buf_len);
						fgets(cmd, input_buf_len, stdin);
						char *cmdarg;
						char cmdargv[10][100];
						int cmdargc = 0;
						int i;
						for (i = 0; i < input_buf_len; i++) {
							if (cmd[i] == '\n')
								cmd[i] = '\0';
						}
						cmdarg = strtok(cmd, " ");
						while (cmdarg) {
							strcpy(cmdargv[cmdargc], cmdarg);
							cmdargc++;
							cmdarg = strtok(NULL, " ");
						}

						if (!(strcasecmp(cmdargv[0], "CREATOR"))) {
							displayCreatorDtls();
						} else if (!(strcasecmp(cmdargv[0], "HELP"))) {
							display_help();
						}

						else if (!(strcasecmp(cmdargv[0], "MYPORT"))) {
							display_myport();
						}

						else if (!(strcasecmp(cmdargv[0], "MYIP"))) {
							display_myip();
						} else if (!(strcasecmp(cmdargv[0], "REGISTER")) && !strcasecmp("c",client_server)) {
							char *server_ip = cmdargv[1];
							char *server_port = cmdargv[2];
							int register_flag=register_client(server_ip, server_port);
							if(register_flag==-1){
								printf("\nERROR - Unable to register the client");
							}
							if(register_flag==-2){
								printf("\nERROR - Client is already registered");
							}
                                                        if(register_flag==-3){
                                                                printf("\nClient cannot register with itself");
                                                        }

						} else if (!(strcasecmp(cmdargv[0], "CONNECT")) && !strcasecmp("c",client_server)) {
							char *peer_ip = cmdargv[1];
				            char *peer_port = cmdargv[2];
							int connect_p=connect_peer(peer_ip,peer_port);
							if(connect_p==-1){
								printf("\nERROR: Socket Error!!! Unable to connect peer");
							}
							else if(connect_p==-2){
								printf("\nERROR: Peer not registered with the server");
							}
							else if(connect_p==-3){
								printf("\nERROR: Duplicate connection");
							}
							else if(connect_p==-4){
								printf("\nERROR: Client not registered with the server");
							}
							else if(connect_p==-5){
								printf("\nERROR: Invalid port");
							}
                                                        else if(connect_p==-6){
                                                                printf("\nERROR: Self-connection not allowed");
                                                        }

						}

						else if (!(strcasecmp(cmdargv[0], "TERMINATE")) && !strcasecmp("c",client_server)) {
							char *conn_id_str = cmdargv[1];
							int ret=terminate(conn_id_str);
							if(ret==-1){
								printf("\nThere is no connection with the specified id %s",conn_id_str);
							}
							else{
								printf("\nConnection terminated successfully");
							}
						}

						else if (!(strcasecmp(cmdargv[0], "EXIT"))) {
							exit_process();
						}
						else if (!(strcasecmp(cmdargv[0], "LIST"))) {
							display_list();
						}
						else if (!(strcasecmp(cmdargv[0], "UPLOAD")) && !strcasecmp("c",client_server)) {
								char *conn_id_str = cmdargv[1];
								char *file_name = cmdargv[2];
								int upload_flag=upload_file(conn_id_str,file_name);
								if(upload_flag==-1){
										printf("\nERROR: Invalid connection");
								}else if(upload_flag==-2){
										printf("\nERROR: File does not exists");
						        }else if(upload_flag==0){
						    		printf("\nPA1$->");
						    		fflush(stdout);
						        }
						}
						else if (!(strcasecmp(cmdargv[0], "DOWNLOAD")) && !strcasecmp("c",client_server)) {
							    //calculate the total no. of files to be downloaded
							    int num_files=0;
							    if(strlen(cmdargv[1])>0 && strlen(cmdargv[2])>0 ){
							    	send_request_for_download(cmdargv[1],cmdargv[2]);
                                    num_files++;
							    }
							    if(strlen(cmdargv[3])>0 && strlen(cmdargv[4])>0 ){
							    	send_request_for_download(cmdargv[3],cmdargv[4]);
                                    num_files++;
							    }
							    if(strlen(cmdargv[5])>0 && strlen(cmdargv[6])>0 ){
							    	send_request_for_download(cmdargv[5],cmdargv[6]);
                                    num_files++;
							    }
							    printf("\nFile download in progress....\n\n");
							    						    		fflush(stdout);
							    //printf("No. of files to be downloaded %d",num_files);

						}
						else if (!(strcasecmp(cmdargv[0], "STATISTICS"))) {
                             display_statistics();
                        }
						else{
							printf("\nInvalid command");
						}
						printf("\nPA1$->");
						fflush(stdout);
						memset(cmd,'\0',input_buf_len);
					}
					/*Check if new client is requesting connection*/
					else if (sock_index == listener) {
						//handle new connections
						addrlen = sizeof remoteaddr;
						newfd = accept(listener,
								(struct sockaddr *) &remoteaddr, &addrlen);
						if (newfd == -1) {
							perror("accept");
						} else {
							FD_SET(newfd, &master_fds); //add to master set
							if (newfd > fdmax) {
								fdmax = newfd;
							}
							/*printf("\nConnection from %s on socket %d",
									inet_ntop(remoteaddr.ss_family,
											get_in_addr(
													(struct sockaddr*) &remoteaddr),
											remoteIP, INET6_ADDRSTRLEN), newfd);
							printf("\nSocket desc allocated %d", newfd);*/
						}
						fflush(stdout);

					}
					/*Read from existing clients*/
					else {
						memset(&buf,0,sizeof buf);
						int len = sizeof buf;
				/*		if ((nbytes = receiveall(sock_index,buf, &len))
														<= 0) {*/
						if ((nbytes = recv(sock_index, buf, sizeof buf, 0))
								<= 0) {
							//printf("\nNodata arrived!!");
							//print_host_details_arr(serverip_list,SERVERIP_LIST_SIZE);
							close(sock_index);
							FD_CLR(sock_index, &master_fds);
						}
						// there is some data for client
						else {
    					/*	printf("\nRECEIVED A NEW MESSAGE %s",buf);
							printf("\n-------------------------------------");*/
							char buf_copy[512],buf_copy1[512];
							strcpy(buf_copy,buf);
							strcpy(buf_copy1,buf);
							convert_msg_to_array(buf_copy);
							if (!strcmp(client_server, "s")) {
								//prepare the array from the message
								//printf("\nData for server %s",buf);
								//printf("\nData for server %s",buf_copy);
								//printf("\nFirst element of the array %s",msg_array[0]);
								if(!strcmp(msg_array[0],"REGISTER")){

									struct host_details host=convert_msgarr_to_host();
									add_host_to_serverip_list(host,sock_index);
									//print the server ip list
									print_serverip_list();
									printf("\nPA1$->");
									fflush(stdout);
									char buf_copy4[512]="REGISTER";
									strcat(buf_copy4,MSG_SEP_HOST_DTLS);
									char *msg_to_be_send=convert_host_arr_to_msg(serverip_list,20,buf_copy4);
									//printf("\nMessage to be sent %s",msg_to_be_send);
									int len = sizeof(msg_to_be_send);

									int j;
									//printf("\nfdmax= %d",fdmax);
									//exclude the standard input, standard output and standard error
									for (j = 3; j <= fdmax; j++) {
										//send the message received to all other clients
										if (FD_ISSET(j, &master_fds)) {
											//except the listner
											if (j != listener) {
												int len = sizeof(msg_to_be_send);
												if (send(j, msg_to_be_send, buf_len,0) == -1){
                                                      perror("send");
												}
											}

										}

									}
								}
								else if(!strcmp(msg_array[0],"EXIT")){
									char buf_copy3[512];
									strcpy(buf_copy3,buf_copy1);
									convert_msg_to_array(buf_copy3);
									struct host_details host=convert_msgarr_to_host();
									//removes the host from server ip list
									remove_host_from_ip_list(serverip_list,host);
									printf("\nHost %s exited from all processes\n",host.host_name);
									//print the new ip list
									print_serverip_list();
									int k;
									for (k = 3; k <= fdmax; k++) {
										//send the message received to all other clients
										if (FD_ISSET(k, &master_fds)) {
											//except the listner
											if (k != listener && k!=sock_index) {
												int len = sizeof(buf);
												if (send(k, buf, buf_len,0) == -1){
													  perror("send");
												}
											}

										}

									}
								}

							} else if (!strcmp(client_server, "c")) {
/*								printf("\nData for client");
								fflush(stdout);
								printf("\nMessage from server %s",buf_copy1);
						        fflush(stdout);
								printf("\nFirst element of the array %s",msg_array[0]);
								fflush(stdout);*/
								if(!strcmp(msg_array[0],"REGISTER")){
									char buf_copy3[512];
									strcpy(buf_copy3,buf_copy1);
									//printf("\nMessage from server 2 %s",buf_copy3);
									update_serverip_list(buf_copy3);
									//print the server ip list
									print_serverip_list();
									printf("\nPA1$->");
									fflush(stdout);
								}
								else if(!strcmp(msg_array[0],"CONNECT")){
									char buf_copy3[512];
									strcpy(buf_copy3,buf_copy1);
									//printf("\nMessage from server 2 %s",buf_copy3);
									convert_msg_to_array(buf_copy3);
									struct host_details host=convert_msgarr_to_host();
									update_clientip_list(sock_index,host);
									print_clientip_list();
									printf("\nPA1$->");
									fflush(stdout);
								}
								else if(!strcmp(msg_array[0],"TERMINATE")){
									char buf_copy3[512];
									strcpy(buf_copy3,buf_copy1);
									//printf("\nMessage from server 2 %s",buf_copy3);
									convert_msg_to_array(buf_copy3);
									struct host_details host=convert_msgarr_to_host();
									int ret=close_conn_with_peer(host,sock_index);
									//printf("\nValue of return is %d",ret);
									if(ret==0){
										printf("\nConnection terminated by %s",host.host_name);
									}
									print_clientip_list();
									printf("\nPA1$->");
									fflush(stdout);
								}
								else if(!strcmp(msg_array[0],"EXIT")){
										char buf_copy3[512];
										strcpy(buf_copy3,buf_copy1);
										convert_msg_to_array(buf_copy3);
										struct host_details host=convert_msgarr_to_host();

										printf("\nHost %s exited from all processes\n",host.host_name);
										//removes the host from server ip list
										remove_host_from_ip_list(serverip_list,host);
										//removes the host from server ip list
										remove_host_from_ip_list(clientip_list,host);
										//print the new ip list
										print_clientip_list();
										printf("\nPA1$->");
										fflush(stdout);
							   }
								else if(!strcmp(msg_array[0],"UPLOAD")){
									    //printf("\nReceived upload msg");
										char buf_copy3[512];
										strcpy(buf_copy3,buf_copy1);
										convert_msg_to_array(buf_copy3);
                                        char file_name[30] ;
                                        char file_size_str[10] ;
                                        strcpy(file_name,msg_array[1]);
                                        strcpy(file_size_str,msg_array[2]);


                                       /* printf("\nFile name and size received :: %s %s",file_name,file_size_str);
                                        fflush(stdout);*/

                                       int file_size = atoi(file_size_str);
                                        /*printf("\nFile name and size received :: %s %d",file_name,file_size);
                                        fflush(stdout);*/

                                       /* printf("\nReceiving more data");
                                        fflush(stdout);*/
                                        char file_buf[file_buf_length];
                                        int total_bytes_received=0;
                        				//start timer
                        				struct timeval start,end;
                        				gettimeofday(&start,NULL);
										while(total_bytes_received<file_size){
											   /* printf("\nReceiving more data loop");
											    fflush(stdout);*/
												int block_bytes_read = 0;
												if ((block_bytes_read = recv(sock_index, file_buf, file_buf_length , 0))
																				>0){
													total_bytes_received = total_bytes_received+block_bytes_read;
													save_file(file_name,file_buf,block_bytes_read);
													memset(&file_buf,'\0',sizeof file_buf);
												}
										}

										/*printf("\nTotal bytes received :: %d",total_bytes_received);
										fflush(stdout);*/

										//stop timer
										gettimeofday(&end,NULL);

										unsigned int time_for_tansfer;

										//for too short files
										if(end.tv_sec-start.tv_sec==0){
						                     time_for_tansfer=0;
										}
										else{
											 time_for_tansfer = end.tv_sec-start.tv_sec;
										}

										unsigned int rate_of_transfer;
										//for too short files set the rate to 0
										if(end.tv_sec-start.tv_sec==0){
											rate_of_transfer=0;
										}
										else{
											rate_of_transfer = (file_size*8)/time_for_tansfer;
										}
										char conn_id_str[20];
										sprintf(conn_id_str,"%d",sock_index);
										struct host_details host = find_host_in_clientip_list_from_conn_id(conn_id_str);
						                //update the peer transfer details for statistics
										set_host_short_name(&host);
						                //total upload size
										int total_size_d = host.size_d ;
										total_size_d = total_size_d+file_size;
										host.size_d = total_size_d;

						                //total time for the uploads
										int total_time_d = host.time_d ;
										total_time_d= total_time_d+time_for_tansfer;
										host.time_d = total_time_d;

						                //total time for the uploads
										int total_num_d = host.num_d ;
										total_num_d= total_num_d+1;
										host.num_d = total_num_d;

										//total rate for the uploads
										//total rate for the uploads
										if(host.time_d==0){
											host.rate_d = 0;
										}
										else{
											host.rate_d = host.size_d/host.time_d;
										}

										printf("\nRx: %s -> %s , File Size: %d Bytes, Time Taken: %d seconds, Rx Rate: %d bits/second\n",
												               host.host_short_name,local_host.host_short_name,file_size,time_for_tansfer,rate_of_transfer);
										fflush(stdout);
										printf("\nPA1$->");
										fflush(stdout);
										update_download_transfer_details_in_clientip_list(host);

							   }
								//if peer receives a DOWNLOAD request upload the file to the requesting peer
							   else if(!strcmp(msg_array[0],"DOWNLOAD")){
										char buf_copy3[512];
										strcpy(buf_copy3,buf_copy1);
									    char conn_id_str[10];
									    sprintf(conn_id_str,"%d",sock_index);
										int upload_flag=upload_file(conn_id_str,msg_array[1]);
										if(upload_flag==-1){
												printf("\nERROR: Invalid connection");
										}else if(upload_flag==-2){
												printf("\nERROR: File does not exists");
										}else if(upload_flag==0){
											printf("\nPA1$->");
											fflush(stdout);
										}
							 }
						  }

						}
					}
				}
			}
		}
	}
	return 0;
}

void displayCreatorDtls() {
	printf(
			"\nFull Name: Priyankar Nandi\nUBIT Name: pnandi\nUB email address: pnandi@buffalo.edu");
	printf(
			"\nI have read and understood the course academic integrity policy located at"
					"\nhttp://www.cse.buffalo.edu/faculty/dimitrio/courses/cse4589_f14/index.html#integrity\n");
}

/**
 * Displays port number
 */
void display_myport() {
	printf("\nPort number:%d", port);
}

/**
 * Displays help menu
 */
void display_help() {
	printf("\n  -CREATOR Displays details about the creator\n");
	printf("\n  -MYIP Obtain the IP address\n");
	printf("\n  -MYPORT The port number the socket is listening to\n");
	if(!strcasecmp("c",client_server)){
		printf("\n  -REGISTER <IP> <PORT> Client registers itself with the server and gets the IP"
				"\n  and listening port numbers of all the peers currently registered with the server\n");
		printf("\n  -CONNECT <DEST> <PORT> Host establishes a new TCP connection to the specified destination"
						"\n   at the specified port\n");
	}
	printf("\n  -LIST Display a numbered list of all the connections this process is part of\n");
	if(!strcasecmp("c",client_server)){
	     printf("\n  -TERMINATE <CONNECTION ID> Terminates the specified connection\n");
	}
	printf("\n  -EXIT Close all connections and terminate the process\n");
	if(!strcasecmp("c",client_server)){
		printf("\n  -UPLOAD <CONNECTION ID> <FILE NAME> Uploads a file to the specified connection\n");
		printf("\n  -DOWNLOAD <CONNECTION ID1> <FILE1> <CONNECTION ID2> <FILE2> Downloads multiple "
							"\n   files from multiple connections\n");
	}
	printf("\n  -STATISTICS Displays statistics about all the uploads and downloads\n");
}

/**
 * Finds the ip of the local host
 */
int set_hostip() {
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(DNS_SERVER, DNS_PORT, &hints, &res)<0) {
		printf("\nError occured while getting IP");
		return -1;
	}
	//create a socket
	int sock_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sock_fd < 0) {
		printf("\nError while creating socket");
		return -1;
	}

	//connect to DNS server
	if (connect(sock_fd, res->ai_addr, res->ai_addrlen) < 0) {
		printf("\nError while connecting");
		return -1;
	}

	/* get local adress information */
	struct sockaddr_in loc_addr;
	socklen_t loc_addr_len = sizeof(loc_addr);

	if(getsockname(sock_fd, (struct sockaddr*) &loc_addr, &loc_addr_len)<-1){
		return -1;
	}

	/* convert ip address to dot notation */
	static char hostip[INET_ADDRSTRLEN];
	inet_ntop(loc_addr.sin_family, &(loc_addr.sin_addr), hostip,
			INET_ADDRSTRLEN);
	ip_addr = hostip;
	close(sock_fd);
	//printf("\nHost ip is %s",ip_addr);
	return 0;

}

/**
 * Finds the host name
 */
void set_hostname() {
	static char hostnameArr[512];
	gethostname(hostnameArr, sizeof hostnameArr);
	hostname = hostnameArr;
	// printf("\nHostname is %s",hostnameArr);
}

/**
 * displays the ip address of the localhost
 */
void display_myip() {
	printf("\nIP address:%s", ip_addr);
}

/**
 * sets up the server socket. Reference has been taken from recitation slides
 */
int set_server_socket() {
	int yes = 1;
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket < 0) {
		perror("\nError while creating server socket");
		return -1;
	}
	struct sockaddr_in server_addr;
	//clear the bits
	memset(&server_addr, 0, sizeof(server_addr));
	//set the server address parameters
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(ip_addr);
	// lose the pesky "address already in use" error message
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	//binds to the port
	if (bind(server_socket, (struct sockaddr *) &server_addr,
			sizeof(server_addr)) < 0) {
		perror("bind");
		return -1;
	}
	//listens for incoming connections
	if (listen(server_socket, BACKLOG) < 0) {
		perror("listen");
		return -1;
	}
	return server_socket;
}

/**
 * Register client with the server
 */
int register_client(char *server_ip, char *server_port) {
        if(!strcmp(server_ip,ip_addr)){
            return -3;
        }
	int sock = connect_server(server_ip, server_port);
	if(sock<2){
		return -1;
	}
	struct host_details host =find_host_in_serverip_list(ip_addr,port_str);
	//check for duplicate registration
	if(strlen(host.host_name)>2){
	    return -2;
	}
	server_sd = sock;
	int server_port_num = atoi(server_port);
	FD_SET(sock, &master_fds);
	if (sock > fdmax)
		fdmax = sock;
	//printf("\nSock desc returned in register: %d\n", sock);
	char msg[512];

	//printf("\nPort %s",port_str);
	strcpy(msg, "REGISTER");
	strcat(msg, MSG_SEP_HOST_DTLS);
	strcat(msg, hostname);
	strcat(msg, MSG_SEP_HOST_DTLS);
	strcat(msg, ip_addr);
	strcat(msg, MSG_SEP_HOST_DTLS);
	strcat(msg, port_str);
	//strcat(msg,"\0");//done to remove garbage values
	//printf("Message to be sent %s", msg);

	int len = strlen(msg);

	/*if (sendall(sock, msg, &len) == -1) {
		perror("sendall");
	}*/
	if (send(sock, msg, len,0) == -1) {
			perror("sendall");
	}
	//If the client is registered successfully , update the server ip list and client ip list on client side
	// with the details of dokken
	else{
		struct host_details host;
		host.host_id = 1;
		host.active =1;
		strcpy (host.host_name,"dokken.cse.buffalo.edu");
		strcpy (host.host_ip,server_ip);
		strcpy (host.host_port,server_port);
		clientip_list[0]=host;
		serverip_list[0]=host;
		//print_clientip_list();
	}
    return 1;
}
/**
 * Establishes a connection at the specified destination ip and
 * port number
 */
void display_connect(char *dest_ip, char *port_no) {

}
/**
 * Display all connections the process is part of
 */
void display_list() {
	if(!strcmp(client_server,"s"))
	{
	   //print_host_details_arr(serverip_list,SERVERIP_LIST_SIZE);
		print_serverip_list();
	}
	else if(!strcmp(client_server,"c"))
	{
	    //print_host_details_arr(clientip_list,CLIENTIP_LIST_SIZE);
	    //print_host_details_arr(serverip_list,SERVERIP_LIST_SIZE);
		print_clientip_list();
	}
}

/**
 * accepts connection from client and talks to it. Returns the secondary socket descriptor
 * on which communication happens
 */
int accept_client(int server_socket) {
	char buf[512];
	struct sockaddr_storage client_addr;
	int len_client_addr = sizeof(client_addr);
	//accepts the connection from the client
	int sec_socket = accept(server_socket, (struct sockaddr *) &client_addr,
			&len_client_addr);
	return sec_socket;
}
/**
 * Used by client. Creates socket and connects with server to the specified port
 * and ip. Returns the socket descriptor
 */
int connect_server(char *server_ip, char *server_port) {
	//printf("\nServer port %s", server_port);
	int port_no = atoi(server_port);
	//printf("\nPort num %d", port_no);
	//create the socket
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	//create the server address
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(server_ip);
	server_addr.sin_port = htons(port_no);
	//printf("\nServer ip$$$ %s", server_ip);
	// printf("\nPort no.$$$ %d",port_no);
	//connects to SERVER
	if (connect(sock, (struct sockaddr*) &server_addr, sizeof(server_addr))
			== -1) {
		perror("connect");
		return -1;
	}
	return sock;
}


/**
 * connect peer at the specified ip and port
 */
int connect_peer(char *peer_ip, char *peer_port){

	    if(!strcmp(peer_ip,ip_addr)||!strcmp(peer_ip,hostname)){
                   return -6;
            } 
            int validate_f = validate_integer(peer_port);
	    if(!validate_f)
	    		return -5;

	    struct host_details host;

	    host =find_host_in_serverip_list(ip_addr,port_str);
		if(strlen(host.host_name)<1){
				return -4;
		}

	    struct host_details host1 =find_host_in_serverip_list(peer_ip,peer_port);
		if(strlen(host1.host_name)<1){
			return -2;
		}

		host =find_host_in_clientip_list(peer_ip,peer_port);

		if(strlen(host.host_name)>1){
					return -3;
		}

		int sock = connect_server(host1.host_ip, peer_port);
		if(sock<2){
			return -1;
		}


		int server_port_num = atoi(peer_port);
		FD_SET(sock, &master_fds);
		if (sock > fdmax)
			fdmax = sock;
		//printf("\nSock desc returned in connect_peer: %d\n", sock);
		char msg[512];

		//printf("\nPort %s",peer_port);
		strcpy(msg, "CONNECT");
		strcat(msg, MSG_SEP_HOST_DTLS);
		strcat(msg, hostname);
		strcat(msg, MSG_SEP_HOST_DTLS);
		strcat(msg, ip_addr);
		strcat(msg, MSG_SEP_HOST_DTLS);
		strcat(msg, port_str);
		//strcat(msg,"\0");//done to remove garbage values
		//printf("Message to be sent in CONNECT PEER %s", msg);
		//printf("===================================================");
		int len = strlen(msg);

		/*if (sendall(sock, msg, &len) == -1) {
			perror("sendall");
		}*/
		if (send(sock, msg, len,0) == -1) {
				perror("sendall");
		}
		else{
			printf("\nConnection established successfully");
			update_clientip_list(sock,host1);
			print_clientip_list();
			printf("\nPA1$->");
		}
    return 0;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*) sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*) sa)->sin6_addr);
}
/* 
 * send the total meassge in buffer
 */
int sendall(int s, char *buf, int *len) {
	int total = 0; // how many bytes we've sent
	int bytesleft = *len; // how many we have left to send
	int n;
	while (total < *len) {
		n = send(s, buf + total, bytesleft, 0);
		if (n == -1) {
			break;
		}
		total += n;
		bytesleft -= n;
	}
	*len = total; // return number actually sent here
	return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
}


/*
 * receives the total meassge in buffer
 */
int receiveall(int s, char *buf, int *len) {
	int total = 0; // how many bytes we've received
	int bytesleft = *len; // how many we have left to receive
	int n;
	while (total < *len) {
		n = recv(s, buf + total, bytesleft, 0);
		if (n == -1) {
			break;
		}
		total += n;
		bytesleft -= n;
	}
	*len = total; // return number actually sent here
	return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
}



/**
 * converts message to an array
 */
void convert_msg_to_array(char *msg_arg){
	memset(&msg_array,0,sizeof msg_array);
	char *arg;
	int argc=0;
	arg=strtok(msg_arg,MSG_SEP_HOST_DTLS);
	while(arg){
		strcpy(msg_array[argc],arg);
		argc++;
		arg=strtok(NULL,MSG_SEP_HOST_DTLS);
	}

}

/**
 * convert message array to host
 */
struct host_details convert_msgarr_to_host(){
    struct host_details host;
	strcpy (host.host_name,msg_array[1]);
	strcpy (host.host_ip,msg_array[2]);
	strcpy (host.host_port,msg_array[3]);
	return host;
}

/**
 * adds host to the server ip list
 */

void add_host_to_serverip_list(struct host_details host,int sock){
	int i;

	for(i=0;i<20;i++){
        if(serverip_list[i].active!=1){
        	serverip_list[i].host_id = sock;
        	strcpy (serverip_list[i].host_name, host.host_name);
        	strcpy (serverip_list[i].host_ip, host.host_ip);
        	strcpy (serverip_list[i].host_port, host.host_port);
        	serverip_list[i].active = 1;
        	break;
        }
	}
}


/**
 * print the host details in the server ip list
 */
void print_host_details_arr(struct host_details ip_list[],int size){
	   printf("\n%-5s%-35s%-20s%-8s\n","ID","Hostname",
			   "IP Address", "Port No.");
	   //fflush(stdout);
       char host_name[30];
       char host_ip[30];
       int i;
	   for(i=0;i<size;i++){
		   if(ip_list[i].active==1){
			   int host_id= ip_list[i].host_id;
			   strcpy(host_name,ip_list[i].host_name);
			   strcpy(host_ip,ip_list[i].host_ip);
			   int port_num= atoi(ip_list[i].host_port);
			   printf("\n%-5d%-35s%-20s%-8d\n",host_id,host_name,
					   host_ip, port_num);
			   //fflush(stdout);
		   }
	    }
}


/**
 * print the host details in the server ip list
 */
void print_serverip_list(){

	   printf("\n\nServer-IP-List\n");
	   printf("\n.....................................................................");
	   printf("\n%-5s%-35s%-20s%-8s","ID","Hostname",
			   "IP Address", "Port No.");
	   printf("\n.....................................................................");
	   //fflush(stdout);
       char host_name[30];
       char host_ip[30];
       int i;
	   for(i=0;i<20;i++){
		   //print the server ip list for active clients except for the host itself
		   //TODO SERVER - 
                    if(serverip_list[i].active==1 && strcmp(serverip_list[i].host_name,hostname)){
		   //if(serverip_list[i].active==1){
			   int host_id= serverip_list[i].host_id;
			   strcpy(host_name,serverip_list[i].host_name);
			   strcpy(host_ip,serverip_list[i].host_ip);
			   int port_num= atoi(serverip_list[i].host_port);
			   printf("\n%-5d%-35s%-20s%-8d\n",host_id,host_name,
					   host_ip, port_num);
			   //fflush(stdout);
		   }
	    }
}


/**
 * print the host details in the client ip list
 */
void print_clientip_list(){
	   printf("\n\nClient-IP-List\n");
	   printf("\n.....................................................................");
	   printf("\n%-5s%-35s%-20s%-8s","ID","Hostname",
			   "IP Address", "Port No.");
	   printf("\n.....................................................................");
	   //fflush(stdout);
       char host_name[30];
       char host_ip[30];
       int i;
	   for(i=0;i<20;i++){
		   if(clientip_list[i].active==1){
			   int host_id= clientip_list[i].host_id;
			   strcpy(host_name,clientip_list[i].host_name);
			   strcpy(host_ip,clientip_list[i].host_ip);
			   int port_num= atoi(clientip_list[i].host_port);
			   printf("\n%-5d%-35s%-20s%-8d\n",host_id,host_name,
					   host_ip, port_num);
			   //fflush(stdout);
		   }
	    }
}

/**
 * converts host array details to message
 */

char * convert_host_arr_to_msg(struct host_details ip_list[],int size,char *msg){
	int i;
	for(i=0;i<size;i++){
		   if(serverip_list[i].active==1){
			   strcat(msg,ip_list[i].host_name);
			   strcat(msg,MSG_SEP_HOST_DTLS);
			   strcat(msg,ip_list[i].host_ip);
			   strcat(msg,MSG_SEP_HOST_DTLS);
			   strcat(msg,ip_list[i].host_port);
			   strcat(msg,MSG_SEP_HOST_DTLS);
		   }
    }
	//printf("\nMessage to be sent to clients %s",msg);
	return msg;
}



/**
 * update serverip list
 */
void update_serverip_list(char *msg){
	    //printf("\nUpdating server ip list");
	    memset(serverip_list, 0, sizeof serverip_list);
	    convert_msg_to_array(msg);
	    //Hard code the details of dokken as the first entry in the server ip list
/*		host.host_id = 1;
		host.active =1;
		strcpy (host.host_name,"dokken.cse.buffalo.edu");
		strcpy (host.host_ip,"128.162.19.1");
		strcpy (host.host_port,"6666");
		serverip_list[0]=host;*/

	    int i;
	    int arr_index=1;
		for(i=1;i<50;i=i+3){
			if((strlen(msg_array[i]))>1)
			{
				struct host_details host;
				memset(&host, 0, sizeof host);
				host.host_id = arr_index;
				host.active =1;
				strcpy (host.host_name,msg_array[i]);
				strcpy (host.host_ip,msg_array[i+1]);
				strcpy (host.host_port,msg_array[i+2]);
				serverip_list[arr_index]=host;
				arr_index++;
			}
	    }
		//serverip_list=temp_list;
}



/**
 * finds the host details based on ip or name from the server ip list
 */
struct host_details find_host_in_serverip_list(char *peerip,char *peerport){
   struct host_details host;
   memset(&host,0,sizeof host);
   int i;
   for(i=0;i<20;i++){
	   //TODO SERVER
		   if(serverip_list[i].active==1 &&
				         ((!strcmp(peerip,serverip_list[i].host_ip))||(!strcmp(peerip,serverip_list[i].host_name)))){
	        /* if(serverip_list[i].active==1 &&
	      				         ((!strcmp(peerip,serverip_list[i].host_ip))||(!strcmp(peerip,serverip_list[i].host_name)))
	      				         && (!strcmp(peerport,serverip_list[i].host_port))){*/
   			    host = serverip_list[i];
   		   }
   }
   return host;
}

/**
 * finds the host details based on ip or name from the client ip list
 */
struct host_details find_host_in_clientip_list(char *peerip,char *peerport){
   struct host_details host;
   memset(&host,0,sizeof host);
   int i;
   for(i=0;i<20;i++){
	   //TODO SERVER
/*		   if(clientip_list[i].active==1 &&
				         ((!strcmp(peerip,clientip_list[i].host_ip))||(!strcmp(peerip,clientip_list[i].host_name)))){*/
	         if(clientip_list[i].active==1 &&
	      				         ((!strcmp(peerip,clientip_list[i].host_ip))||(!strcmp(peerip,clientip_list[i].host_name)))
	      				         && (!strcmp(peerport,clientip_list[i].host_port))){
   			    host = clientip_list[i];
   		   }
   }
   return host;
}


/**
 * update client ip list
 */
void update_clientip_list(int sock_index,struct host_details host){
	    //printf("\nCalled update client ip list with port :: %s",host.host_port);
	    int i;
	    for(i=0;i<20;i++){
	    		   if(clientip_list[i].active!=1){
	    			   strcpy(clientip_list[i].host_name,host.host_name);
	    			   strcpy(clientip_list[i].host_ip,host.host_ip);
	    			   strcpy(clientip_list[i].host_port,host.host_port);
	    			   clientip_list[i].active = 1;
	    			   clientip_list[i].host_id = sock_index;
	    			   //default the upload/download parameters
	    			   clientip_list[i].num_u = 0;
	    			   clientip_list[i].size_u = 0;
	    			   clientip_list[i].time_u = 0;
	    			   clientip_list[i].rate_u = 0;
	    			   clientip_list[i].num_d = 0;
	    			   clientip_list[i].size_d = 0;
	    			   clientip_list[i].time_d = 0;
	    			   clientip_list[i].rate_d = 0;
	    			   break;
	    		   }
	    }
}

/**
 * Terminates a connection
 */
int terminate(char* conn_id_str){
	struct host_details host =find_host_in_clientip_list_from_conn_id(conn_id_str);
	if(strlen(host.host_name)>1){
		//terminate the connection
		FD_CLR(host.host_id,&master_fds);
		//send a TERMINATE message to the peer
		char msg[512];
		strcpy(msg, "TERMINATE");
		strcat(msg, MSG_SEP_HOST_DTLS);
		strcat(msg, hostname);
		strcat(msg, MSG_SEP_HOST_DTLS);
		strcat(msg, ip_addr);
		strcat(msg, MSG_SEP_HOST_DTLS);
		strcat(msg, port_str);
		int len = strlen(msg);
		if (send(host.host_id, msg, len,0) == -1) {
			perror("sendall");
			return -1;
		}
		close_conn_with_peer(host,host.host_id);
		print_clientip_list();
        return 0;
	}
	else{
		return -1;
	}
}


/**
 * finds the host details based on connection id from the client ip list
 */
struct host_details find_host_in_clientip_list_from_conn_id(char* conn_id_str){
   struct host_details host;
   memset(&host,0,sizeof host);
   int conn_id = atoi(conn_id_str);
   int i;
   for(i=0;i<20;i++){
   		   if(clientip_list[i].active==1
   				         && conn_id==clientip_list[i].host_id){
   			    host = clientip_list[i];
   			    break;
   		   }
   }
   return host;
}


/**
 * terminates the connection with peer and also remove the conn id from master list and clientip_list
 */
int close_conn_with_peer(struct host_details host,int fd){
	   int i;
	   close(fd);
	   FD_CLR(fd,&master_fds);
	   for(i=0;i<20;i++){
	   		   if(clientip_list[i].active==1
	   				         && (!strcmp(host.host_name,clientip_list[i].host_name))
	   				         && (!strcmp(host.host_ip,clientip_list[i].host_ip))
	   				         && (!strcmp(host.host_port,clientip_list[i].host_port))
	   		          )
	   		   {
	   			    //printf("\nClosing connection with client %s",host.host_name);
	   			    clientip_list[i].active = 0;
	   			    break;
	   		   }
	   }
	   return 0;
}

/**
 * Called by the client when it closes all connections and exits
 */
void exit_process(){
	struct host_details host1 =find_host_in_serverip_list(ip_addr,port_str);
	char *server_ip = serverip_list[0].host_ip;
    char *server_port = serverip_list[0].host_port;
    if(!strcmp(client_server,"c") && strlen(host1.host_name)>1)
    {
        //send an EXIT message to the peer
        char msg[512];
        strcpy(msg, "EXIT");
        strcat(msg, MSG_SEP_HOST_DTLS);
        strcat(msg, hostname);
        strcat(msg, MSG_SEP_HOST_DTLS);
        strcat(msg, ip_addr);
        strcat(msg, MSG_SEP_HOST_DTLS);
        strcat(msg, port_str);
        int len =strlen(msg);
        printf("\nServer socket in exit %d",server_sd);
        if (send(server_sd, msg, len,0) == -1) {
            perror("sendall");
        }
        int i=0;
        for(i=0;i<20;i++){
                FD_CLR(clientip_list[i].host_id,&master_fds);
                close(clientip_list[i].host_id);
                clientip_list[i].active = 0;
        }
   }
   exit(1);
}



/**
 * removes host from server/client ip list
 */
void remove_host_from_ip_list(struct host_details ip_list[],struct host_details host){
    int i;
    for(i=0;i<20;i++){
               if(ip_list[i].active==1
                             && (!strcmp(host.host_name,ip_list[i].host_name))
                             && (!strcmp(host.host_ip,ip_list[i].host_ip))
                             && (!strcmp(host.host_port,ip_list[i].host_port))
                      )
               {
                    FD_CLR(ip_list[i].host_id,&master_fds);
                    close(ip_list[i].host_id);
                    ip_list[i].active = 0;
                    break;
               }
    }
}


/**
 * Uploads a file to th specified connection id
 * Reference - https://www.cs.bu.edu/teaching/c/file-io/intro/
 * http://www.codecodex.com/wiki/Read_a_file_into_a_byte_array
 */
int upload_file(char *conn_id_str,char *filepath){
	struct host_details host =find_host_in_clientip_list_from_conn_id(conn_id_str);
	//variable to store the file name
	char *filename;
	if(strlen(host.host_name)<1){
		return -1;
	}
	printf("\nUploading File %s to %s",filepath,host.host_name);
	fflush(stdout);
	FILE *in;
	char *mode = "r";
	in = fopen(filepath,mode);
	if(in==NULL){
		return -2;
	}
	else{
		//Find the file length
		fseek(in,0,SEEK_END);
		int file_size = ftell(in);
/*		printf("\nSize of the file = %d bytes",file_size);
		fflush(stdout);*/
		char file_size_str[20];
		sprintf(file_size_str,"%d",file_size);
		fseek(in,0,SEEK_SET);

		//Find the filename
		char filepath_copy[300];
		strcpy(filepath_copy,filepath);
		int index= find_file_name(filepath_copy);
		filename = msg_array[index-1];

		//send the metadata of the file
		char msg[512];
		strcpy(msg, "UPLOAD");
		strcat(msg, MSG_SEP_HOST_DTLS);
		strcat(msg, filename);
		strcat(msg, MSG_SEP_HOST_DTLS);
		strcat(msg, file_size_str);
		int len =sizeof(msg);
		if (send(host.host_id, msg, len,0) == -1) {
					perror("sendall");
		}

		//start timer
		struct timeval start,end;
		gettimeofday(&start,NULL);

		//read the file block by block and send
		char file_buf[file_buf_length];
		int total_bytes_read=0;
		while(total_bytes_read<file_size){
				int block_bytes_read = fread(file_buf,sizeof(char),file_buf_length,in);
				total_bytes_read = total_bytes_read+block_bytes_read;
				if(sendall(host.host_id, file_buf, &file_buf_length) ==-1){
					perror("\nError while sending");
				}
				//memset(&file_buf,0,sizeof file_buf);
		}

		//stop timer
		gettimeofday(&end,NULL);

		unsigned int time_for_tansfer;

		//for too short files set the time to 0
		if(end.tv_sec-start.tv_sec==0){
			 time_for_tansfer=0;
		}
		else{
			 time_for_tansfer = end.tv_sec-start.tv_sec;
		}

		unsigned int rate_of_transfer;
		//for too short files set the rate to 0
		if(end.tv_sec-start.tv_sec==0){
			rate_of_transfer=0;
		}
		else{
			rate_of_transfer = (file_size*8)/time_for_tansfer;
		}
		//update the peer transfer details for statistics
		set_host_short_name(&host);
		//total upload size
		int total_size_u = host.size_u ;
		total_size_u = total_size_u+file_size;
		host.size_u = total_size_u;

		//total time for the s
		int total_time_u = host.time_u ;
		total_time_u= total_time_u+time_for_tansfer;
		host.time_u = total_time_u;

		//total time for the uploads
		int total_num_u = host.num_u ;
		total_num_u= total_num_u+1;
		host.num_u = total_num_u;

		//total rate for the uploads
		if(host.time_u==0){
			host.rate_u = 0;
		}
		else{
			host.rate_u = host.size_u/host.time_u;
		}
/*		printf("\nTotal bytes sent :: %d",total_bytes_read);
		printf("\nFile uploaded successfully");*/

		printf("\nTx: %s -> %s , File Size: %d Bytes, Time Taken: %d seconds, Tx Rate: %d bits/second\n",
							   local_host.host_short_name,host.host_short_name,file_size,time_for_tansfer,rate_of_transfer);
		fflush(stdout);
/*		printf("\nPA1$->");
		fflush(stdout);*/
		update_upload_transfer_details_in_clientip_list(host);

	}
	fclose(in);
	return 0;

}


/**
 * sets the filename in the global array and returns the index at which the
 * filename is stored
 */
int find_file_name(char *msg_arg){
    char *arg;
	int argc=0;
	arg=strtok(msg_arg,"/");
	while(arg){
		strcpy(msg_array[argc],arg);
		argc++;
		arg=strtok(NULL,"/");
	}
	return argc;
}
/**
 * Saves the file to the disk
 */
void save_file(char *filename,char *filecontent,int block_bytes_read){
	//printf("\nSaving file %s",filename);
	fflush(stdout);
	FILE *out;
	char *append_mode="a";
	out=fopen(filename,append_mode);
	if(NULL!=out){
	 /*  printf("\nFile opened ");
	   fflush(stdout);*/
       fwrite(filecontent,sizeof(char),file_buf_length,out);
	   fclose(out);
	}
	else{
		printf("\nCannot open file");
	}
}


/**
 * sets the host short name
 */
void set_host_short_name(struct host_details *host){
   if(!strcmp("underground.cse.buffalo.edu",host->host_name)){
       strcpy(host->host_short_name,"underground");
   }
   else if(!strcmp("euston.cse.buffalo.edu",host->host_name)){
	   strcpy(host->host_short_name,"euston");
   }
   else if(!strcmp("embankment.cse.buffalo.edu",host->host_name)){
	   strcpy(host->host_short_name,"embankment");
   }
   else if(!strcmp("highgate.cse.buffalo.edu",host->host_name)){
		strcpy(host->host_short_name,"highgate");
   }
   else if(!strcmp("phantom",host->host_name)){
		strcpy(host->host_short_name,"phantom_short");
   }
}

/**
 * Function called when DOWNLOAD Command is typed from the command prompt
 */

void send_request_for_download(char *conn_id_str,char *filepath){
	 struct host_details host =find_host_in_clientip_list_from_conn_id(conn_id_str);
	 if(strlen(host.host_name)>1){
			//printf("\nHost Found");
			char msg[512];
			strcpy(msg,"DOWNLOAD");
			strcat(msg, MSG_SEP_HOST_DTLS);
			strcat(msg, filepath);
			int len = sizeof(msg);
			if (send(host.host_id, msg, len,0) == -1) {
								perror("sendall");
			}
	 }else{
			printf("\nThere is no host with the specified connection id %s",conn_id_str);
	 }

}

/**
 * Checks if a given string contains only numbers
 */
int validate_integer(char *s){

	int flag = 1;
	int i;
	for (i = 0; i < strlen(s); i++) //Checking that each character of the string is numeric
	{
		    if (s[i]!='\n' && !isdigit(s[i]))
	        {
	            flag = 0;
	            break;
	        }
	}
	return flag;
}

/**
 * Updates the upload statistics in client ip list
 */
void update_upload_transfer_details_in_clientip_list(struct host_details host){
	 int i;
	 for(i=0;i<20;i++){
		 if(clientip_list[i].active==1  && !strcmp(host.host_name,clientip_list[i].host_name)){
			 clientip_list[i].num_u = host.num_u;
			 clientip_list[i].rate_u = host.rate_u;
			 clientip_list[i].size_u = host.size_u;
			 clientip_list[i].time_u = host.time_u;
		 }
	}
}


/**
 * Updates the upload statistics in client ip list
 */
void update_download_transfer_details_in_clientip_list(struct host_details host){
	 int i;
	 for(i=0;i<20;i++){
		 if(clientip_list[i].active==1  && !strcmp(host.host_name,clientip_list[i].host_name)){
			 clientip_list[i].num_d = host.num_d;
			 clientip_list[i].rate_d = host.rate_d;
			 clientip_list[i].size_d = host.size_d;
			 clientip_list[i].time_d = host.time_d;
		 }
	}
}

/**
 * display upload / download statistics in client
 */

void display_statistics(){
	       printf("\n\nUpload/Download Statistics\n");
		   printf("\n.......................................................................................................................");
		   printf("\n%-30s%-20s%-35s%-20s%-20s","Hostname","Total Uploads","Average Upload Speed(bps)",
				   "Total Downloads", "Average download speed (bps)");
		   printf("\n.......................................................................................................................");
		   fflush(stdout);
	       char host_name[30];
	       char host_ip[30];
	       int i;
		   for(i=0;i<20;i++){
			   if(clientip_list[i].active==1 && clientip_list[i].host_id!=1){
				   struct host_details host = clientip_list[i];
				   printf("\n%-30s%-20d%-35d%-20d%-20d\n",host.host_name,host.num_u,
						   host.rate_u, host.num_d,host.rate_d);
				   fflush(stdout);
			   }
		    }
}
