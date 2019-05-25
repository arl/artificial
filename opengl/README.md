
This directory contains the opengl C library containing fast GPU-rendering code
for artificial project.

The `sht` prefix of exported C functions stands for _shapify triangles_ and is 
tightly coupled with `pkg/shapify` Go package, evolving an original image by
rendering genetic algorithms bitstring coding for a set of colored triangles.

The `art` prefix stands for _artificial_ and gathers more general-purpose
functions.

The current external API (called from Go code) is as follows:

ALL exported functions returns an error code, return values are passed
by pointer as argument.

/* Render a single image.
 */
sht_render_image()

/* Load original (or reference) image into a texture.
 */
sht_set_original_image(path *char)

/* Render bitstring into a texture and returns a value that represents how 
 * much the texture differs from the original image. 
 */
sht_diff(bitstring *Bitstring, value *float32)