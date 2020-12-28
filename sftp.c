#define SFTP_STATIC 1

#include <stdio.h>
#include <libssh/sftp.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

//int sftp_session(ssh_session session);
//int sftp_directory(ssh_session session, sftp_session sftp_conn);

ssh_session session;


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
        fprintf(stderr, "Error establishing connection: %s\n", sftp_get_error(sftp_conn));
        sftp_free(sftp_conn);//Frees the connection
        return conn_check;
    }

    sftp_free(sftp_conn);
    return SSH_OK;
}

int sftp_directory(ssh_session session, sftp_session sftp_conn)
{
    int conn_check_sftp;

    conn_check_sftp = sftp_mkdir(sftp_conn, "Welcome to SFTP server", S_IRWXU);//The last argument specifies the permissions to use
    if(conn_check_sftp != SSH_OK)
    {
        if(sftp_get_error(sftp_conn) != SSH_FX_FILE_ALREADY_EXISTS)
        {
            fprintf(stderr, "Can't create directory: %s\n", ssh_get_error(session));
            return conn_check_sftp;
        }
    }

    return SSH_OK;
}

int main()
{
    sftp_connection(session);

    return 0;
}