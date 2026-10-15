#include <errno.h>
#include <stddef.h>
#include <fcntl.h>    
#include <string.h>
#include <unistd.h>  
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include "../include/opcp.h"
#define CHUNK_SIZE (16 * 1024 * 1024) 


int copy(const char *src, const char *dst, size_t buff_size)
{
	char *buffer = (char *) malloc(buff_size * sizeof(char));
	int ret = 0;
	int fd_src = -1;
	int fd_dst = -1;
	off_t last_evict = 0; 
	size_t bytes_since_evict = 0;

	if (buffer == NULL) {
		fprintf(stderr, "opcp: %s\n", strerror(errno));
		return 1;
	}

	fd_src = open(src, O_RDONLY);
	if (fd_src < 0) {
		fprintf(stderr, "opcp: %s %s\n", src, strerror(errno));
		ret = 1;
		goto cleanup;
	}
	
	fd_dst = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd_dst < 0) {
		fprintf(stderr, "opcp: %s %s\n", dst, strerror(errno));
		ret = 1;
		goto cleanup;
	}
	
	posix_fadvise(fd_src, 0, 0, POSIX_FADV_SEQUENTIAL);

	ssize_t bytes_read;

	while (1){

		bytes_read = read(fd_src, buffer, buff_size);

		if (bytes_read == 0) {
			break;
		}
		if (bytes_read < 0) {
			if (errno == EINTR) {
				continue;
			}
			fprintf(stderr, "opcp: %s %s\n", src, strerror(errno));
			ret = 1;
			goto cleanup;
		}
		
		ssize_t len = 0;
		size_t remain = bytes_read;
		char *ptr = buffer;

		while (remain > 0) {
			len = write(fd_dst, ptr, remain);
			if (len < 0) {
				if (errno == EINTR) {
					continue;
				}

				fprintf(stderr, "opcp: %s %s\n", dst, strerror(errno));
				ret = 1;
				goto cleanup;
			}
			remain -= len;
			ptr += len;
			
			// Posix Advise Dont Neeed 
			bytes_since_evict += len;
			
			if (bytes_since_evict >= CHUNK_SIZE) {
				posix_fadvise(fd_src, last_evict, bytes_since_evict, POSIX_FADV_DONTNEED);
				posix_fadvise(fd_dst, last_evict, bytes_since_evict, POSIX_FADV_DONTNEED);
				
				last_evict += bytes_since_evict;
				bytes_since_evict = 0; 
			}
		}
	}

	if (bytes_since_evict > 0) {
		posix_fadvise(fd_src, last_evict, bytes_since_evict, POSIX_FADV_DONTNEED);
		posix_fadvise(fd_dst, last_evict, bytes_since_evict, POSIX_FADV_DONTNEED);
	}

	goto cleanup;

cleanup:                                                                                                       
       if (fd_src >= 0) {                                                                                         
           if (close(fd_src) != 0) {                                                                              
               fprintf(stderr, "opcp: close src: %s\n", strerror(errno));                                         
               ret = 1;                                                                                           
           }                                                                                                      
       }                                                                                                          
       if (fd_dst >= 0) {                                                                                         
           if (close(fd_dst) != 0) {                                                                              
               fprintf(stderr, "opcp: close dst: %s\n", strerror(errno));                                         
               ret = 1;                                                                                           
           }                                                                                                      
       }                                                                                                          
       free(buffer);                                                                                              
       return ret; 
}
