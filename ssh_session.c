#define LIBSSH_STATIC 1//Static linking
#include <libssh/libssh.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/signal.h>
#include <errno.h>
#include <string.h>

#define MIN_PORT 0
#define MAX_PORT 65535
#define DEFAULT_PORT 22

//Static linking is the process of copying all library modules used in the program into the final executable image

ssh_session my_ssh_session;

//Signal handler function
void test()
{
    printf("Quitting\n");
}


//Server Authentication

int server_authentication(ssh_session server_session)
{
    enum ssh_known_hosts_e state;
    unsigned char *hash = NULL;
    ssh_key server_public_key = NULL;
    size_t hash_len;
    char buf[10];
    char *hexa;
    char *p;
    int cmp;
    int key_check_server;

    //Obtaining server key

    key_check_server = ssh_get_server_publickey(server_session, &server_public_key);
    if(key_check_server < 0)//If there is no public key
    {
        return -1;
    }

    key_check_server = ssh_get_publickey_hash(server_public_key, SSH_PUBLICKEY_HASH_SHA1, &hash, &hash_len);//Obtains the binary hash key

    ssh_key_free(server_public_key);//Deallocates the SSH server key
    if(key_check_server < 0)
    {
        return -1;
    }


    //State of the session
    state = ssh_session_is_known_server(server_session);

    switch(state)
    {
        case SSH_KNOWN_HOSTS_OK: 
            break;
        case SSH_KNOWN_HOSTS_CHANGED:
        {
            fprintf(stderr, "Host key for server changed: it is now:\n");
            ssh_print_hexa("Public key hash", hash, hash_len);
            fprintf(stderr, "For security reasons, connection will be stopped\n");
            ssh_clean_pubkey_hash(&hash);
            return -1;
        }
        case SSH_KNOWN_HOSTS_OTHER:
        {
            fprintf(stderr, "The host key for this server was not found but an other"
                    "type of key exists.\n");
            fprintf(stderr, "An attacker might change the default server key to"
                    "confuse your client into thinking the key does not exist\n");
            ssh_clean_pubkey_hash(&hash);
            return -1;
        }
        case SSH_KNOWN_HOSTS_NOT_FOUND:
        {
            fprintf(stderr, "Could not find known host file.\n");
            fprintf(stderr, "If you accept the host key here, the file will be"
                    "automatically created.\n");
            /* FALL THROUGH to SSH_SERVER_NOT_KNOWN behavior */
        }
        case SSH_KNOWN_HOSTS_UNKNOWN:
            {
                hexa = ssh_get_hexa(hash, hash_len);
                fprintf(stderr,"The server is unknown. Do you trust the host key?\n");
                fprintf(stderr, "Public key hash: %s\n", hexa);
                ssh_string_free_char(hexa);
                ssh_clean_pubkey_hash(&hash);
                p = fgets(buf, sizeof(buf), stdin);
                if (p == NULL) {
                    return -1;
                }
            
                cmp = strncasecmp(buf, "yes", 3);
                if (cmp != 0) {
                    return -1;
                }
                key_check_server = ssh_session_update_known_hosts(server_session);
                if (key_check_server < 0) {
                    fprintf(stderr, "Error %s\n", strerror(errno));
                    return -1;
                }
                break;
            }

        case SSH_KNOWN_HOSTS_ERROR:
        {
            fprintf(stderr, "Error %s", ssh_get_error(server_session));
            ssh_clean_pubkey_hash(&hash);
            return -1;
        }

    }

}

//User Authentication

void user_authentication()
{
    int conn_check;
    char *password;

    //Check server identity
    if(server_authentication(my_ssh_session) < 0)//If the host is not available
    {
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);//Closes the SSH connection
        exit(-1);
    }

    //Authenticate user
    password = getpass("Password: ");
    conn_check = ssh_userauth_password(my_ssh_session, NULL, password);
    if(conn_check != SSH_AUTH_SUCCESS)//If the authentication is not successful
    {
        fprintf(stderr, "Error authenticating");
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        exit(-1);
    }
}

//Open a remote shell

int remote_shell(ssh_session session)
{
    ssh_channel channel;
    int conn_check;

    channel = ssh_channel_new(session);//Creates the SSH channel
    if(channel == NULL)
        return SSH_ERROR;
    
    conn_check = ssh_channel_open_session(channel);//Opens the remote shell
    if(conn_check != SSH_OK)//If the channel is not open
    {
        ssh_channel_free(channel);//Frees the channel
        return conn_check;
    }

    ssh_channel_close(channel);
    ssh_channel_send_eof(channel);//Sends a EOF signal to the channel
    ssh_channel_free(channel);

    return SSH_OK;
}

int main(int argc, char *argv[])
{
    //Signal description
    signal(SIGINT,test);//Calls test function when SIGINT pressed

    // //First SSH session
    my_ssh_session = ssh_new();
    int protocol_type = SSH_LOG_PROTOCOL;
    int ssh_port = DEFAULT_PORT;//Typical SSH port
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

    //server_authentication(my_ssh_session);
    user_authentication();
    remote_shell(my_ssh_session);

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

