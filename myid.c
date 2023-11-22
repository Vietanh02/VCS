#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 500

struct UserInfo {
    char username[MAX_LEN];
    char password[MAX_LEN];  // Note: Store the hashed password, not plaintext, for security
    int UID;
    int GID;
    char homedir[MAX_LEN];
    char shell[MAX_LEN];
};
struct GroupInfo{
    char groupname[MAX_LEN];
    char password[MAX_LEN];
    int GID;
    char user[MAX_LEN];

};

int main() {
    FILE *fp,*groupfile;
    char *line = NULL;  // Adjust the buffer size accordingly
    size_t len = 0;
    size_t read;
    char username[255],password[255];
    struct UserInfo *user[255];
    struct GroupInfo *group[255];
    char *token;

    /*reading passwd file*/
    fp = fopen("/etc/passwd", "r");
    if (fp == NULL) {
        perror("Error opening file");
        return 1;
    }
    int i=0;
    while ((read = getline(&line, &len, fp)) != -1) {
        /*Allocate the memory of user*/
        user[i] = malloc(sizeof(struct UserInfo));
        if (user[i] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        /*end of allocate*/
        
        /*separate line to get username*/
        token = strtok_r(line, ":", &line);
        strncpy(user[i]->username,token,strlen(token));
        user[i]->username[strlen(token)] = '\0';  // Ensure null-terminate

        /*separate to get password*/
        token = strtok_r(line, ":", &line);
        strncpy(user[i]->password,token,strlen(token));
        user[i]->password[strlen(token)] = '\0';
        
        /*separate to get UID*/
        token = strtok_r(line, ":", &line);
        user[i]->UID = atoi(token);

        /*separate to get GID*/
        token = strtok_r(line, ":", &line);
        user[i]->GID = atoi(token);

        /*separated to get addition information*/
        token = strtok_r(line, ":", &line);

        /*separate to get home*/
        token = strtok_r(line, ":", &line);
        strncpy(user[i]->homedir,token,strlen(token));
        user[i]->homedir[strlen(token)] = '\0';

        /*separated to get shell*/
        strncpy(user[i]->shell,line,strlen(line));
        user[i]->shell[strlen(line)] = '\0';

        i++;
    }
    //-----------------------------------------------------

    /*reading passwd file*/
    groupfile = fopen("/etc/group", "r");
    if (groupfile == NULL) {
        perror("Error opening file");
        return 1;
    }
    int iGID=0;
    while ((read = getline(&line, &len, groupfile)) != -1) {
        /*Allocate the memory of user*/
        group[iGID] = malloc(sizeof(struct GroupInfo));
        if (group[iGID] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        /*end of allocate*/
        
        /*separate line to get username*/
        token = strtok_r(line, ":", &line);
        strncpy(group[iGID]->groupname,token,strlen(token));
        group[iGID]->groupname[strlen(token)] = '\0';  // Ensure null-terminate

        /*separate to get password*/
        token = strtok_r(line, ":", &line);
        strncpy(group[iGID]->password,token,strlen(token));
        group[iGID]->password[strlen(token)] = '\0';

        /*separate to get GID*/
        token = strtok_r(line, ":", &line);
        group[iGID]->GID = atoi(token);

        /*separated to get shell*/
        strncpy(group[iGID]->user,line,strlen(line));
        group[iGID]->user[strlen(line)] = '\0';

        iGID++;
    }
    //-----------------------------------------------------


    int j=0;
    while (1==1)
    {
        printf("Enter your username: ");
        scanf("%s",username);
        j=0;
        for(j=0;j<i;j++){
            if(strcmp(user[j]->username,username)!=0){
                continue;
            }else{
                break;
            }
        }
        if(j==i){
            printf("No user found\n");
            continue;
        }else{
            printf("User found!\n");
            printf("Username: %s\nUser ID: %d\nHome dir: %s\nUser belong to group:",user[j]->username,user[j]->UID,user[j]->homedir);
            for(int k=0;k<iGID;k++){
                if(strstr(group[k]->user,user[j]->username)!=NULL){
                    printf(" %s",group[k]->groupname);
                }
            }
            printf("\n");
            break;
        }
        
    }

    return 0;
}

