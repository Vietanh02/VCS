#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <crypt.h>
#include <unistd.h>
#define MAX_LEN 500

struct UserInfo {
    char username[MAX_LEN];
    char password[MAX_LEN];  // Note: Store the hashed password, not plaintext, for security
    int UID;
    int GID;
    char homedir[MAX_LEN];
    char shell[MAX_LEN];
};
int CheckPassword(char *passhash);
void ChangePassword(char *oldpass);
int main() {
    FILE *fp,*fout;
    char *line = NULL;  // Adjust the buffer size accordingly
    size_t len = 0;
    size_t read;
    char username[255],password[255];
    struct UserInfo *user[255];
    char *token;
    char *token_pass;
    fp = fopen("/etc/shadow", "r");
    if (fp == NULL) {

        perror("Error opening file");
        return 1;
    }
    int i=0;
    while ((read = getline(&line, &len, fp)) != -1) {
        /*Allocate the memory of user*/
        user[i] = malloc(sizeof(struct UserInfo));
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

        /*separate to get home*/
        token = strtok_r(line, ":", &line);
        strncpy(user[i]->homedir,token,strlen(token));
        user[i]->homedir[strlen(token)] = '\0';
        token = strtok_r(line, ":", &line);
        strncpy(user[i]->shell,token,strlen(token));
        user[i]->shell[strlen(token)] = '\0';
        i++;
    }

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
            while(1==1){
                int check = CheckPassword(user[j]->password);
                if(check ==1){
                    break;
                }else{
                    printf("\n");
                    continue;
                }
            }
            ChangePassword(user[j]->password);
            break;
        }
        
    }

    fout = fopen("/etc/shadow","w");
    char *buffer = malloc(500);
    for(int j=0;j<i;j++){
        memset(buffer,'\0',500);
        sprintf(buffer,"%s:%s:%d:%d:%s:%s:::\n",user[j]->username,user[j]->password,user[j]->UID,user[j]->GID,user[j]->homedir,user[j]->shell);
        fputs(buffer,fout);
    }
    fclose(fout);
    printf("Change password for user: %s success!!\n",user[j]->username);
    return 0;
}

int CheckPassword(char *passhash){
    char *result;
    int check;
    /* Read in the user's password and encrypt it,
        passing the expected password in as the salt. */
    result = crypt(getpass("Type your password: "), passhash);
    /* Test the result. */
    check= (strcmp(result, passhash) == 0);
    puts(check ? "PASSWORD IS CORRECT.\nCHANGE YOUR PASSWORD\n" : "PASSWORD IS INCORRECT.\nTRY AGAIN\n");
    return check ;
}

void ChangePassword(char *oldpass){
    strcpy(oldpass,crypt(getpass("Type your new password: "), oldpass));
}
