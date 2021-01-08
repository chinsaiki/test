#pragma once


#define RTE_MP_MAX_FD_NUM	8    /* The max amount of fds */
#define RTE_MP_MAX_NAME_LEN	64   /* The max length of action name */
#define RTE_MP_MAX_PARAM_LEN	256  /* The max length of param */
struct rte_mp_msg {
	char name[RTE_MP_MAX_NAME_LEN];
	int len_param;
	int num_fds;
	uint8_t param[RTE_MP_MAX_PARAM_LEN];
	int fds[RTE_MP_MAX_FD_NUM];
};

struct rte_mp_reply {
	int nb_sent;
	int nb_received;
	struct rte_mp_msg *msgs; /* caller to free */
};

/**
 * Action function typedef used by other components.
 *
 * As we create  socket channel for primary/secondary communication, use
 * this function typedef to register action for coming messages.
 *
 * @note When handling IPC request callbacks, the reply must be sent even in
 *   cases of error handling. Simply returning success or failure will *not*
 *   send a response to the requestor.
 *   Implementation of error signalling mechanism is up to the application.
 *
 * @note No memory allocations should take place inside the callback.
 */
typedef int (*rte_mp_t)(const struct rte_mp_msg *msg, const void *peer);

/**
 * Asynchronous reply function typedef used by other components.
 *
 * As we create socket channel for primary/secondary communication, use
 * this function typedef to register action for coming responses to asynchronous
 * requests.
 *
 * @note When handling IPC request callbacks, the reply must be sent even in
 *   cases of error handling. Simply returning success or failure will *not*
 *   send a response to the requestor.
 *   Implementation of error signalling mechanism is up to the application.
 *
 * @note No memory allocations should take place inside the callback.
 */
typedef int (*rte_mp_async_reply_t)(const struct rte_mp_msg *request,
		const struct rte_mp_reply *reply);

/**
 * @warning
 * @b EXPERIMENTAL: this API may change without prior notice
 *
 * Register an action function for primary/secondary communication.
 *
 * Call this function to register an action, if the calling component wants
 * to response the messages from the corresponding component in its primary
 * process or secondary processes.
 *
 * @note IPC may be unsupported in certain circumstances, so caller should check
 *    for ENOTSUP error.
 *
 * @param name
 *   The name argument plays as the nonredundant key to find the action.
 *
 * @param action
 *   The action argument is the function pointer to the action function.
 *
 * @return
 *  - 0 on success.
 *  - (<0) on failure.
 */
__rte_experimental
int
rte_mp_action_register(const char *name, rte_mp_t action);

/**
 * @warning
 * @b EXPERIMENTAL: this API may change without prior notice
 *
 * Unregister an action function for primary/secondary communication.
 *
 * Call this function to unregister an action  if the calling component does
 * not want to response the messages from the corresponding component in its
 * primary process or secondary processes.
 *
 * @note IPC may be unsupported in certain circumstances, so caller should check
 *    for ENOTSUP error.
 *
 * @param name
 *   The name argument plays as the nonredundant key to find the action.
 *
 */
__rte_experimental
void
rte_mp_action_unregister(const char *name);

/**
 * @warning
 * @b EXPERIMENTAL: this API may change without prior notice
 *
 * Send a message to the peer process.
 *
 * This function will send a message which will be responded by the action
 * identified by name in the peer process.
 *
 * @param msg
 *   The msg argument contains the customized message.
 *
 * @return
 *  - On success, return 0.
 *  - On failure, return -1, and the reason will be stored in rte_errno.
 */
__rte_experimental
int
rte_mp_sendmsg(struct rte_mp_msg *msg);

/**
 * @warning
 * @b EXPERIMENTAL: this API may change without prior notice
 *
 * Send a request to the peer process and expect a reply.
 *
 * This function sends a request message to the peer process, and will
 * block until receiving reply message from the peer process.
 *
 * @note The caller is responsible to free reply->replies.
 *
 * @note This API must not be used inside memory-related or IPC callbacks, and
 *   no memory allocations should take place inside such callback.
 *
 * @note IPC may be unsupported in certain circumstances, so caller should check
 *    for ENOTSUP error.
 *
 * @param req
 *   The req argument contains the customized request message.
 *
 * @param reply
 *   The reply argument will be for storing all the replied messages;
 *   the caller is responsible for free reply->msgs.
 *
 * @param ts
 *   The ts argument specifies how long we can wait for the peer(s) to reply.
 *
 * @return
 *  - On success, return 0.
 *  - On failure, return -1, and the reason will be stored in rte_errno.
 */
__rte_experimental
int
rte_mp_request_sync(struct rte_mp_msg *req, struct rte_mp_reply *reply,
	       const struct timespec *ts);

/**
 * @warning
 * @b EXPERIMENTAL: this API may change without prior notice
 *
 * Send a request to the peer process and expect a reply in a separate callback.
 *
 * This function sends a request message to the peer process, and will not
 * block. Instead, reply will be received in a separate callback.
 *
 * @note IPC may be unsupported in certain circumstances, so caller should check
 *    for ENOTSUP error.
 *
 * @param req
 *   The req argument contains the customized request message.
 *
 * @param ts
 *   The ts argument specifies how long we can wait for the peer(s) to reply.
 *
 * @param clb
 *   The callback to trigger when all responses for this request have arrived.
 *
 * @return
 *  - On success, return 0.
 *  - On failure, return -1, and the reason will be stored in rte_errno.
 */
__rte_experimental
int
rte_mp_request_async(struct rte_mp_msg *req, const struct timespec *ts,
		rte_mp_async_reply_t clb);

/**
 * @warning
 * @b EXPERIMENTAL: this API may change without prior notice
 *
 * Send a reply to the peer process.
 *
 * This function will send a reply message in response to a request message
 * received previously.
 *
 * @note When handling IPC request callbacks, the reply must be sent even in
 *   cases of error handling. Simply returning success or failure will *not*
 *   send a response to the requestor.
 *   Implementation of error signalling mechanism is up to the application.
 *
 * @param msg
 *   The msg argument contains the customized message.
 *
 * @param peer
 *   The peer argument is the pointer to the peer socket path.
 *
 * @return
 *  - On success, return 0.
 *  - On failure, return -1, and the reason will be stored in rte_errno.
 */
__rte_experimental
int
rte_mp_reply(struct rte_mp_msg *msg, const char *peer);

