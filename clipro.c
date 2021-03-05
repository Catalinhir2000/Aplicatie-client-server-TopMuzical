#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

// codul de eroare reuturnat de anumite apeluri
extern int errno;


void list()
{
    FILE * fp;
    fp = fopen ("title.txt", "r");
    if (fp == NULL)
    {
        perror("unable to opent the title.txt file\n");
    }
    
    char lista[128];
    int i = 1;
    while (fgets(lista, sizeof(lista), fp) != NULL)
    {
        if (i % 2 == 1)
        {
            lista[strlen(lista) - 1] = '\0';
        }
        
        printf("%s", lista);
        i++;
    }
    fclose(fp);
}




void listtop()
{
    FILE * fp;
    fp = fopen ("top.txt", "r");
    if (fp == NULL)
    {
        perror("unable to opent the title.txt file\n");
    }
    
    char lista[128];
    int i = 1;
    while (fgets(lista, sizeof(lista), fp) != NULL)
    {
        if (i % 2 == 1)
        {
            lista[strlen(lista) - 1] = '\0';
        }
        
        printf("%s ", lista);
        i++;
    }
    fclose(fp);
}



//portul de conectare la server;
int port;
int quit = 1;
int main(int argc, char *argv[])
{
    int sd; //socket descriptor
    struct sockaddr_in server; //structura folosita pentru coenctarea la sever
    char msg[100]; //mesajul trimis
    int role = 0;

    //exista toate argumentele in linia de comanda?
    if (argc != 3)
    {
        printf("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
        return -1;
    }

    //stabilim portul
    port = atoi (argv[2]);

    //cream socketul
    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror ("[CLIENT]Eroare la crearea socketului");
        return errno;
    }

    //umplem structura folosita pentru realizarea conexiunii cu serverul
    //familia socketului
    server.sin_family = AF_INET;
    //adresa ip a serverului
    server.sin_addr.s_addr - inet_addr(argv[1]);
    //portul de conectare
    server.sin_port = htons (port);

    //ne conectam la server
    if (connect (sd, (struct sockaddr *) &server, sizeof(struct sockaddr)) == -1)
    {
        perror("[CLIENT]Eroare la connect.\n");
        return errno;
    }
        int idc = -1; //id ul comenzii
        int idmusic;
        while (idc != 0)
        {
        idc = -1;
        if (role == 0)
        {
            printf("Comenzi disponibile:\n");
            printf("login\n");
            printf("quit\n");
        }
        if (role == 1)
        {
            printf("Comenzi disponibile:\n");
            printf("login\n");
            printf("quit\n");
            printf("list\n");
            printf("vote\n");
            printf("about\n");
            printf("link\n");
            printf("top\n");
        }
        if (role == 2)
        {
            printf("Comenzi disponibile:\n");
            printf("login\n");
            printf("quit\n");
            printf("list\n");
            printf("vote\n");
            printf("about\n");
            printf("link\n");
            printf("top\n");
        }
        
        
        //citirea mesajului
        bzero (msg, 100);
        printf("[CLIENT]Introduceti o comanda:\n ");
        fflush(stdout);
        read (0, msg, 100);
        msg[strlen(msg) -1] = '\0';
        
        
        if (strcmp(msg, "quit") == 0)
        {
            idc = 0;
        }
        if (strcmp(msg, "login") == 0)
        {
            idc = 1;
        }
        if (strcmp(msg, "list") == 0)
        {
            idc = 2;
        }
        if (strcmp(msg, "vote") == 0)
        {
            idc = 3;
        }
        if (strcmp(msg, "about") == 0)
        {
            idc = 4;
        }
        if (strcmp(msg, "link") == 0)
        {
            idc = 5;
        }
        if (strcmp(msg, "top") == 0)
        {
            idc = 6;
        }
        if (strcmp(msg, "topgen") == 0)
        {
            idc == 7;
        }
        
        
        
        
        
        
        //trimitrea mesajului la server
        if (write (sd, &idc, sizeof(idc)) <= 0)
        {
            perror ("[CLIENT]Eroare la write pre server.\n");
            return errno;
        }
        if (read (sd, &idc, sizeof(int)) < 0)
        {
            perror("[CLIENT]Eroare la read de la server.\n");
            return errno;
        }
        printf("id ul primit de la server este %d\n", idc);

        

        
        

        
        
        //login
        if (idc == 1)
        {
            printf("dati username si parola\n");
            read (0, msg, 100);
            printf("usernameul citit este:%s \n", msg);
            if (write (sd, msg, 100) <= 0) //transmitem serverului user ul de la tastatura
            {
            perror ("[CLIENT]Eroare la write spre server.\n");
            return errno;
            }
            if (read (sd, &role, sizeof(int)) < 0)
            {
                perror("[CLIENT]Eroare la read de la server.\n");
                return errno;
            }
            if (role == 1)
            {
                printf("Utilizator conectat ca user\n");
            }
            if (role == 2)
            {
                printf("Utilizator conectat ca admin\n");
            }
            if (role == 0)
            {
                printf("Utilizator neconectat, eroare la credentiale\n");
            }
        }//end login

        //list
        if (idc == 2 && (role == 1 || role ==2)) 
        {
            list();
            printf("\n");
        }
        
        
        //vote
        if (idc == 3 && (role == 1 || role ==2))
        {
            printf("dati id-ul piesei pe care doriti sa o votati\n");
            scanf ("%d", &idmusic);
            if (write (sd, &idmusic, sizeof(int)) <= 0) //transmitem serverului id-ul piesei de la tastatura
            {
                perror ("[CLIENT]Eroare la write spre server.\n");
                return errno;
            }
            if (read (sd, msg, 100) < 0)
            {
                perror("[CLIENT]Eroare la read de la server.\n");
                return errno;
            }
            printf("%s %d \n", msg, idmusic);
        }
        
        //about
        if (idc == 4 && (role == 1 || role ==2))
        {
            printf("dati id-ul piesei careia doriti sa ii vedeti descrierea\n");
            scanf ("%d", &idmusic);
            if (write (sd, &idmusic, sizeof(int)) <= 0) //transmitem serverului id-ul piesei de la tastatura
            {
                perror ("[CLIENT]Eroare la write spre server.\n");
                return errno;
            }
            if (read (sd, msg, 100) < 0)
            {
                perror("[CLIENT]Eroare la read de la server.\n");
                return errno;
            }
            printf("%s \n", msg);
        }

        //link
        if (idc == 5 && (role == 1 || role ==2))
        {
            printf("dati id-ul piesei careia doriti sa ii vedeti linkul\n");
            scanf ("%d", &idmusic);
            if (write (sd, &idmusic, sizeof(int)) <= 0) //transmitem serverului id-ul piesei de la tastatura
            {
                perror ("[CLIENT]Eroare la write spre server.\n");
                return errno;
            }
            if (read (sd, msg, 100) < 0)
            {
                perror("[CLIENT]Eroare la read de la server.\n");
                return errno;
            }
            printf("%s \n", msg);
        }
        
        //top
        if (idc == 6 && (role == 1 || role ==2))
        {
            if (read (sd, msg, 100) < 0)
            {
                perror("[CLIENT]Eroare la read de la server.\n");
                return errno;
            }
            printf("%s \n", msg);
            listtop();
            printf("\n");
        }


        if (idc == 7 && (role == 1 || role ==2))
        {
            printf("dati genul pentru care doriti sa vedeti un top\n");
            read (0, msg, 100);
            if (write (sd, msg, 100) <= 0) //transmitem serverului user ul de la tastatura
            {
            perror ("[CLIENT]Eroare la write spre server.\n");
            return errno;
            }
        }
        
        


        
        

        if (idc == -1)
        {
            printf("comanda gresita\n");
        }
        
        }
    //incheiem conexiunea cu serverul
    close(sd);
    
    
    
    
}