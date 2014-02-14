//THIS IS A CLIENT

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <signal.h>
#define PORT 42069


static void handle_signal(int signum) {
	psignal(signum, " Signal caught");
	exit(EXIT_FAILURE);
}

static void setup_signal_handlers() {
	struct sigaction s;
	memset(&s, 0, sizeof(s));
	s.sa_handler = handle_signal;
	sigaction(SIGINT, &s, NULL);
}

int *get_divisors(int n) {
	
	int *divisors = malloc(sizeof(int) * n/2);
	
	//finds the divisors of n
	int j = 0;
	for(int i = 1; i < n/2+1; i++) {
		if(n % i == 0){
			divisors[j] = i;
			j++;
		}
	}
	return divisors;
}

long instructions_per_second() {
	//function that gives instructions per seconds of the function compute_perfect.
	//pretty gross function but it's technically correct - the best kind of correct
	time_t start = time(NULL);
	volatile long iops = 0;
	long magic_value = 17;
	
	for(long i = 1; i < 42000000; ++i) {
		iops += magic_value % i;
	}
	time_t end = time(NULL);
	//printf("%ld\n", end-start);

	//just happens to be that 42000000 is approximately how many mod operations os-class can do in 1 second.
	return 42000000; 
}

int *parse_xml(char *xml_string) {
	
	int *range = malloc(sizeof(int) * 2);
	//if the header is <range>
	if(xml_string[0] == '<' && xml_string[1] == 'r' && xml_string[2] == 'a' &&
	   xml_string[3] == 'n' && xml_string[4] == 'g' && xml_string[5] == 'e' &&
	   xml_string[6] == '>') {
		range[0] = xml_string[7] - '0'; //the start of the range to check (always 1)
		
		char temp[5];
		temp[0] = xml_string[10];
		temp[1] = xml_string[11];
		temp[2] = xml_string[12];
		temp[3] = xml_string[13];
		temp[4] = xml_string[14];

		range[1] = atoi(temp);
	}
	return range;
}

int *compute_perfect(int start, int end) {
	/*computes and returns the perfect numbers from start to end.*/
	/*brute force algorithm, which makes it extremely slow for high values*/
	  
	int *perfect_numbers = malloc(sizeof(int) * end); //arbitrary sizing
	
	volatile int k = 0;
	for(volatile int i = start; i < end; i++) {
		volatile int j = 0; 
		volatile int temp = 0;
		int *divisors = get_divisors(i);
		while(divisors[j]) { 
			temp += divisors[j];
			j++;
		}
		if(i == temp) {
			perfect_numbers[k] = i;
			k++;
		}
	}	
	return perfect_numbers;
}

int main(int argc, char **argv) {
	
	//amount of operations that can be performed in approximately 15 seconds.
	long usable_operations = instructions_per_second() * 15; 
	char message_to_send[5000];
	char range[5000];
	int hostname = getpid();
	int sockfd;
	int len, rc;
	struct sockaddr_in address;
	int result;
	setup_signal_handlers();
	
	snprintf(message_to_send, 5000, "%s%d, %ld%s", "<proc>", hostname, usable_operations, "</proc>");
	printf("SENDING HOSTNAME AND SPECIFICATIONS TO SERVER\n");

	for(int i = 0; i < 2; i++) { //magical for loop
	
	    //Create socket for client
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if(sockfd == -1) {
			perror("Socket create failed.\n");
			exit(EXIT_FAILURE);
		}
	
		//Name the socket
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = inet_addr("128.193.37.168");
		address.sin_port = htons(PORT);
		len = sizeof(address);
		
		result = connect(sockfd, (struct sockaddr *)&address, len);
		if(result == -1) {
			perror("wat");
			exit(EXIT_FAILURE);
		}
		
		//Sends hostname and operations per second to the server
		rc = write(sockfd, message_to_send, 5000);
		
		if(i == 1) break; //don't remove - this is magic

		//Accepts the range of perfect numbers to check returned by the server
		memset(range, 0, sizeof(range));
		read(sockfd, range, 5000);
		printf("RANGE RECEIVED FROM SERVER\n");
		
		//Parses the range and computes perfect numbers
		int *range_to_check = parse_xml(range);
		time_t start = time(NULL);
		sleep(6);
		int *perfect_numbers = compute_perfect(range_to_check[0], range_to_check[1]);
		time_t end = time(NULL);
		//Sends perfect numbers to the server
		memset(message_to_send, 0, sizeof(message_to_send));
		snprintf(message_to_send, 5000, "%s%d, %d, %d, %d%s", "<results>", perfect_numbers[0], perfect_numbers[1], perfect_numbers[2], perfect_numbers[3], "</results>");
		
		write(sockfd, message_to_send, 5000);
		printf("PERFECT NUMBERS SENT TO SERVER\n");
		printf("TIME: %lds\n", end-start);
	}
	close(sockfd);

	return 0;
}
