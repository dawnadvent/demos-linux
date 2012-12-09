/*
	This file is part of the linuxapi project.
	Copyright (C) 2011, 2012 Mark Veltzer <mark.veltzer@gmail.com>

	The linuxapi package is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	The linuxapi package is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with the GNU C Library; if not, write to the Free
	Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
	02111-1307 USA.
*/

#include <firstinclude.h>
#include <netdb.h> // for getservbyname(3)
#include <stdio.h> // for printf(3), fprintf(3)
#include <stdlib.h> // for EXIT_SUCCESS, EXIT_FAILURE
#include <us_helper.h> // for CHECK_NOT_NULL()

/*
* A simple example to demonstrate getservbyname(3)
*/

int main(int argc,char** argv,char** envp) {
	if(argc!=3) {
		fprintf(stderr, "usage: %s [service-name] [protocol-name]\n",argv[0]);
		return EXIT_FAILURE;
	}
	// getservbyname() - opens the etc.services file and returns the
	// values for the requested service and protocol.
	struct servent *serv;
	CHECK_NOT_NULL(serv=getservbyname(argv[1], argv[2]));
	// Print it.
	printf("Name: %s Port: %d Protocol: %s\n",serv->s_name,ntohs(serv->s_port),serv->s_proto);
	return EXIT_SUCCESS;
}
