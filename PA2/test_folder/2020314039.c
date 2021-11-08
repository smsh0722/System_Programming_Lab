/* SWE2024(Fall 2021)
 * 2020314039
 * 홍혁진
 * */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/stat.h>

#define SIZE1 6
#define SIZE2 4
#define SIZE3 3
#define SIZE4 2
#define cmd_type1 (const char*[SIZE1]){ "ls", "man", "grep", "sort", "awk", "bc" }
#define cmd_type2 (const char*[SIZE2]){ "head", "tail", "cat", "cp" }
#define cmd_type3 (const char*[SIZE3]){ "mv", "rm", "cd" }
#define cmd_type4 (const char*[SIZE4]){ "pwd", "exit" }

enum SYMBOLS {
    DEFAULT,
    INPUT_REDIR,
    OUTPUT_REDIR_APPEND,
    OUTPUT_REDIR,
};
enum CMD_TYPE {
    TYPE1,
    TYPE2,
    TYPE3,
    TYPE4,
    TYPE_NONE,
};

void term_sig_control( int sig )
{
    write( 1, "\n", sizeof(char) );
    fflush(stdin);
    return;
}
void error_control( const char* cmd, const int error )
{
    char buf[512] = {0};
    switch (error)
    {
        case EACCES :
        {
            strcpy( buf, "Permission denied\n" );
            break;
        }
        case EISDIR :
        {
            strcpy( buf, "Is a directory\n" );
            break;
        }
        case ENOENT :
        {
            strcpy( buf, "No such file or directory\n" );
            break;
        }
        case ENOTDIR :
        {
            strcpy( buf, "Not a directory\n" );
            break;
        }
        case EPERM :
        {
            strcpy( buf, "Permission denied\n" );
            break;
        }
        default :
        {
            sprintf( buf, "%d", error );
            break;
        }
    }
    write( 2, cmd, sizeof(char)*strlen(cmd) );
    write( 2, buf, sizeof(char)*strlen(buf) );
}

void my_tok(char* cmd, char* arg[], char* arg2[], const char* target)
{
    char* pch = strchr( cmd, target[0] );
    char* l_cmd = (char*)malloc( sizeof(char)*strlen(cmd) );
    char* r_cmd = (char*)malloc( sizeof(char)*strlen(cmd) );
    strncpy( l_cmd, cmd, pch - cmd );
    if ( strchr( pch+1, target[0] ) != NULL ){
        pch = strchr( pch+1, target[0] );
    }
    strcpy( r_cmd, pch + 1 );

    int i;
    {
        i = 0;
        char* ptok;
        ptok = strtok(l_cmd, " ");
        while (ptok != NULL)
        {
            arg[i] = ptok;
            i++;
            ptok = strtok(NULL, " ");
        }
        arg[i] = NULL;
    }
    {
        i = 0;
        char* ptok;
        ptok = strtok(r_cmd, " ");
        while (ptok != NULL)
        {
            arg2[i] = ptok;
            i++;
            ptok = strtok(NULL, " ");
        }
        arg2[i] = NULL;
    }
    return;
}

enum CMD_TYPE check_cmd_type( const char* user_cmd )
{
    int i;
    for ( i = 0; i < SIZE1; i++ )
        if ( strcmp(cmd_type1[i], user_cmd) == 0 )
            return TYPE1;
    for ( i = 0; i < SIZE2; i++ )
        if ( strcmp(cmd_type2[i], user_cmd) == 0 )
            return TYPE2;
    for ( i = 0; i < SIZE3; i++ )
        if ( strcmp(cmd_type3[i], user_cmd) == 0 )
            return TYPE3;
    for ( i = 0; i < SIZE4; i++ )
        if ( strcmp(cmd_type4[i], user_cmd) == 0 )
            return TYPE4;
    return TYPE_NONE;
}

void cmd_execv( char* arg[], char* arg2[], enum SYMBOLS symbol )
{
    char path[128] = {0};
    sprintf(path, "/bin/%s", arg[0]);

    /* redirection */
    switch ( symbol )
    {
        case DEFAULT:
            break;
        case INPUT_REDIR:
        {
            int dst;
            if ( (dst = open( arg2[0], O_RDONLY ) ) < 0 ){
                int gid = getpgrp();
                {
                    const char* tmp_buf = "swsh: No such file\n";
                    write( 1, tmp_buf, sizeof(char)*strlen(tmp_buf) );
                }
                killpg( gid, SIGKILL );
            }
            dup2( dst, 0 ); // stdin redirection
            break;
        }
        case OUTPUT_REDIR_APPEND:
        {
            int dst = open( arg2[0], O_WRONLY | O_CREAT | O_APPEND, 0664 );
            // lseek( dst, 0, SEEK_END );
            dup2( dst, 1 ); // stdout redirection
            break;
        }
        case OUTPUT_REDIR:
        {
            int dst = open( arg2[0], O_WRONLY | O_CREAT | O_TRUNC , 0664 ); 
            dup2( dst, 1 ); // stdout redirection
            break;
        }
    }

    /* execute by cmd_tpye */
    enum CMD_TYPE cmd_type = check_cmd_type( arg[0] );
    if ( cmd_type == TYPE1 ){
        execv( path, arg );
    }
    else if ( cmd_type == TYPE2 ){
        if ( strcmp( arg[0], "head" ) == 0 ){
            char path[256] = {0};
            int line = 10;
            strcpy( path, arg[1] );
            
            if ( (arg[3] != NULL)&&(strcmp( arg[1], "-n" ) == 0) ){
                strcpy( path, arg[3] );
                line = atoi( arg[2] );
            }
            else if ( (arg[3] != NULL)&&(strcmp( arg[2], "-n" ) == 0 ) ){
                strcpy( path, arg[1] );
                line = atoi( arg[3] );
            }
            
            int rfd = open( path, O_RDONLY ); // always exist
            for ( int i = 0; i < line; i++ ){
                char forbreak = 0; // true = 1, false = 0
                while ( 1 ){
                    char c[2] = {0};
                    if ( read( rfd, c, 1 ) <= 0 ){
                        forbreak = 1;
                        break;
                    }
                    write( 1, c, sizeof(char) );

                    if ( c[0] == '\n' )
                        break;
                }
                if ( forbreak == 1 )
                    break;
            }
            close(rfd);
        }
        else if ( strcmp( arg[0], "tail") == 0 ){
            char path[256] = {0};
            int line = 10;
            strcpy( path, arg[1] );
            
            if ( (arg[3] != NULL)&&(strcmp( arg[1], "-n" ) == 0) ){
                strcpy( path, arg[3] );
                line = atoi( arg[2] );
            }
            else if ( (arg[3] != NULL)&&(strcmp( arg[2], "-n" ) == 0 ) ){
                strcpy( path, arg[1] );
                line = atoi( arg[3] );
            }
            
            int rfd = open( path, O_RDONLY ); // always exist
            char* buf;
            int size;
            {
                struct stat st;
                stat( path, &st );
                size = st.st_size;
                buf = (char*)malloc( sizeof(char)* size );
            }
            read(rfd, buf, size );
            int line_file = 0;
            for ( int i = 0; i < size; i++ ){
                if ( buf[i] == '\n' )
                    line_file++;
            }
            for ( int i = 0; i < size; i++ ){
                if ( line_file > line && buf[i] == '\n' ){
                    line_file--;
                    continue;
                }
                if ( line_file <= line ){
                    char c[2] = {0};
                    c[0] = buf[i];
                    write(1, c, sizeof(char) );
                }
            }
            close(rfd);
            free( buf );
        }
        else if ( strcmp( arg[0], "cat" ) == 0 ){
            char path[256] = {0};
            strcpy( path, arg[1] );
            
            int rfd = open( path, O_RDONLY ); // alway exist
            char* c[2] = {0};
            while ( read(rfd, c, 1 ) != 0 ){
                write(1, c, sizeof( char ) );
            }
            close(rfd);
        }
        else if ( strcmp( arg[0], "cp" ) == 0 ){
            const char* file1 = arg[1]; // always exist
            const char* file2 = arg[2];
            /* copy */
            {
                int fd1 = open( file1, O_RDONLY );
                int fd2 = open( file2, O_WRONLY | O_CREAT | O_TRUNC, 0664 );
                char* buf;
                int fSize = 0;
                {
                    struct stat st;
                    stat( file1, &st );
                    fSize = st.st_size;
                    buf = (char*)malloc(sizeof(char)*fSize);
                }
                read( fd1, buf, sizeof(char)*fSize );
                write( fd2, buf, sizeof(char)*fSize );
                
                close(fd1); close(fd2);
                free(buf);
            }
        }
    }
    else if ( cmd_type == TYPE3 ){
        if ( strcmp( arg[0], "mv" ) == 0 ){
            const char* file1 = arg[1];
            const char* file2 = arg[2];
            if ( rename( file1, file2 ) == -1 )
                error_control( "mv: ", errno );
        }
        else if ( strcmp( arg[0], "rm" ) == 0 ){
            const char* file = arg[1];
            if ( unlink( file ) == -1 )
                error_control( "rm: ", errno );
        }
        else if ( strcmp( arg[0], "cd" ) == 0 ){
            const char* dir = arg[1];
            if ( chdir( dir ) == -1 )
                error_control( "cd: ", errno );
        }
    }
    else if ( cmd_type == TYPE4 ){
        if ( strcmp( arg[0], "pwd" ) == 0 ) {
            char buf[512] = {0};
            getcwd( buf, sizeof(buf) );
            write( 1, buf, sizeof(char)*strlen(buf) );
            write( 1, "\n", sizeof(char) );
        }
        else if ( strcmp( arg[0], "exit" ) == 0 ){
            int status = atoi( arg[1] );
            exit(status);
        }
    }
    else {
        const char* tmp_buf = "swsh: Command not found\n";
        write( 2, tmp_buf, sizeof(char)*strlen(tmp_buf) ); // to stderr
        int gid = getpgrp();
        killpg( gid, SIGKILL );
    }
}

void my_excv( char* cmds[], int cmds_i, const int cmds_size )
{
    int child_status;
    int fd[2];
    char* arg[128] = {0};
    char* arg2[128] = {0};
    enum SYMBOLS symbol = DEFAULT;
    int pipe_flag = 1; // true == 1, false == 0;

    /* Set arg, arg2, symbol */
    if ( cmds_i == 0 || cmds_i + 1 == cmds_size ){
        if ( strchr( cmds[cmds_i], '<' ) != NULL ){
            my_tok( cmds[cmds_i], arg, arg2, "<");
            symbol = INPUT_REDIR;
        }
        else if ( strstr( cmds[cmds_i], ">>" ) != NULL ){
            my_tok( cmds[cmds_i], arg, arg2, ">");
            symbol = OUTPUT_REDIR_APPEND;
        }
        else if ( strchr( cmds[cmds_i], '>' ) != NULL ){
            my_tok( cmds[cmds_i], arg, arg2, ">");
            symbol = OUTPUT_REDIR;
        }
    }
    if ( symbol == DEFAULT ){
        int i = 0;
        char tmp[512] = {0};
        strcpy( tmp, cmds[cmds_i] );
        char* pch = strtok( tmp, " " );
        while ( pch != NULL ){
            arg[i] = pch;
            i++;
            pch = strtok( NULL, " " );
        }
        arg[i] = NULL;
    }

    /* pipe check */
    if ( cmds_i + 1 == cmds_size )
        pipe_flag = 0; 
    
    /* execute */
    {
        if ( pipe_flag == 1 ){
            pipe(fd);
            if ( fork() == 0 ){

                close(fd[0]);
                dup2( fd[1], 1 ); // output to next cmd
                // exec
                cmd_execv( arg, arg2, symbol );

                exit(0);
            }
            else {
                wait(&child_status);
                close(fd[1]);
                dup2( fd[0], 0 ); // input from previous cmd
                my_excv( cmds, cmds_i + 1, cmds_size );
            }
        }
        else { // pipe_flag == 0
            //exec
            cmd_execv( arg, arg2, symbol );
            exit(0);
        }
    }
}

int main( void )
{
    size_t size;
    char* input;
    int i;
    int child_status;

    /* signal */
    {
        signal( SIGINT, term_sig_control );
        signal( SIGTSTP, term_sig_control );
    }


    while(1){
        /* input */
        char* cmds[16] = {0};
        int cmds_size = 0;
        {
            input = NULL;
            getline(&input, &size, stdin );
            input[strlen(input)-1] = '\0';

            i = 0;
            char* pch = strtok( input, "|" );
            while ( pch != NULL ){
                cmds[i] = pch;
                i++;
                pch = strtok( NULL, "|" );
            }
            cmds_size = i;
        }

        /* exit, cd check */
        {
            char* arg[128] = {0};
            {
                i = 0;
                char tmp[512] = {0};
                strcpy( tmp, cmds[0] );
                char* pch = strtok( tmp, " " );
                while ( pch != NULL ){
                    arg[i] = pch;
                    i++;
                    pch = strtok( NULL, " " );
                }
                arg[i] = NULL;
            }
            if ( strcmp( arg[0], "exit" ) == 0 ){
                int status = arg[1] != NULL ? atoi( arg[1] ) : 0;
                exit( status );
            }
            else if ( strcmp( arg[0], "cd" ) == 0 ){
                const char* trg_dir = arg[1];
                if ( chdir( trg_dir ) == -1 )
                    error_control( "cd: ", errno );
                continue;
            }
        }

        /* execute */
        if ( fork() == 0 ){
            if ( setsid() < 0 )
                exit(1);

            my_excv( cmds, 0, cmds_size );
            exit(0);
        }
        else 
            wait(&child_status);
    }

    wait(0);
    return 0;
}