CS342 OPERATING SYSTEMS PROJECT 3, SPRING 2019
---------------------------------------------------------------------------
PROJECT MADE BY:

NAISILA PUKA 21600336 SECTION 2
KUNDUZ EFRONOVA 21600469 SECTION 2
---------------------------------------------------------------------------

app.c is our application file.

Invoke by 
	make
	./app.c DEADLOCK_HANDLING_METHOD

where DEADLOCK_HANDLING_METHOD is to be replaced by:
	1 for DEADLOCK_NOTHING
	2 for DEADLOCK_DETECTION
	3 for DEADLOCK_AVOIDANCE

In deadlock detection, it calls detection every second, for 20 seconds.
You will see that sometimes there is deadlock, sometimes not, depending
on the instant.

Program has 20 threads and 10 resources.

---------------------------------------------------------------------------
Our experiment.c source file is tested through the following invokation:
	
	./exp

Read more in our report.
---------------------------------------------------------------------------