1. Overview
-----------
syspart (system partitioning) is a set of scripts for partitioning Linux systems
into domains (partitions) that can be used for different purposes. For example, 
one partition can be used for general purpose tasks and another for hard & soft
realtime (RT) applications.

Currently only two partitions are supported: 'gpp' and 'isp'.
gpp (general-purpose partition) is setup for running general purpose tasks, handling interrupts, etc. 
isp (isolatated partition) is setup for running RT tasks. CPUs in 'isp' are isolated from the rest
of the system.

The main tool is usually installed as /usr/sbin/syspart.

As of version 2.0 syspart has been tested with Linux kernels 3.12.x and requires additional
patches to achieve complete isolation. See more info below.

2. Usage examples
-----------------
Partition the system such that CPUs 2 and 3 are isolated

    syspart --isp-cpus 2-3 --create

Partition the system such that CPU 1 is isolated, and constrain ISP 
partiton to NUMA node 1 and GPP to node 0

    syspart --isp-cpus 2-3 --isp-mems 1 --gpp-mems 0 --create

Check and report current partitioning setup

    syspart --check

Destory system partitions (if any) and return everything to default settings

    syspart --destroy

3. Integration with Fedora, RedHat and CentOS systems
-----------------------------------------------------
Syspart package comes with the default sysconfig file, sysv init.d script
and systemd service file:

    /etc/init.d/syspart
    /etc/sysconfig/syspart
    /usr/lib/systemd/system/syspart.service

See sysconfig/syspart for more details on how to configure syspart service.  
If needed you can enable syspart service to be started at boot

    /sbin/chkconfig syspart on
or on the newer versions that use systemd

    systemctl enable syspart

4. Integration with Debian and Ubuntu
-------------------------------------
The main syspart tool by itself works fine under Ubuntu.
Proper upstart integration is being worked on.

5. Support kernel versions and extra patches
--------------------------------------------
As of syspart-2.0 the officialy supported kernel is 3.12. Newer kernels should 
work too.
In order to achive complete CPU isolation the following patches on top 
of 3.12 are recomended:

    kthread: Introduce kernel.default_kthread_cpumask sysctl
    workqueue: Introduce workqueue.avoid_cpus kernel parameter
    pagealloc: Introduce cpumask for restricting page drain to certain cpus
    workqueue: Expose all system workqueues via sysfs
    sysctl: Add generic cpumask parser for sysctls
