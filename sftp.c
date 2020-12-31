#define LIBSSH_STATIC 1

#include <stdio.h>
#include <libssh/sftp.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

//int sftp_session(ssh_session session);
//int sftp_directory(ssh_session session, sftp_session sftp_conn);

#define DEFAULT_PORT 22

ssh_session session_1;


int sftp_connection(ssh_session session)
{
    sftp_session sftp_conn;
    int conn_check;

    sftp_conn = sftp_new(session);
    if(sftp_conn == NULL)//If there is not a SFTP connection
    {
        fprintf(stderr, "Error opening SFTP session: %s\n", ssh_get_error(session));
        return SSH_ERROR;
    }

    conn_check = sftp_init(sftp_conn);//Initializes a SFTP connection
    if(conn_check != SSH_OK)
    {
        //fprintf(stderr, "Error establishing connection: %s\n", sftp_get_error(sftp_conn));
        sftp_free(sftp_conn);//Frees the connection
        return conn_check;
    }

    sftp_free(sftp_conn);
    return SSH_OK;
}

// int sftp_directory(ssh_session session, sftp_session sftp_conn)
// {
//     int conn_check_sftp;

//     conn_check_sftp = sftp_mkdir(sftp_conn, "Welcome to SFTP server", S_IRWXU);//The last argument specifies the permissions to use
//     if(conn_check_sftp != SSH_OK)
//     {
//         if(sftp_get_error(sftp_conn) != SSH_FX_FILE_ALREADY_EXISTS)
//         {
//             fprintf(stderr, "Can't create directory: %s\n", ssh_get_error(session));        
//             return conn_check_sftp;
//         }
//     }

//     return SSH_OK;
// }

int main()
{
    //Create the ssh session
    session_1 = ssh_new();
    int protocol_type = SSH_LOG_PROTOCOL;
    int ssh_port = DEFAULT_PORT;//Typical SSH port
    int conn_check;

    if(session_1 == NULL)
    {
        exit(-1);//Exits with return code -1
    }

    //SSH session configuration
    ssh_options_set(session_1, SSH_OPTIONS_HOST, "localhost");//The ssh_options_set function defines the type of connection in the ssh session, the second
    //argument of the function refers to the type of connection to be done, in this case SSH_OPTIONS_HOST refers to the hostname or IP address to connect to

    ssh_options_set(session_1, SSH_OPTIONS_PORT, &ssh_port);//Defines the port to connect to in case the port is a integer
    ssh_options_set(session_1, SSH_OPTIONS_LOG_VERBOSITY, &protocol_type);//Defines the session logging verbosity

    //Checks connection
    conn_check = ssh_connect(session_1);//Connection to server
    if(conn_check != SSH_OK)//If the connection is not successful
    {
        fprintf(stderr, "Error connecting to SSH server %s\n", ssh_get_error(session_1));
        exit(-1);
    }
    else{
        printf("Connected");
    }

    sftp_connection(session_1);

    return 0;
}