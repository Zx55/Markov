#pragma once
#ifndef _MY_LIB_H
#define _MY_LIB_H


#define TABLESIZE 909859
#define HASHSEED 131
#define SUFLISTNUM 64
#define ARTICLESIZE 6280000
#define BUFSIZE 64
#define WORDLENGTH 81
#define is_blank(x) (((x)==' '||(x)=='\t')?1:0)
#define TRUE 1


// Suffix
struct suf_node;
typedef struct suf_node SufNode;
typedef SufNode *SufList;

// Elements of Hash Table
struct hash_cell;
typedef struct hash_cell Cell;

// Hash Table
struct hash_tbl;
typedef struct hash_tbl HashTblRecord;
typedef HashTblRecord *HashTbl;


inline HashTbl create_map(char *beg1, char *beg2);
inline void generate_article(HashTbl h, const char *beg1, const char *beg2);
inline HashTbl create_hashtbl();
inline char *read_article();
inline void write_article(char *article, int size);
inline void add_suf(HashTbl h, int pre_pos, int cur_pos);
inline int read_word(char *dst, const char *src, int p_src);
inline int skip_blank(const char *src, int p_src);
inline int get_pos(const HashTbl h, const char *s1, const char *s2, int tbl_size);
inline int get_suf(const HashTbl h, int pos, double *seed);
inline int hash(const char *s1, const char *s2, int tbl_size);
inline double rrand(double *seed);
inline void clean_memory(HashTbl h);


struct suf_node
{
	int list[SUFLISTNUM];
	int i;
	SufList next;
};

struct hash_cell
{
	char pref1[WORDLENGTH];
	char pref2[WORDLENGTH];
	SufList suf;
	int SufNum;
};

struct hash_tbl
{
	Cell *cells;
};


#endif