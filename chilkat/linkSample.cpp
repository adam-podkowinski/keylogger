// LinkSample.cpp : Defines the entry point for the console application.
//

// This example does nothing more than include a number of Chilkat headers
// and instantiating objects for the purpose of demonstrating a clean build
// and link (i.e. there are no unresolved externals).

#include <stdio.h>
	
#include "include/CkZip.h"
#include "include/CkFtp2.h"
#include "include/CkMailMan.h"
#include "include/CkXml.h"
#include "include/CkPrivateKey.h"
#include "include/CkRsa.h"
#include "include/CkHttp.h"
#include "include/CkMime.h"
#include "include/CkMht.h"
#include "include/CkSsh.h"
#include "include/CkSFtp.h"

void DoNothing(void)
{
	// Instantiate the objects...
	CkZip zip;
	CkMailMan mailman;
	CkFtp2 ftp2;
	CkXml xml;
	CkPrivateKey privKey;
	CkRsa rsa;
	CkHttp http;
	CkMime mime;
	CkMht mht;
	CkSsh ssh;
	CkSFtp sftp;
	
	printf("Zip version: %s\n",zip.version());
}

int main(int argc, const char* argv[])
{
	DoNothing();

	return 0;
}

