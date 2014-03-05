/*Contains simple utility functions for the wattr project
 *
 *
 */
//Endian agnostic string to uint32_t (very slow, though)
//
inline uint32_t * string_to_uint(uint8_t *str)
{
	uint32_t t = 	((uint32_t)str[0] << 24) |
			((uint32_t)str[1] << 16) |
			((uint32_t)str[2] << 8 ) |
			 (uint32_t)str[3];
	uint32_t *ia =	(uint32_t*)(void*)str;
	*ia = t;
	return ia;
}

inline uint8_t * uint_to_string(uint32_t *uint);
