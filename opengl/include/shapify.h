#ifndef INCLUDE_SHAPIFY_H
#define INCLUDE_SHAPIFY_H


/* Load original (or reference) image into a texture.
 */
sht_set_original_image(path *char);

/* Render bitstring into a texture and returns a value that represents how 
 * much the texture differs from the original image. 
 */
sht_diff(bitstring *Bitstring, value *float32);

#endif //INCLUDE_SHAPIFY_H