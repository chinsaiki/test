
# ifdef __USE_GNU
/* Structure used as control block for asynchronous lookup.  */
struct gaicb
{
  const char *ar_name;		/* Name to look up.  */
  const char *ar_service;	/* Service name.  */
  const struct addrinfo *ar_request; /* Additional request specification.  */
  struct addrinfo *ar_result;	/* Pointer to result.  */
  /* The following are internal elements.  */
  int __return;
  int __unused[5];
};

/* Lookup mode.  */
#  define GAI_WAIT	0
#  define GAI_NOWAIT	1
# endif

/* Possible values for `ai_flags' field in `addrinfo' structure.  */
# define AI_PASSIVE	0x0001	/* Socket address is intended for `bind'.  */
# define AI_CANONNAME	0x0002	/* Request for canonical name.  */
# define AI_NUMERICHOST	0x0004	/* Don't use name resolution.  */
# define AI_V4MAPPED	0x0008	/* IPv4 mapped addresses are acceptable.  */
# define AI_ALL		0x0010	/* Return IPv4 mapped and IPv6 addresses.  */
# define AI_ADDRCONFIG	0x0020	/* Use configuration of this host to choose
				   returned address type..  */
# ifdef __USE_GNU
#  define AI_IDN	0x0040	/* IDN encode input (assuming it is encoded
				   in the current locale's character set)
				   before looking it up. */
#  define AI_CANONIDN	0x0080	/* Translate canonical name from IDN format. */
#  define AI_IDN_ALLOW_UNASSIGNED 0x0100 /* Don't reject unassigned Unicode
					    code points.  */
#  define AI_IDN_USE_STD3_ASCII_RULES 0x0200 /* Validate strings according to
						STD3 rules.  */
# endif
# define AI_NUMERICSERV	0x0400	/* Don't use name resolution.  */

/* Error values for `getaddrinfo' function.  */
# define EAI_BADFLAGS	  -1	/* Invalid value for `ai_flags' field.  */
# define EAI_NONAME	  -2	/* NAME or SERVICE is unknown.  */
# define EAI_AGAIN	  -3	/* Temporary failure in name resolution.  */
# define EAI_FAIL	  -4	/* Non-recoverable failure in name res.  */
# define EAI_FAMILY	  -6	/* `ai_family' not supported.  */
# define EAI_SOCKTYPE	  -7	/* `ai_socktype' not supported.  */
# define EAI_SERVICE	  -8	/* SERVICE not supported for `ai_socktype'.  */
# define EAI_MEMORY	  -10	/* Memory allocation failure.  */
# define EAI_SYSTEM	  -11	/* System error returned in `errno'.  */
# define EAI_OVERFLOW	  -12	/* Argument buffer overflow.  */
# ifdef __USE_GNU
#  define EAI_NODATA	  -5	/* No address associated with NAME.  */
#  define EAI_ADDRFAMILY  -9	/* Address family for NAME not supported.  */
#  define EAI_INPROGRESS  -100	/* Processing request in progress.  */
#  define EAI_CANCELED	  -101	/* Request canceled.  */
#  define EAI_NOTCANCELED -102	/* Request not canceled.  */
#  define EAI_ALLDONE	  -103	/* All requests done.  */
#  define EAI_INTR	  -104	/* Interrupted by a signal.  */
#  define EAI_IDN_ENCODE  -105	/* IDN encoding failed.  */
# endif

# ifdef __USE_MISC
#  define NI_MAXHOST      1025
#  define NI_MAXSERV      32
# endif

# define NI_NUMERICHOST	1	/* Don't try to look up hostname.  */
# define NI_NUMERICSERV 2	/* Don't convert port number to name.  */
# define NI_NOFQDN	4	/* Only return nodename portion.  */
# define NI_NAMEREQD	8	/* Don't return numeric addresses.  */
# define NI_DGRAM	16	/* Look up UDP service rather than TCP.  */
# ifdef __USE_GNU
#  define NI_IDN	32	/* Convert name from IDN format.  */
#  define NI_IDN_ALLOW_UNASSIGNED 64 /* Don't reject unassigned Unicode
					code points.  */
#  define NI_IDN_USE_STD3_ASCII_RULES 128 /* Validate strings according to
					     STD3 rules.  */
# endif

