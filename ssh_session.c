#include <stdio.h>
#include <stdlib.h>
#include <libssh/libssh.h>
#include <signal.h>

#define LIBSSH_STATIC 1//Static linking

//Static linking is the process of copying all library modules used in the program into the final executable image

ssh_session my_ssh_session;

//Signal handler function
void test()
{
    printf("Quitting\n");
}


// void options_setting()
// {
//     //First SSH session
//     my_ssh_session = ssh_new();
//     int protocol_type = SSH_LOG_PROTOCOL;
//     int ssh_port = 22;//Typical SSH port

//     //SSH session configuration
//     ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, "localhost");//The ssh_options_set function defines the type of connection in the ssh session, the second
//     //argument of the function refers to the type of connection to be done, in this case SSH_OPTIONS_HOST refers to the hostname or IP address to connect to

//     ssh_options_set(my_ssh_session, SSH_OPTIONS_PORT, &ssh_port);//Defines the port to connect to in case the port is a integer
//     ssh_options_set(my_ssh_session, SSH_OPTIONS_LOG_VERBOSITY, &protocol_type);//Defines the session logging verbosity
// }

// void connecting_to_server()
// {
//     int conn_check;

//     conn_check = ssh_connect(my_ssh_session);//Connection to server
//     if(conn_check != SSH_OK)//If the connection is not successful
//     {
//         fprintf(stderr, "Error connecting to SSH server %s\n", ssh_get_error(my_ssh_session));
//         exit(-1);
//     }
//     else{
//         printf("Connected");
//     }

//     ssh_disconnect(my_ssh_session);
// }

int main()
{
    //Signal description
    signal(SIGINT,test);//Calls test function when SIGINT pressed

    // //First SSH session
    my_ssh_session = ssh_new();
    int protocol_type = SSH_LOG_PROTOCOL;
    int ssh_port = 22;//Typical SSH port
    int conn_check;

    if(my_ssh_session == NULL)
    {
        exit(-1);//Exits with return code -1
    }

    //SSH session configuration
    ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, "localhost");//The ssh_options_set function defines the type of connection in the ssh session, the second
    //argument of the function refers to the type of connection to be done, in this case SSH_OPTIONS_HOST refers to the hostname or IP address to connect to

    ssh_options_set(my_ssh_session, SSH_OPTIONS_PORT, &ssh_port);//Defines the port to connect to in case the port is a integer
    ssh_options_set(my_ssh_session, SSH_OPTIONS_LOG_VERBOSITY, &protocol_type);//Defines the session logging verbosity


    //Server Connection
    conn_check = ssh_connect(my_ssh_session);//Connection to server
    if(conn_check != SSH_OK)//If the connection is not successful
    {
        fprintf(stderr, "Error connecting to SSH server %s\n", ssh_get_error(my_ssh_session));
        exit(-1);
    }
    else{
        printf("Connected");
    }

    ssh_disconnect(my_ssh_session);
    //connecting_to_server();

    ssh_free(my_ssh_session);//Ends SSH session

    return 0;
}

