#include <sys/stat.h>

#include <errno.h>

#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

int
main (int argc, char** argv)
{
	struct stat s;
	char *dev;
	int fd, rv;

#if 0
	bzero(&s, sizeof(s));

	fd = open(argv[1], O_RDWR, 0644);
	printf("fd=%d errno=%d\n", fd, errno);

	rv = fstat(fd, &s);
	printf("rv=%d errno=%d\n", rv, errno);

	printf("1) %x\n", s.st_dev);
	printf("1) %x\n", s.st_rdev);
	printf("2) %u\n", s.st_ino);
#else
	dev = devname(0xA902, S_IFCHR);

	printf("2) %s\n", dev);
#endif
	//close(fd);

	return 0;
}
