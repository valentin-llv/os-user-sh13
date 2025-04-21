#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 

// For sockets
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

// For srand init
#include <time.h>

/*

    Data structures and global vars

*/

typedef struct _client {
    // Network infos
    char ipAddress[40];
    int port;

    // Player infos
    char name[40];
} Client;

Client tcpClients[4];

int nbClients;

// Flag to indicate the server is waiting for players
int fsmServer = 0;

// Sh13 game infos
int deck[13] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
int tableCartes[4][8];
char *nomcartes[] = {"Sebastian Moran", "Irene Adler", "Inspector Lestrade", "Inspector Gregson", "Inspector Baynes", "Inspector Bradstreet", "Inspector Hopkins", "Sherlock Holmes", "John Watson", "Mycroft Holmes", "Mrs. Hudson", "Mary Morstan", "James Moriarty"};

// Game state
int joueurCourant = 0;
int eliminated[4] = {0, 0, 0, 0}; // Indicate if a player is eliminated, 0 = not eliminated, 1 = eliminated
int nbPlayersRemaining;	// Count the number of players remaining

/*

    Utils

*/

void error(const char *msg) {
    perror(msg);
    exit(1);
}

/*

    Game utils

*/

void melangerDeck() {
    int index1, index2, tmp;

    for(int i = 0; i < 1000; i++) {
        index1 = rand() % 13;
        index2 = rand() % 13;

        tmp = deck[index1];
        deck[index1] = deck[index2];
        deck[index2] = tmp;
    }
}

void createTable() {
	// Le joueur 0 possede les cartes d'indice 0,1,2
	// Le joueur 1 possede les cartes d'indice 3,4,5 
	// Le joueur 2 possede les cartes d'indice 6,7,8 
	// Le joueur 3 possede les cartes d'indice 9,10,11 
	// Le coupable est la carte d'indice 12

	int i,j,c;

	for(i = 0; i < 4; i++) for(j = 0; j < 8; j++) tableCartes[i][j]=0;

	for(i = 0; i < 4; i++) {
		for(j = 0; j < 3; j++) {
			c = deck[i * 3 + j];

			switch(c) {
				case 0: // Sebastian Moran
					tableCartes[i][7]++;
					tableCartes[i][2]++;

					break;
				case 1: // Irene Adler
					tableCartes[i][7]++;
					tableCartes[i][1]++;
					tableCartes[i][5]++;

					break;
				case 2: // Inspector Lestrade
					tableCartes[i][3]++;
					tableCartes[i][6]++;
					tableCartes[i][4]++;

					break;
				case 3: // Inspector Gregson 
					tableCartes[i][3]++;
					tableCartes[i][2]++;
					tableCartes[i][4]++;

					break;
				case 4: // Inspector Baynes 
					tableCartes[i][3]++;
					tableCartes[i][1]++;

					break;
				case 5: // Inspector Bradstreet 
					tableCartes[i][3]++;
					tableCartes[i][2]++;

					break;
				case 6: // Inspector Hopkins 
					tableCartes[i][3]++;
					tableCartes[i][0]++;
					tableCartes[i][6]++;

					break;
				case 7: // Sherlock Holmes 
					tableCartes[i][0]++;
					tableCartes[i][1]++;

					tableCartes[i][2]++;
					break;
				case 8: // John Watson 
					tableCartes[i][0]++;
					tableCartes[i][6]++;
					tableCartes[i][2]++;

					break;
				case 9: // Mycroft Holmes 
					tableCartes[i][0]++;
					tableCartes[i][1]++;
					tableCartes[i][4]++;

					break;
				case 10: // Mrs. Hudson 
					tableCartes[i][0]++;
					tableCartes[i][5]++;

					break;
				case 11: // Mary Morstan 
					tableCartes[i][4]++;
					tableCartes[i][5]++;

					break;
				case 12: // James Moriarty 
					tableCartes[i][7]++;
					tableCartes[i][1]++;

					break;
			}
		}
	}
}

int nextPlayer(int currentPlayer) {
    // Return the next player to play

	int p = (currentPlayer + 1) % 4;
	while(eliminated[p]) p = (p + 1) % 4;

	return p;
}

/*

    Log utils

*/

void printDeck() {
    int i, j;
    for(i = 0; i < 13; i++) printf("%d %s\n",deck[i],nomcartes[deck[i]]);

	for(i = 0; i < 4; i++) {
        for(j = 0; j < 8; j++) printf("%2.2d ",tableCartes[i][j]);
		puts("");
	}
}

void printClients() {
    for(int i = 0; i < nbClients; i++) printf("%d: %s %5.5d %s\n",i,tcpClients[i].ipAddress, tcpClients[i].port, tcpClients[i].name);
}

/*

    Socket connection utils

*/

int findClientByName(char *name) {
    for(int i = 0; i < nbClients; i++) if(strcmp(tcpClients[i].name,name) == 0) return i;
    return -1;
}

void sendMessageToClient(char *clientip,int clientport,char *mess) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server = gethostbyname(clientip);
    if(server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(clientport);

    if(connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        printf("ERROR connecting\n");
        exit(1);
    }

    sprintf(buffer,"%s\n",mess);
    n = write(sockfd,buffer,strlen(buffer));

    close(sockfd);
}

void broadcastMessage(char *mess) {
    for(int i = 0; i < nbClients; i++) sendMessageToClient(tcpClients[i].ipAddress, tcpClients[i].port, mess);
}

/*

    Main

*/

int main(int argc, char *argv[]) {
	srand(time(NULL)); // initialisation de rand

    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;

    int i, n;

    char com;
    char clientIpAddress[256], clientName[256];
    int clientPort;
    int id;
    char reply[256];

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) error("ERROR on binding");

    listen(sockfd,5);
    clilen = sizeof(cli_addr);

	printDeck();
	melangerDeck();
	createTable();
	printDeck();

	for(i = 0; i < 4; i++){
        strcpy(tcpClients[i].ipAddress,"localhost");
        tcpClients[i].port =- 1;
        strcpy(tcpClients[i].name,"-");
	}

    while (1) {    
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if(newsockfd < 0) error("ERROR on accept");

        bzero(buffer,256);
        n = read(newsockfd,buffer,255);
        if (n < 0) error("ERROR reading from socket");

        printf("Received packet from %s:%d\nData: [%s]\n\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buffer);

        if(fsmServer == 0) {
            switch (buffer[0]) {
                case 'C':
                    sscanf(buffer,"%c %s %d %s", &com, clientIpAddress, &clientPort, clientName);
                    printf("COM=%c ipAddress=%s port=%d name=%s\n",com, clientIpAddress, clientPort, clientName);

                    // fsmServer == 0 alors j'attends les connexions de tous les joueurs
                    strcpy(tcpClients[nbClients].ipAddress,clientIpAddress);
                    tcpClients[nbClients].port = clientPort;
                    
                    strcpy(tcpClients[nbClients].name,clientName);
                    nbClients++;

                    printClients();

				    // rechercher l'id du joueur qui vient de se connecter
                    id = findClientByName(clientName);
                    printf("id = %d\n",id);

				    // lui envoyer un message personnel pour lui communiquer son id
                    sprintf(reply,"I %d",id);
                    sendMessageToClient(tcpClients[id].ipAddress, tcpClients[id].port, reply);

                    // Envoyer un message broadcast pour communiquer a tout le monde la liste des joueurs actuellement
                    // connectes
                    sprintf(reply,"L %s %s %s %s", tcpClients[0].name, tcpClients[1].name, tcpClients[2].name, tcpClients[3].name);
                    broadcastMessage(reply);

                    if(nbClients == 4) {
                        // Send it's deck to each player and the corresponding symbols on his table
                        for(int i = 0; i < 4; i++) {
                            sprintf(reply, "D %d %d %d", deck[0 + 3 * i], deck[1 + 3 * i], deck[2 + 3 * i]);
                            sendMessageToClient(tcpClients[i].ipAddress, tcpClients[i].port, reply);

                            for(int j = 0; j < 8; j ++) {
                                sprintf(reply, "V %d %d %d", i, j, tableCartes[i][j]);
                                sendMessageToClient(tcpClients[i].ipAddress, tcpClients[i].port, reply);
                            }
                        }

                        // On envoie enfin un message a tout le monde pour definir qui est le joueur courant=0
                        // RAJOUTER DU CODE ICI
                        sprintf(reply, "M %d", joueurCourant);
                        broadcastMessage(reply);

                        fsmServer = 1;
                    }

                break;
            }
        } else if(fsmServer == 1) {
            switch(buffer[0]) {
                case 'G':
                    int player, culprit;
					sscanf(buffer, "G %d %d", &player ,&culprit);

                    if(culprit == deck[12]) { // Player found the culprit, he wins
                        sprintf(reply, "W %d %d", player, culprit);
                        broadcastMessage(reply);

                        printf("Le joueur %d a trouvé le coupable !\n", player);
                    } else { // If not, he is eliminated
                        sprintf(reply, "E %d %d", player, culprit);
                        broadcastMessage(reply);

                        nbPlayersRemaining --;
                        eliminated[player] = 1;

                        // Check if there is only one player remaining, if so, he wins
                        if(nbPlayersRemaining == 1) sprintf(reply, "W %d %d", nextPlayer(joueurCourant), deck[12]);
                        else { // Else play next turn as usual
                            joueurCourant = nextPlayer(joueurCourant);
                            sprintf(reply, "M %d", joueurCourant);
                        }
                        
                        broadcastMessage(reply);
                    }

                break;

                case 'O':
                    int j;
                    sscanf(buffer, "O %d %d", &id, &j);

                    for(i = 0; i < 4; i++) {
                        if(i != id) {
                            if(tableCartes[i][j] > 0) sprintf(reply, "V %d %d 100", i, j);
                            else sprintf(reply, "V %d %d 0", i, j);

                            broadcastMessage(reply);
                        }
                    }

                    joueurCourant = nextPlayer(joueurCourant);
                    sprintf(reply, "M %d", joueurCourant);

                    broadcastMessage(reply);
                break;

                case 'S':
                    sscanf(buffer, "S %d %d %d", &id, &i, &j);
                    sprintf(reply, "V %d %d %d", i, j, tableCartes[i][j]);

                    broadcastMessage(reply);

                    joueurCourant = nextPlayer(joueurCourant);
                    sprintf(reply, "M %d", joueurCourant);

                    broadcastMessage(reply);
                break;

                default: break;
            }
        }

        close(newsockfd);
    }

    close(sockfd);
    return 0; 
}