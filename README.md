# dnsperf-tcp: TCP Support in dnsperf

This is a fork of Nominum's 2.1.0.0 dnsperf tool
http://nominum.com/measurement-tools/ which adds initial support for TCP. 
This is a work in progress!

Be aware that the current implementation of TCP works within the existing design
of this software, with the limitations imposed by this. So it is not really 
optimal for TCP connection handling - a better solution would be a standalone
tool with all the functionality of dnsperf/resperf.

Feedback and comments are welcome either via email to 
sara@sinodun.com or jad@sinodun.com or via the issue tracker for this repo.

This work is supported by a donation from NLnet Foundation.

# Notes

* To date only dnsperf has been updated, not resperf.

* When performance testing TCP there are many more considerations than with UDP,
  particularly in terms of tuning the kernel and the nameserver configuration.
  Also when limiting the number of queries per connection at high query rates
  errors may be encountered because the client machine may be attempting to 
  re-use connections in the TIME_WAIT state.

* There are many optimisations that could be added to this implementation and a 
  number of TCP specific features to add in future e.g.
  * TCP Fast Open
  * Option to use TCP_NODELAY
  * Support for TLS

* The Statistics need to carefully interpreted when using TCP. The output called
  'Latency' has been re-named to 'RTT' since this is what it actually measures
  for each DNS message. The 'Run time' reported for UDP is the amount of time 
  taken to send all the queries. This is also true for TCP, but it will include
  the time taken for TCP handshakes and connection re-cycling.

* Note that a few TCP 'extra' connections may be opened at the end of the run
 even though they are not needed due to the way dnsperf works.

# Usage

* TCP is selected by specifying the '-z' flag. 

* This implementation was done with RFC7766 in mind so that by default dnsperf
  will open 1 TCP connection per client (as specified with the '-c' flag) and 
  pipeline queries over that connection. A limit on the number of queries sent
  per connection can be set with the '-Z' flag.

* The '-x' flag is only honoured for TCP when the number of messages on a 
  connection is unlimited.
