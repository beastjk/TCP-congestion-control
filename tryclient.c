#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SERVER_PORT 5432
#define buf_size 256

typedef struct frame{
	int packet_type;
	int frameno;
	int ack;
	char packet[1024];
};

int main(int argc, char * argv[])
{
		  
		  FILE *fp;
		  FILE *fp1;
		  struct hostent *hp;
		  struct sockaddr_in sin;
		  char *host;
		  char buf[buf_size];
		  fp = fopen("data.txt","w");
		  fp1 = fopen("data1.txt","w");
		  //fp2 = fopen("data2.txt","w");
		  int s;
		  int count=0;
		  int len;
		  int i=1;
		  
		  struct frame packet_send;
		  struct frame packet_recv;
		  int acknoledge_recieve = 1;
		  if (argc==2) 
		  {
				host = argv[1];
		  }
		  else 
		  {
				perror("usage: simplex-talk host\n");
				exit(1);
		  }
		  hp = gethostbyname(host);
		  if (!hp) 
		  {
				perror("simplex-talk: unknown host \n");
				exit(1);
		  }
		  else
		  {
				printf("Client's remote host: %s\n", argv[1]);
		  }
		  bzero((char *)&sin, sizeof(sin));
		  sin.sin_family = AF_INET;
		  bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
		  sin.sin_port = htons(SERVER_PORT);

		  s = socket(PF_INET, SOCK_STREAM, 0);
			  printf("Client created socket...\n");

		  connect(s, (struct sockaddr *)&sin, sizeof(sin));
			  printf("Client connected...\n");
			  
			  
			  
			  
		  int congestion_window = 1;
		  int frame_no = 1;
		  int ssthresh = 20;
		  int recieve_window = 40;
		  int dup_acks = 0;
		  int store_acks = 0;
		  int count_dup_acks = 0;
		  int flag = 0, dup_flag = 0;

		  while(1)
		  {
				 
				 if (acknoledge_recieve = 1)
				 {
						//frame_send.frameno = frame_id;
						packet_send.packet_type = 1;
						packet_send.ack=0;

						//printf("Enter data :- ");
						//scanf("%s", buf);
						//frame_id = 0;
						for (i = 1 ; i <= congestion_window && i <= recieve_window && count <=50 ; i++)
						{			
							packet_send.frameno = frame_no;
							send(s, &packet_send, sizeof(struct frame), 0);
							frame_no++;
							printf(" Frame no. sent is : %d\n",packet_send.frameno);
							//recv(s, &packet_recv, sizeof(struct frame), 0);
							
							
							
							if(packet_send.frameno%40==0 && packet_send.frameno !=0)
							{
								printf("Acknoledgement not received------ packet loss\n");
							}
							else
							{
							
								if(len=recv(s, &packet_recv, sizeof(struct frame), 0))  //len = recv(new_s, buf, sizeof(buf), 0)
								{
									
									fprintf(fp1,"%d\n", packet_recv.ack);
																					
									printf("Acknoledgement recieved, ACK NO : %d\n",packet_recv.ack);
						
									if(store_acks == packet_recv.ack && count_dup_acks <= 3 && packet_recv.ack!=0)
									{
										count_dup_acks++;
										printf("count_dup_acks is : %d\n", count_dup_acks);
										if(count_dup_acks == 3)
										{
											packet_send.frameno = packet_send.frameno - 3;
											send(s, &packet_send, sizeof(struct frame), 0);
											printf("-------Frame sent----%d\n",packet_send.frameno);
											recv(s, &packet_recv, sizeof(struct frame), 0);
											printf("-------ACK recv----ACK NO : %d\n",packet_recv.ack);
											packet_send.frameno = packet_send.frameno + 3;
											goto label;
										}
									}
									store_acks = packet_recv.ack;
									//printf("store acks value is : %d\n",packet_recv.ack);
									acknoledge_recieve = 1;
								}
								else
								{
									printf("Acknoledgement not received------ packet loss\n");
									acknoledge_recieve = 0;
								}
							}
							//printf("in for loop %d\n", count);
							//count++;
							
						}
						printf("out for loop\n");
						//if(i == recieve_window+1)
						//{
							
					}
					
					label:
					
					count++;
					if(count==51)
					{
						break;
					}
					printf("-------Round trip time(RTT) completed :  %d \n",count);
					
					
					fprintf(fp,"%d,", congestion_window);
					fprintf(fp,"%d\n", count);
					//fprintf(fp2,"%d,", congestion_window);
					
					
		
					if(count_dup_acks == 3)
					{
						printf("Congestion is detected\n");
						printf(" New congestion window size is :  %d\n", congestion_window); 
						ssthresh = congestion_window/2;
						printf("New ssthresh is : %d\n", ssthresh);
						flag = 0;
						congestion_window = 1;
						dup_flag = 1;		//frame_no = ssthresh;
						count_dup_acks = 0;
					}
					else
					{
						if (congestion_window*2 > ssthresh)
						{
							flag++;
							if(flag==1 /*&& dup_flag != 1*/)
							{
								congestion_window = ssthresh;
								dup_flag = 0;
							}
							else
							{
								congestion_window++;	
							}
						} 
						else
						{ 
							congestion_window = congestion_window*2;
						}
				    }
				    	
		}
		fclose(fp);
		fclose(fp1);
		close(s);
}




