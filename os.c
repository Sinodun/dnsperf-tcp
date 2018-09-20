/*
 * Copyright (C) 2011 - 2015 Nominum, Inc.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose with or without fee is hereby granted,
 * provided that the above copyright notice and this permission notice
 * appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND NOMINUM DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL NOMINUM BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Copyright (C) 2016 Sinodun IT Ltd.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose with or without fee is hereby granted,
 * provided that the above copyright notice and this permission notice
 * appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND NOMINUM DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL NOMINUM BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include <poll.h>

#include <isc/result.h>
#include <isc/types.h>

#include "log.h"
#include "os.h"
#include "util.h"

void
perf_os_blocksignal(int sig, isc_boolean_t block)
{
	sigset_t sset;
	int op;

	op = block ? SIG_BLOCK : SIG_UNBLOCK;

	if (sigemptyset(&sset) < 0 ||
	    sigaddset(&sset, sig) < 0 ||
	    pthread_sigmask(op, &sset, NULL) < 0)
		perf_log_fatal("pthread_sigmask: %s", strerror(errno));
}

void
perf_os_handlesignal(int sig, void (*handler)(int))
{
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = handler;

	if (sigfillset(&sa.sa_mask) < 0 ||
	    sigaction(sig, &sa, NULL) < 0)
		perf_log_fatal("sigaction: %s", strerror(errno));
}

isc_result_t
perf_os_waituntilwriteable(int fd, isc_int64_t timeout)
{
	struct pollfd write_fds[1];
	int timeout_msec;
	int n;

	write_fds[0].fd = fd;
	write_fds[0].events = POLLOUT;
	if (timeout < 0) {
		timeout_msec = 0;
	} else {
		timeout_msec = timeout / THOUSAND;
	}
	n = poll(write_fds, 1, timeout_msec);
	if (n < 0) {
		if (errno != EINTR)
			perf_log_fatal("select() failed: Error was %s", strerror(errno));
		return (ISC_R_CANCELED);
	} else if (write_fds[0].revents & POLLOUT) {
		return (ISC_R_SUCCESS);
	} else {
		return (ISC_R_TIMEDOUT);
	}
}

isc_result_t
perf_os_waituntilreadable(int fd, int pipe_fd, isc_int64_t timeout)
{
	struct pollfd write_fds[2];
	int timeout_msec;
	int n;

	write_fds[0].fd = fd;
	write_fds[1].fd = pipe_fd;
	write_fds[0].events = POLLIN;
	write_fds[1].events = POLLIN;

	if (timeout < 0) {
		timeout_msec = 0;
	} else {
		timeout_msec = timeout / THOUSAND;
	}
	n = poll(write_fds, 2, timeout_msec);
	if (n < 0) {
		if (errno != EINTR)
			perf_log_fatal("select() failed: Error was %s", strerror(errno));
		return (ISC_R_CANCELED);
	} else if (write_fds[0].revents & POLLIN) {
		return (ISC_R_SUCCESS);
	} else if (write_fds[1].revents & POLLIN) {
		return (ISC_R_CANCELED);
	} else {
		return (ISC_R_TIMEDOUT);
	}
}

isc_result_t
perf_os_waituntilanyreadable(int *fds, unsigned int nfds, int pipe_fd,
			     isc_int64_t timeout)
{
	struct pollfd read_fds[nfds + 1];
	int timeout_msec;
	unsigned int i;
	int n;

	for (i = 0; i < nfds; i++) {
		read_fds[i].fd = fds[i];
		read_fds[i].events = POLLIN;
	}
	read_fds[nfds].fd = pipe_fd;

	if (timeout < 0) {
		timeout_msec = 0;
	} else {
		timeout_msec = timeout / THOUSAND;
	}
	n = poll(read_fds, nfds + 1, timeout_msec);
	if (n < 0) {
		if (errno != EINTR)
			perf_log_fatal("select() failed: Error was %s", strerror(errno));
		return (ISC_R_CANCELED);
	} else if (n == 0) {
		return (ISC_R_TIMEDOUT);
	} else if (read_fds[nfds].revents & POLLIN) {
		return (ISC_R_CANCELED);
	} else {
		return (ISC_R_SUCCESS);
	}
}
