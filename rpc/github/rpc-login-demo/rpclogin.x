struct userDetails {
	char username[256];
	char password[256];
};
struct userStatus {
	long rescode;
	long usercount;
};
program RPCLOGIN_PROG {
	version RPCLOGIN_VERS {
		userStatus REGISTERPROCESS(userDetails) = 1;
		userStatus LOGINPROCESS(userDetails) = 2;
		} = 1;
} = 0x23451111;
