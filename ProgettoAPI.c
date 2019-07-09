#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 20

int main(){

    char id_orig[MAX]="";
    char id_dest[MAX]="";
    char id_rel[MAX]="";


    char id_ent[MAX]="";
    int i,j;
    int cont,k,f;

    //Dichiarazione Variabili
    char c[MAX];
    char *buffer ;

    while(true)
    {
        //User input
        gets(c);
        buffer=(char*)malloc(strlen(c)*sizeof(char));
        assert(buffer!=NULL);
        strcpy(buffer,c);


        //ADDENT
        if (strncmp(buffer,"addent",6) == 0)
        {
            i=0,j=0;
            for(i=7;i<strlen(buffer);i++)
            {
                if(buffer[i]!= 34)
                {
                    id_ent[j]=buffer[i];
                    j++;
                }

            }
            id_ent[j]=0;
        }

        //DELENT
        if (strncmp(buffer,"delent",6) == 0)
        {

                i=0,j=0;
                for(i=7;i<strlen(buffer);i++)
                {
                if(buffer[i]!= 34)
                {
                    id_ent[j]=buffer[i];
                    j++;
                }

                }
                id_ent[j]=0;

        }

        //ADDREL
        if (strncmp(buffer,"addrel",6) == 0)
          {

            i=0,j=0;
            cont=0,k=0,f=0;


            for(i=7;i<strlen(buffer);i++)
            {

                if(buffer[i]!= 34&&cont==1)
                {
                    id_orig[j]=c[i];
                    j++;
                }
                if(buffer[i]==34)
                    cont++;
                if(buffer[i]!= 34&&cont==3)
                {
                    id_dest[k]=buffer[i];
                    k++;
                }

                if(buffer[i]!= 34&&cont==5)
                {
                    id_rel[f]=buffer[i];
                    f++;
                }
            }


            id_orig[j]=0;
            id_dest[k]=0;
            id_rel[f]=0;

        }


        //DELREL
        if (strncmp(buffer,"delrel",6) == 0)
         {
            i=0,j=0;
            cont=0,k=0,f=0;
            for(i=7;i<strlen(buffer);i++)
            {
                if(buffer[i]!= 34&&cont==1)
                {
                    id_orig[j]=buffer[i];
                    j++;
                }
                if(buffer[i]==34)
                    cont++;
                if(buffer[i]!= 34&&cont==3)
                {
                    id_dest[k]=c[i];
                    k++;
                }

                if(buffer[i]!= 34&&cont==5)
                {
                    id_rel[f]=buffer[i];
                    f++;
                }
            }
            id_orig[j]=0;
            id_dest[k]=0;
            id_rel[f]=0;

        }



        //REPORT
        if(strcmp(buffer,"report")==0)
        {
            if(strlen(id_rel)!=0)
            {
                printf("\"%s\" \"%s\";\n",id_rel,id_dest);

            }
            else
                printf("none\n");
        }




        //END***
        if (strcmp(buffer,"end") == 0)
            break;

    }

    //Dealloco spazio risevato a buffer
    free(buffer);

}
