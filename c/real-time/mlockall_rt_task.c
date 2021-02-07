
static void mlockall_rt_task()
{
	if(mlockall(MCL_CURRENT | MCL_FUTURE)) {
		perror("mlockall failed.");
		assert(0);
	}
}
