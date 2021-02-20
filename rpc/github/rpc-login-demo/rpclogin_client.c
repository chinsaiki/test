/**
 * @author : Sohil Trivedi
 * BITS ID - 2015HT12632
 */

#include "rpclogin.h"

/**
 * Responsible for allowing user to login
 * @param host
 * 
 */
void loginUser(char *host) {

       CLIENT *clnt;

       userStatus  *result;
       userDetails  login;

       char username[256];
       char password[256];

       // Create the client
       clnt = clnt_create (host, RPCLOGIN_PROG, RPCLOGIN_VERS, "udp");
       if (clnt == NULL) {
               clnt_pcreateerror (host);
               exit (1);
       }

       printf("\n*********************** User Login ***********************\n");
       printf("Enter Username: ");
       scanf("%s", username);

       printf("Enter Password: ");
       scanf("%s%*c", password);

       strcpy(login.username,username);
       strcpy(login.password,password);

       result = loginprocess_1(&login, clnt);
       if (result == (userStatus *) NULL) {
           clnt_perror (clnt, "call failed: got err.");
       }

       if(result->rescode != 0){
           printf("\nWelcome %s !\n",username);
	   if(strcmp(username,"admin")==0) {
		printf("\nNumber of registered Users: %ld\n", (result->usercount - 1));
	   }
	   exit(0);
       }else {
           printf("Invalid user. Try again!");
       }

       clnt_destroy(clnt);
}


/**
 * Responsible for registering new user
 * @param host
 */
void registerUser(char *host)
{
	CLIENT *clnt;

	userStatus  *result;
	userDetails  uregister;

	char req;
	char username[256];
	char password[256];

	// Create the client
	clnt = clnt_create (host, RPCLOGIN_PROG, RPCLOGIN_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}

	printf("\n******************** User Registration *************************\n");

	printf("Enter Username: ");
	scanf("%s", username);

	printf("Enter Password: ");
	scanf("%s%*c", password);

	strcpy(uregister.username,username);
	strcpy(uregister.password,password);

	result = registerprocess_1(&uregister, clnt);
	if (result == (userStatus *) NULL) {
		clnt_perror (clnt, "call failed: got err.");
	}

	if(result->rescode != 0) {
	  printf("\n Registration Successful !\n");
	  printf("Like to login? (Y/N): ");
	  scanf("%c",&req);
	  if(req == 'Y' || req == 'y'){
	     loginUser(host);
	  }else {
	     exit(0);
          }

	}else {
	  printf("User with same Username already exist. Please try using different Username\n");
	} 
	
	clnt_destroy (clnt);
}

int main (int argc, char *argv[])
{
	char *host;
	int type;

	if (argc < 2) {
		printf ("Command Options: %s server host\n", argv[0]);
		exit (1);
	}

	host = argv[1];

	while(type != 3 ) {
	   system("clear"); 
	   printf("Welcome !!!\n");
	   printf("1. Register\n");
	   printf("2. Login\n");
	   printf("3. Quit\n");
	   printf("Enter Choice: ");
           scanf("%d",&type);
	
	   switch(type){
		case 1:
		  registerUser(host);
		break;

		case 2:
		  loginUser(host);
		  break;

		case 3:
		break;

		default:
		  printf("Invalid Option\n");
		  type = 3;
		  break;
	    }	
	}

	exit (0);
}
