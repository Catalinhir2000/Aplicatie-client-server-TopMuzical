#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

// portul folosit
#define PORT 9999

//codul de eroare returnat de anumite apeluri
extern int errno;

int forky = 1;

int rol = 0;
int autorizare = 0;
int gasit = 0;


//functia de login
int login(char* uid)
{
    FILE * fp;
    fp = fopen("username.txt", "r");
    if (fp == NULL)
    {
        perror("unable to open the file");
        return errno;
    }
    char verifid[128];
    while (fgets(verifid, sizeof(verifid), fp) != NULL && gasit == 0) // sizeof/strlen
    {
        
        if (strcmp(uid, verifid) == 0)
        {
            gasit = 1;
        }
        verifid[strlen(verifid) - 1] = '\0';
        if (strcmp("utilizatori:", verifid) == 0)
        {
            rol = 1;
        }
        if (strcmp("administratori:", verifid) == 0)
        {
            rol = 2;
        }
        
    }

    if (gasit == 1 && rol == 1)
    {
        autorizare = 1;
    }
    if (gasit == 1 && rol == 2)
    {
        autorizare = 2;
    }
    
    fclose(fp);
    return autorizare;
}



//nrvotes
int vot[100] = {0};
int lung = 0;
void nrvotes()
{
    int nri;
    char nr[128];
    FILE * fp;
    fp = fopen("nrvot.txt", "r");
    if (fp == NULL)
    {
        perror("unable to open the file");
    }
    int i = 1;
    int j = 1;
    while (fgets(nr, sizeof(nr), fp) != NULL)
    {
        nri = atoi(nr);
        if (i % 2 == 0)
        {
            vot[j] = nri;
            j++;
            lung++;
        }
        
        i++;
    }
    fclose(fp);
}


//vote
void vote(int idmuzica)
{
    nrvotes();
    FILE * fp;
    fp = fopen("nrvot.txt", "w");
    if (fp == NULL)
    {
        perror("unable to open the file");
    }
    for (int i = 1; i <= lung; i++)
    {
        if (i == idmuzica)
        {
            fprintf(fp, "%d\n%d\n", i, vot[i] + 1);
        }
        else
        {
            fprintf(fp, "%d\n%d\n", i, vot[i]);
        }
        
        
    }

    fclose(fp);
    


}





//despre
char rez[128];
void despre(int idmuzica)
{
    FILE * fp;
    fp = fopen("descriere.txt", "r");
    if (fp == NULL)
    {
        perror("unable to open the file");
    }
    char desc[128];
    int i = 1;
    while (fgets(desc, sizeof(desc), fp) != NULL) // sizeof/strlen
    {
        
        if (i % 2 == 0)
        {
            
            if (i / 2 == idmuzica)
            {
                printf("%d\n%d", idmuzica, i);
                printf("%s\n", desc);
                bzero(rez, 100);
                strcat(rez, desc);
            }
            
        }
        i++;
        
    }
    fclose(fp);
}



//link
char lnk[128];
void linkul(int idmuzica)
{
    FILE * fp;
    fp = fopen("link.txt", "r");
    if (fp == NULL)
    {
        perror("unable to open the file");
    }
    char desc[128];
    int i = 1;
    while (fgets(desc, sizeof(desc), fp) != NULL) // sizeof/strlen
    {
        
        if (i % 2 == 0)
        {
            
            if (i / 2 == idmuzica)
            {
                printf("%d\n%d", idmuzica, i);
                printf("%s\n", desc);
                bzero(lnk, 100);
                strcat(lnk, desc);
            }
            
        }
        i++;
        
    }
    fclose(fp);
}
int vect[100] = {0};
void vectorder()
{
    nrvotes();
    int i;
    int j;
    int aux;

    for (i = 1; i <= lung; i++)
    {
        vect[i] = vot[i];
    }
    
    for (i = 1; i < lung; i++)
    {
        for (j = i + 1; j <= lung; j++)
        {
            if (vot[i] >= vot[j])
            {
                aux = vot[i];
                vot[i] = vot[j];
                vot[j] = aux;
            }
            
        }
        
    }
    
    

}


int check[100] = {0};
void top()
{

    vectorder();
    int i = 1;
    FILE * fp;
    fp = fopen("top.txt", "w");
    if (fp == NULL)
    {
        perror("unable to open the file");
    }
    for (int k = 1; k <= lung; k++)
    {
        int afisare = 0;
        for ( i = 1; i <= lung && afisare == 0; i++)
        {
            
            if ( vect[i] == vot[k] && check[i] == 0 )
            {
                check [i] = 1;
                afisare = 1;
                fprintf(fp, "%d\n%d\n", i, vot[k]);
            }
            
        }
        
        
    }

    fclose(fp);
    
}






void topgen(int idmuzica)
{
    
}







int main()
{
    struct sockaddr_in server; //structura folosita de server
    struct sockaddr_in from;
    int idc;
    int idmusic;
    int idpiesa;
    char msg[100]; //mesajul primit de la client
    char msgrasp[100] = " "; //mesajul de raspuns pentru client;
    int sd; //socket descriptorul
    

    //crearea socketului
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("[SERVER]Eroare la crearea socketului.\n");
        return errno;
    }

    //pregatirea structurilor de date
    bzero(&server, sizeof(server));
    bzero(&from, sizeof(from));

    //umplem structura folosita de server
    //stabilirea familiei de socketuri
    server.sin_family = AF_INET;
    //acceptam orice adresa
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    //utilizam un port de utilizator
    server.sin_port = htons(PORT);

    //atasam socketul
    if (bind(sd, (struct sockaddr *) &server, sizeof(struct sockaddr)) == -1)
    {
        perror("[SERVER]Eroare la bind.\n");
        return errno;
    }

    //punem serverul sa asculte daca vin clienti sa se conecteze
    if (listen (sd, 1) == -1)
    {
        perror ("[SERVER]Eroare la listen.\n");
        return errno;
    }
    

    //servim in mod concurent clientii...
    
    while(1)
    {
        int client;
        int length = sizeof (from);

        printf("[SERVER]Asteptam la portul %d...\n", PORT);
        fflush (stdout);

        //acceptam un client (stare blocanta pana la realizarea conexiunii)
        client = accept (sd, (struct sockaddr *) &from, &length);

        //eroare la acceptarea conexiunii de la un client
        if (client < 0)
        {
            perror("[SERVER]Eroare la accept.\n");
            continue;
        }
        
        if(forky == 1)
        {
            int pid;
            pid = fork();
            if (pid == -1)
            {
                perror("eroare la fork");
                close(client);
                continue;            
            }
            else if (pid > 0) //parinte
            {
                close(client);
                while (waitpid(-1, NULL, WNOHANG)); 
                continue;
            }
            else if (pid == 0) //copil
            {
                while (1)
                {
                    close(sd);
                    int role = 0;
                    //s-a realizat conexiunea se asteapta comanda
                    bzero(msg, 100);
                    printf("[SERVER]Asteptam comanda...\n");
                    fflush(stdout);

                    //citim comanda
                    if (read(client, &idc, sizeof(int)) <= 0)
                    {
                        perror("[SERVER]Eroare la read de la client.\n");
                        close(client); //inchidem conexiunea cu clientul
                        continue; //continuam sa ascultam
                    }
                    
                    
                    
                    
                    
                    
                    //quit
                    if (idc == 0)
                    {
                        forky = 0;
                        close(client);
                        exit(0);
                    }
                    
                    
                    
                    
                    //login
                    if (idc == 1)
                    {
                        printf("comanda primita este login\n");
                        if (write(client, &idc, sizeof(idc)) <= 0)
                        {
                            perror("[SERVER]Eroare la write catre client,\n");
                            continue; //continuam sa ascultam
                        }
                        else
                        {
                            printf("[SERVER]Mesajul a fost transims cu succes.\n");
                        }
                        
                        //citeste user
                        if (read(client, msg, 100) <= 0)
                        {
                            perror("[SERVER]Eroare la read de la client.\n");
                            close(client); //inchidem conexiunea cu clientul
                            continue; //continuam sa ascultam
                        }
                        printf("usernameul primit este : %s\n", msg);
                        
                        
                        //user
                        if (login(msg) == 1)
                        {
                            printf("utilizator conectat ca user\n");
                            role = 1;
                            if (write(client, &role, sizeof(int)) <= 0)
                            {
                                perror("[SERVER]Eroare la write catre client,\n");
                                continue; //continuam sa ascultam
                            }
                            else
                            {
                                printf("[SERVER]Mesajul a fost transims cu succes.\n");
                            }
                        }


                        //admin
                        if (login(msg) == 2)
                        {
                            printf("utilizator conectat ca admin\n");
                            role = 2;
                            if (write(client, &role, sizeof(int)) <= 0)
                            {
                                perror("[SERVER]Eroare la write catre client\n");
                                continue;
                            }
                            else
                            {
                                printf("[SERVER]Mesajul a fost transmis cu succes\n");
                            }
                            
                        }


                        //conectare esuata
                        if (login(msg) == 0)
                        {
                            printf("utilizator neconectat, credentiale gresite\n");
                            role = 0;
                            if (write(client, &role, sizeof(int)) <= 0)
                            {
                                perror("[SERVER]Eroare la write catre client\n");
                                continue;
                            }
                            else
                            {
                                printf("[SERVER]Mesajul a fost transmis cu succes\n");
                            }
                            
                            
                        }
                        
                        
                        
                        
                      
                    }//end login
                    

                    //list
                    if (idc == 2)
                    {
                        printf("comanda primita este list\n");
                        if (write(client, &idc, sizeof(idc)) <= 0)
                        {
                            perror("[SERVER]Eroare la write catre client,\n");
                            continue; //continuam sa ascultam
                        }
                        else
                        {
                            printf("[SERVER]Mesajul a fost transims cu succes.\n");
                        }
                        
                    }

                    //vote
                    if (idc == 3)
                    {
                        printf("comanda primita este vote\n");
                        if (write(client, &idc, sizeof(idc)) <= 0)
                        {
                            perror("[SERVER]Eroare la write catre client,\n");
                            continue; //continuam sa ascultam
                        }
                        else
                        {
                            printf("[SERVER]Mesajul a fost transims cu succes.\n");
                        }
                        //citeste id piesa
                        if (read(client, &idmusic, sizeof(int)) <= 0)
                        {
                            perror("[SERVER]Eroare la read de la client.\n");
                            close(client); //inchidem conexiunea cu clientul
                            continue; //continuam sa ascultam
                        }
                        printf("id-ul piesei este : %d\n", idmusic);
                        vote(idmusic);
                        
                        bzero(msgrasp, 100);
                        strcat(msgrasp, "Ati votat cu succes piesa cu id-ul:");
                        if (write(client, msgrasp, 100) <= 0)
                        {
                            perror("[SERVER]Eroare la write catre client,\n");
                            continue; //continuam sa ascultam
                        }
                        else
                        {
                            printf("[SERVER]Mesajul a fost transims cu succes.\n");
                        }
                    }
                    
                    


                    //about
                    if (idc == 4)
                    {
                        printf("comanda primita este about\n");
                        if (write(client, &idc, sizeof(idc)) <= 0)
                        {
                            perror("[SERVER]Eroare la write catre client,\n");
                            continue; //continuam sa ascultam
                        }
                        else
                        {
                            printf("[SERVER]Mesajul a fost transims cu succes.\n");
                        }
                        //citeste id piesa
                        if (read(client, &idmusic, sizeof(int)) <= 0)
                        {
                            perror("[SERVER]Eroare la read de la client.\n");
                            close(client); //inchidem conexiunea cu clientul
                            continue; //continuam sa ascultam
                        }
                        printf("id-ul piesei este : %d\n", idmusic);

                        despre(idmusic);
                        
                        if (write(client, rez, 100) <= 0)
                        {
                            perror("[SERVER]Eroare la write catre client,\n");
                            continue; //continuam sa ascultam
                        }
                        else
                        {
                            printf("[SERVER]Mesajul a fost transims cu succes.\n");
                        }

                    }





                    //link
                    if (idc == 5)
                    {
                        printf("comanda primita este about\n");
                        if (write(client, &idc, sizeof(idc)) <= 0)
                        {
                            perror("[SERVER]Eroare la write catre client,\n");
                            continue; //continuam sa ascultam
                        }
                        else
                        {
                            printf("[SERVER]Mesajul a fost transims cu succes.\n");
                        }
                        //citeste id piesa
                        if (read(client, &idmusic, sizeof(int)) <= 0)
                        {
                            perror("[SERVER]Eroare la read de la client.\n");
                            close(client); //inchidem conexiunea cu clientul
                            continue; //continuam sa ascultam
                        }
                        printf("id-ul piesei este : %d\n", idmusic);

                        linkul(idmusic);
                        
                        if (write(client, lnk, 100) <= 0)
                        {
                            perror("[SERVER]Eroare la write catre client,\n");
                            continue; //continuam sa ascultam
                        }
                        else
                        {
                            printf("[SERVER]Mesajul a fost transims cu succes.\n");
                        }

                    }



                    //top
                    if (idc == 6)
                    {
                        
                        printf("comanda primita este top\n");
                        if (write(client, &idc, sizeof(idc)) <= 0)
                        {
                            perror("[SERVER]Eroare la write catre client,\n");
                            continue; //continuam sa ascultam
                        }
                        else
                        {
                            printf("[SERVER]Mesajul a fost transims cu succes.\n");
                        }
                        top();
                        bzero(msgrasp, 100);
                        strcat(msgrasp, "topul a fost actualizat:");
                        if (write(client, msgrasp, 100) <= 0)
                        {
                            perror("[SERVER]Eroare la write catre client,\n");
                            continue; //continuam sa ascultam
                        }
                        else
                        {
                            printf("[SERVER]Mesajul a fost transims cu succes.\n");
                        }


                    }


                    if (idc == 7)
                    {
                        printf("comanda primita este topgen\n");
                        if (write(client, &idc, sizeof(idc)) <= 0)
                        {
                            perror("[SERVER]Eroare la write catre client,\n");
                            continue; //continuam sa ascultam
                        }
                        else
                        {
                            printf("[SERVER]Mesajul a fost transims cu succes.\n");
                        }
                        if (read(client, msg, 100) <= 0)
                        {
                            perror("[SERVER]Eroare la read de la client.\n");
                            close(client); //inchidem conexiunea cu clientul
                            continue; //continuam sa ascultam
                        }
                        printf("usernameul primit este : %s\n", msg);
                        //apelam topgen(idmusic);

                        
                    }
                    if (idc == -1)
                    {
                        printf("comanda primita este gresita\n");
                        if (write(client, &idc, sizeof(idc)) <= 0)
                        {
                            perror("[SERVER]Eroare la write catre client,\n");
                            continue; //continuam sa ascultam
                        }
                        else
                        {
                            printf("[SERVER]Mesajul a fost transims cu succes.\n");
                        }
                    }
                    
                    
                    
                    
                    
                    
            
                }
                exit(0);

            }// copil

        }//if forky
        
        
        
     }//while
    
    
    
} // main