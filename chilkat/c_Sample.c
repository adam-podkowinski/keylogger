
	
#include "include/C_CkSocket.h"

void DoNothing(void)
{
    HCkSocket socket;

    socket = CkSocket_Create();

    CkSocket_Dispose(socket);
}

int main(int argc, const char* argv[])
{
	DoNothing();

	return 0;
}

