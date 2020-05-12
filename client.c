#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

int hash(char *string) {
    if(strcmp(string, "register") == 0) {
        return 0;
    }
    if(strcmp(string, "login") == 0) {
        return 1;
    }
    if(strcmp(string, "enter_library") == 0) {
        return 2;
    }
    if(strcmp(string, "get_books") == 0) {
        return 3;
    }
    if(strcmp(string, "get_book") == 0) {
        return 4;
    }
    if(strcmp(string, "add_book") == 0) {
        return 5;
    }
    if(strcmp(string, "delete_book") == 0) {
        return 6;
    }
    if(strcmp(string, "logout") == 0){
        return 7;
    }

    return -1;
}

int main(int argc, char *argv[]) {

    printf("Welcome!\nUser Rules:\n1.username and password are up to 20 chars.\n2.one book's fields are up to 30 chars.\n3.you can choose between:\nregister, login, entry_level, get_books, get_book, add_book, delete_book, logout, exit.\nChoose wisely!\n\n");

    char *message;
    char *response;
    int sockfd;

    char *command = malloc(20 * sizeof(char));

    int no_fields = 2;
    char* username_buf = malloc(20 * sizeof(char));
    char* password_buf = malloc(20 * sizeof(char));
    char** data_fields = malloc(no_fields* sizeof(char*));
    
    char* token = NULL;
    char** cookies = NULL;

    printf("choose action: ");
    scanf("%s", command);

    while(strcmp(command, "exit") != 0) {
        switch (hash(command))
        {  
            case 0:
                {
                    //task 0: register

                    printf("register\n");

                    //prompt for username&password
                    printf("username=");
                    scanf("%s", username_buf);
                    data_fields[0] = malloc(strlen("\"username\": \"") * sizeof(char));
                    data_fields[1] = malloc(strlen("\"password\": \"") * sizeof(char));
                    memcpy(data_fields[0], "\"username\": \"", strlen("\"username\": \""));
                    strcat(data_fields[0], username_buf);
                    strcat(data_fields[0], "\"");

                    printf("password=");
                    scanf("%s", password_buf);
                    memcpy(data_fields[1], "\"password\": \"", strlen("\"password\": \""));
                    strcat(data_fields[1], password_buf);
                    strcat(data_fields[1], "\"");


                    //server interaction
                    sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
                    message = compute_post_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com", 
                        "/api/v1/tema/auth/register", "application/json", data_fields, no_fields, NULL, 0);
                    puts(message);
                    send_to_server(sockfd, message);
                    response = receive_from_server(sockfd);
                    puts(response);

                    //free & close connection
                    //free(response);
                    close(sockfd);
                }
                break;
            case 1:
               {
                //task1: login 
                    printf("login\n");
                    
                    char **login_fields = malloc(no_fields * sizeof(char*));
                    printf("username=");
                    scanf("%s", username_buf);
                    login_fields[0] = malloc(strlen("\"username\": \"") * sizeof(char));
                    login_fields[1] = malloc(strlen("\"password\": \"") * sizeof(char));
                    memcpy(login_fields[0], "\"username\": \"", strlen("\"username\": \""));
                    strcat(login_fields[0], username_buf);
                    strcat(login_fields[0], "\"");
                    
                    printf("password=");
                    scanf("%s", password_buf);
                    memcpy(login_fields[1], "\"password\": \"", strlen("\"password\": \""));
                    strcat(login_fields[1], password_buf);
                    strcat(login_fields[1], "\"");

                    //server interaction
                    sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
                    message = compute_post_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com", 
                        "/api/v1/tema/auth/login", "application/json", login_fields, no_fields, NULL, 0);
                    puts(message);
                    send_to_server(sockfd, message);
                    response = receive_from_server(sockfd);
                    puts(response);

                    //save cookies!
                    cookies = malloc(100 * sizeof(char*));
                    for(int i = 0; i < 100; i++) {
                        cookies[i] = malloc(150 * sizeof(char));
                    }
                    char* cookie_start = strstr(response, "Set-Cookie: ");
                    if(cookie_start != NULL) {
                        cookie_start = cookie_start + strlen("Set-Cookie: ");
                        strncpy(cookies[0], cookie_start, 100);
                        int i = 0;
                        int length = 0;
                        while (1) {
                            if (cookies[0][i] != ';') {
                                length++;
                            } else {
                                break;
                            }
                            i++;
                        }
                        memset(cookies[0] + length, 0, 100 - length);
                    }
                    
                    //free & close connection
                    //free(response);
                    close(sockfd);                
               }
               break;
            case 2:
               {
                //task 2: enter_library
                    if(cookies == NULL) {
                        printf("Please log in first (login)\n");
                    } else {
                        printf("enter_library\n");

                        //server interaction
                        sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
                        message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com", 
                            "/api/v1/tema/library/access", NULL, cookies, 1);
                        puts(message);
                        send_to_server(sockfd, message);
                        response = receive_from_server(sockfd);
                        puts(response);
                        
                        //save token!
                        //save json
                        char* json = basic_extract_json_response(response);
                        char* token_start = strstr(json, ":");
                        token = malloc(500 * sizeof(char));
                        if(token_start != NULL) {
                            token_start = token_start + 2;

                            strncpy(token, token_start, 500);
                            int i = 0;
                            int length = 0;
                            while (1) {
                                if (token[i] != '}') {
                                    length++;
                                } else {
                                    break;
                                }
                                i++;
                            }
                            length--; //no quotation marks
                            memset(token + length, 0, 500 - length);
                        }
                        close(sockfd);
                    }
               }
               break;
            case 3:
               {
                //task3: get_books
                    if(cookies == NULL) {
                        printf("Please log in first (login)\n");
                    } else {                
                        if(token == NULL){
                            printf("Please get access first (enter_library)\n");
                        } else {
                            printf("\nget_books\n");

                            //server interaction
                            sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
                            message = compute_getbooks_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com", 
                                "/api/v1/tema/library/books", NULL, cookies, 1, token);

                            puts(message);
                            send_to_server(sockfd, message);
                            response = receive_from_server(sockfd);
                            puts(response);

                            close(sockfd); 
                        }
                    }
               }
               break;
            case 4:
               {
                //task4: get_book (by id)
                    if(cookies == NULL) {
                        printf("Please log in first (login)\n");
                    } else { 
                        if(token == NULL){
                            printf("Please get access first (enter_library)\n");
                        } else {
                            int id;
                            printf("\nget_book\n");
                            printf("id=");
                            scanf("%d", &id);

                            //server interaction
                            sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
                            message = compute_get_one_book_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com", 
                                "/api/v1/tema/library/books/", NULL, cookies, 1, token, id);
                            puts(message);
                            send_to_server(sockfd, message);
                            response = receive_from_server(sockfd);
                            puts(response);

                            close(sockfd);
                        }
                    }
               }
               break;
            case 5:
               {
                //task5: add_book
                    if(cookies == NULL) {
                        printf("Please log in first (login)\n");
                    } else { 
                        if(token == NULL){
                            printf("Please get access first (enter_library)\n");
                        } else {
                            printf("\nadd_book\n");
                            char temp;
                            int no_jfields = 5;
                            char** json_fields = malloc(no_jfields * sizeof(char*));
                            char* title_buf = malloc(30 * sizeof(char));
                            char* author_buf = malloc(30 * sizeof(char));
                            char* genre_buf = malloc(30 * sizeof(char));
                            char* publisher_buf = malloc(30 * sizeof(char));
                            char* page_count = malloc(30 * sizeof(char));

                            printf("title=");
                            scanf("%c",&temp);
                            fgets(title_buf,30,stdin);

                            printf("author=");
                            fgets(author_buf,30,stdin);
                           
                            printf("genre=");
                            fgets(genre_buf,30,stdin);
                            
                            printf("publisher=");
                            fgets(publisher_buf,30,stdin);
                            
                            printf("page_count=");
                            fgets(page_count,30,stdin);
                            

                            json_fields[0] = malloc(strlen("\"title\": \"")*sizeof(char));
                            json_fields[1] = malloc(strlen("\"author\": \"")*sizeof(char)); 
                            json_fields[2] = malloc(strlen("\"genre\": \"")*sizeof(char));
                            json_fields[3] = malloc(strlen("\"page_count\": \"")*sizeof(char));  
                            json_fields[4] = malloc(strlen("\"publisher\": \"")*sizeof(char));
                            
                            strcpy(json_fields[0], "\"title\": \"");    
                            strncat(json_fields[0], title_buf, strlen(title_buf) - 1);
                            strcat(json_fields[0], "\"");

                            strcpy(json_fields[1], "\"author\": \"");
                            strncat(json_fields[1], author_buf, strlen(author_buf) - 1);
                            strcat(json_fields[1], "\"");

                            strcpy(json_fields[2], "\"genre\": \"");
                            strncat(json_fields[2], genre_buf, strlen(genre_buf) - 1); 
                            strcat(json_fields[2], "\""); 

                            strcpy(json_fields[3], "\"page_count\": \"");
                            strncat(json_fields[3], page_count, strlen(page_count) - 1);
                            strcat(json_fields[3], "\"");

                            strcpy(json_fields[4], "\"publisher\": \"");
                            strncat(json_fields[4], publisher_buf, strlen(publisher_buf) - 1);
                            strcat(json_fields[4], "\"");

                            //server interaction
                            sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
                            message = compute_postbook_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com", 
                                "/api/v1/tema/library/books", "application/json", json_fields, no_jfields, cookies, 1, token);
                            puts(message);
                            send_to_server(sockfd, message);
                            response = receive_from_server(sockfd);
                            puts(response);

                            close(sockfd);
                        }
                    }
               }
               break;
            case 6:
               {
                //task6: delete_book
                    if(cookies == NULL) {
                        printf("Please log in first (login)\n");
                    } else {                 
                        if(token == NULL){
                            printf("Please get access first (enter_library)\n");
                        } else {
                            int id = 0;
                            printf("\ndelete_book\n");
                            printf("id=");
                            scanf("%d", &id);

                            //server interaction
                            sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
                            message = compute_delete_book_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com", 
                                "/api/v1/tema/library/books", NULL, cookies, 1, token, id);
                            puts(message);
                            send_to_server(sockfd, message);
                            response = receive_from_server(sockfd);
                            puts(response);

                            close(sockfd);                        
                        }
                    }
               }
               break;
            case 7:
               {
                //task7: logout
                    if(cookies == NULL) {
                        printf("Please log in first (login)\n");
                    } else { 
                        printf("logout\n");

                        //server interaction
                        sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
                        message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com", 
                            "/api/v1/tema/auth/logout", NULL, cookies, 1);
                        puts(message);
                        send_to_server(sockfd, message);
                        free(message);
                        response = receive_from_server(sockfd);
                        puts(response);
                        free(response);

                        free(cookies);
                        cookies = NULL;
                        free(token);
                        token = NULL;
                    }
               }
               break;
            default:
               printf("no such command available\n");
        }

        printf("choose action: ");
        scanf("%s", command);        
    }

    //task8: exit
    printf("exit\n");
    exit(0);


    return 0;
}
