#ifndef SERVER_H_
#define SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include<commons/config.h>

#include "serverUtils.h"

t_config* iniciar_config(void);
void iterator(char* value);

#endif /* SERVER_H_ */
