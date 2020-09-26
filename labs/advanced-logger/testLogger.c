#include <stdio.h>
#include <stdlib.h>
#include "logger.h"

int main(int argc, char **argv)
{
	int option;
	if(argc==1)
	{
		option = 0;
	}else
	{
		option = atoi(argv[1]);
	}
	
	printf("%d\n", option);
	switch (option)
	{
	case 1:
		// default logging
		infof("INFO Message %d", 1);
		warnf("WARN Message %d", 2);
		errorf("ERROR Message %d", 2);
		break;
	case 2:
		// stdout logging
		initLogger("stdout");
		infof("INFO Message %d", 1);
		warnf("WARN Message %d", 2);
		errorf("ERROR Message %d", 2);
		break;
	case 3:
		// syslog logging
		initLogger("syslog");
		infof("INFO Message %d", 1);
		warnf("WARN Message %d", 2);
		errorf("ERROR Message %d", 2);
		break;
	default:
		errorf("Invalid test case");
		break;
	}

	return 0;
}
