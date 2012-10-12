/* 
 * File:	environ.c
 * Module:  
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:
 * Comment: 
 * Copyright(c) 2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#define ENV_MAX 8

char * env_tab[ENV_MAX];

const unsigned short env_max = ENV_MAX;

const char **environ = (const char **)env_tab;

