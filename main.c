#include "httpclient.h"

int main(int argc , char * argv[])
{
	char resp[256] = {};
	do_get("10.128.106.93", 8181, "/connect?host=local&pid=222", resp, 255);
	printf("resp = %s\n", resp);	
	return 0;

}
