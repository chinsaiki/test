#include <assert.h>
#include <link.h>
#include <pthread.h>
#include <signal.h>
#include <syslog.h>
#include <unistd.h>
#include "patch_set.h"
#include "patch_script.h"
#include "status.h"

#define PROGRAM_IDENT "dpatch"

/**
 * Suppresses unused parameter warnings.
 *
 * @note `UNUSED` is intended for use in function bodies
 * when a parameter is required by an API, but is unused.
 *
 * `UNUSED` generates a synaxically correct, but
 * meaningless statement, using the variable, making it
 * technically 'used' to suppress the warning.
 */
#define UNUSED(x) ((void)(x))

/**
 * Signals a pending patch to the patcher-thread.
 *
 * @note This should be set by the patch signal handler.
 */
bool patch_pending = false;

/**
 * Applies a pending patch.
 *
 * @note The patch parsing functions are not reentrant, so
 * they must be called outside of the signal handler, or
 * made reentrant.
 */
dpatch_status do_patch()
{
    syslog(LOG_INFO, "Dynamic patch initiated.");
    patch_script_t* patch_script = NULL;
    patch_set_t* patch_set = NULL;
    /* 
     * Exit on error shouldn't be the default behaviour in
     * the long term. We do this as a temporary solution to
     * test apporaches without robust error handling.
     */
    EXIT_ON_ERROR(patch_script_new(&patch_script));
    EXIT_ON_ERROR(patch_set_new(&patch_set));
    EXIT_ON_ERROR(patch_script_parse(patch_script, patch_set));
    LOG_ON_ERROR(patch_set_apply(patch_set));
    patch_script_free(patch_script);
    patch_set_free(patch_set);
    syslog(LOG_INFO, "Dypamic patch applied.");
    return DPATCH_STATUS_OK;
}

/**
 * Applies a patch then exits the current thread.
 *
 * `patch_in_thread` is indended to be called from a
 * transient thread spawed to execute a patch.
 *
 * @param args Unused, but required by the `pthreads` API.
 * @return Nothing - required by the P
 */
void* patch_in_thread(void* args)
{
    UNUSED(args);
    int status = (int) do_patch();
    pthread_exit(&status);
}


/**
 * SIGUSR2 initiates a dynamic path.
 *
 * @note We apply the patch from another thread, rather
 * than from the signal handler, because many functions
 * used by `dpatch` are not reentrant - such as `malloc`.
 * Calling them from inside the signal's interrupt context
 * can cause very undefined behaviour.
 *
 * @param signal The incomming singal to handle.
 */
void sigusr2_handler(int signal)
{
    pthread_t patch_thread = 0;
    assert(signal == SIGUSR2);
    syslog(LOG_INFO, "Recieved SIGUSR2. Requesting patch.");
    pthread_create(&patch_thread, NULL, &patch_in_thread, NULL);
}

/**
 * Inidcate the verion of the link audit library this tool
 * was compiled against.
 * 
 * `la_version` allows the RTDL to reect this audit library
 * at runtime if it uses an unsupported ABI.
 *
 * @param version The RTDL Audit API the host supports.
 * @return The RTDL audit API this object was compiled
 * for.
 */
extern unsigned int la_version(unsigned int version)
{
    if (version != LAV_CURRENT)
    {
        openlog(PROGRAM_IDENT, LOG_PERROR, LOG_USER);
        syslog(LOG_ERR, "Unsupported RTLD audit API. Host: v%d, dpatch: v%d", version, LAV_CURRENT);
        closelog();
        exit(EXIT_FAILURE);
    }
    return LAV_CURRENT;
}

/**
 * Preinit hook to be called before the target's `main` is
 * executed.
 *
 * The pre-init hook sets up a signal handler to listen for
 * dynamic patches.
 *
 * @param cookie The object at the head of the link map.
 */
extern void la_preinit(uintptr_t* cookie)
{
    UNUSED(cookie);
    openlog(PROGRAM_IDENT, LOG_PERROR, LOG_USER);
    signal(SIGUSR2, sigusr2_handler);
}
