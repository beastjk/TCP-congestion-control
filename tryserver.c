#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include<sys/stat.h>
#define SERVER_PORT 5432
#define MAX_PENDING 6
#define buf_size 256




typedef struct frame{
	int packet_type;
	int frameno;
	int ack;
	char packet[1024];
};


int main(){
  
  struct sockaddr_in sin, newAddr;
  char buf[buf_size];
  socklen_t addr_size;
  int len;
  int s, new_s;
  int frame_id = 0;
  struct frame packet_send;
  struct frame packet_recv;
  char str[INET_ADDRSTRLEN];
  
  bzero((char *)&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons(SERVER_PORT);
  s = socket(PF_INET, SOCK_STREAM, 0);
  inet_ntop(AF_INET, &(sin.sin_addr), str, INET_ADDRSTRLEN);
  printf("Server is using address %s and port %d.\n", str, SERVER_PORT);

  bind(s, (struct sockaddr *)&sin, sizeof(sin)); 
  printf("Server bind done...\n");
  listen(s, MAX_PENDING);
	
	int cnwd = 1;
	int ssthresh; 
	int congestion = 40;	
	int flag = 0;

	
	
	while(1) 
	{
			
			if ((new_s = accept(s, (struct sockaddr *)&sin, &len)) < 0) 
			{
			  	perror("simplex-talk: accept");
			  	exit(1);
			}
			printf("Server Listening.\n");

		  	while(1)
		  	{
					
					int recv_len ;
					recv_len = recv(new_s, &packet_recv, sizeof(struct frame), 0);
					//printf("CHECK type of frame %d\n",packet_recv.packet_type);
					//printf("CHECK seq num %d\n",packet_recv.frameno);	
				  	if (recv_len > 0 && packet_recv.packet_type == 1) 
					//&& packet_recv.frameno == frame_id)   
					{
					
							
						if(packet_recv.frameno%congestion == 1 && packet_recv.frameno!=0 && packet_recv.frameno!=1)
						{	
							printf("Frame Received  : %d\n", packet_recv.frameno);
							packet_send.frameno = packet_recv.frameno;
							packet_send.packet_type = 0;
							packet_send.ack = packet_recv.frameno - 2;
							send(new_s, &packet_send, sizeof(struct frame), 0);
							printf("ACK SENT : %d \n", packet_send.ack);
						}
						else if(packet_recv.frameno%congestion == 2 && packet_recv.frameno!=0 && packet_recv.frameno!=2)
						{
							printf("Frame Received  :%d\n", packet_recv.frameno);
							packet_send.frameno = packet_recv.frameno;
							packet_send.packet_type = 0;
							packet_send.ack = packet_recv.frameno - 3;
							//for(int i = 0; i < 3; i++)
							//{
							send(new_s, &packet_send, sizeof(struct frame), 0);
							printf("ACK SENT : %d \n", packet_send.ack);
						}
						else if(packet_recv.frameno%congestion == 3 && packet_recv.frameno!=0 && packet_recv.frameno!=3)
						{
							printf("Frame Received  :  %d\n", packet_recv.frameno);
							packet_send.frameno = packet_recv.frameno;
							packet_send.packet_type = 0;
							packet_send.ack = packet_recv.frameno - 4;
							//for(int i = 0; i < 3; i++)
							//{
							send(new_s, &packet_send, sizeof(struct frame), 0);
							printf("ACK SENT : %d \n", packet_send.ack);
							
						}
 						else if(packet_recv.frameno%congestion == 0 && packet_recv.frameno!=0 && flag==0)
						{
							printf("Frame not received, packet loss\n");
							flag = 1;
							
						}
						else
						{
							if(flag==1)
							{
								printf("Frame Received : %d\n", packet_recv.frameno);
								packet_send.frameno = packet_recv.frameno;
								packet_send.packet_type = 0;
								packet_send.ack = packet_recv.frameno + 3;
								send(new_s, &packet_send, sizeof(struct frame), 0);		
								printf("ACK SENT : %d \n", packet_send.ack);
								flag = 0;	
							}
							else
							{
								printf("Frame Received : %d\n", packet_recv.frameno);
								packet_send.frameno = packet_recv.frameno;
								packet_send.packet_type = 0;
								packet_send.ack = packet_recv.frameno;
								send(new_s, &packet_send, sizeof(struct frame), 0);		
								printf("ACK SENT : %d \n", packet_send.ack);
							}
				
						}

					}
					else
					{
						printf("Frame not received  \n");
						break;
					}	
			//frame_id++;	
			}
			
			close(s);
			
    }
}
