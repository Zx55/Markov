/*
**
**    Copyright (c) 2018, Zx55.
**    Homepage: https://github.com/Zx55/Markov
** 
** This program is free software; you can redistribute it and/or modify 
** it under the terms of the GNU General Public License as published by 
** the Free Software Foundation; either version 3, or (at your option) 
** any later version.
**
** This program is distributed in the hope that it will be useful, but 
** WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
** or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
** for more details.
**
** You should have received a copy of the GNU General Public License
** along with GCC; see the file COPYING3.  If not see
** <http://www.gnu.org/licenses/>.
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib.h"


int main()
{
	HashTbl h;
	char beg1[WORDLENGTH];
	char beg2[WORDLENGTH];

	h = create_map(beg1, beg2);
	generate_article(h, beg1, beg2);
	
	clean_memory(h);
	return 0;
}