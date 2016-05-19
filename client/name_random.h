#pragma once

namespace namesvc {
struct name_random_t;
name_random_t * name_random_create(const char * file);
void            name_random_destroy(name_random_t * nr);
const   char *  name_random(name_random_t * nr, std::string & namebuff, int type = 0);

}
