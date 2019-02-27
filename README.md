# artificial

input: 
 - base image
 - dimension of the working canvas

API:
 /*
   base image path
   working canvas dimensions (only power of 2 are accepted so that every bit is
   significative)
 */
 - shapify_init(base string, xcanvas, ycanvas)
	loads base image buffer in memory,
	resize it to the working canvas dimension
