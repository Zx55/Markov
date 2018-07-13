#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lib.h"


// Create a hash table to link prefixes with suffixes according to article.txt.
inline HashTbl create_map(char *beg1, char *beg2)
{
	HashTbl h;
	char *article;
	int cur_pos;
	int pre_pos = -1;
	char buf1[BUFSIZE], buf2[BUFSIZE];
	int i;
	
	h = create_hashtbl();
	
    // Read article from article.txt.
    article = read_article();
    
    // Read first word in the article.
	i = read_word(buf1, article, 0);
	strcpy(beg1, buf1);

	while (TRUE)
	{
		i = skip_blank(article, i);

        // Add -1 to previous prefix's suffix list if article ends.
		if (article[i] == '\0')
		{
			add_suf(h, pre_pos, -1);
			break;
		}

        // Read the second word.
		i = read_word(buf2, article, i);
		if (pre_pos == -1)
			strcpy(beg2, buf2);

        // Calculate the position of prefixes in the hash table.
		cur_pos = get_pos(h, buf1, buf2, TABLESIZE);

        // Create a new cell if prefixes not found in the hash table.
		if (h->cells[cur_pos].SufNum == 0)
		{
			strcpy(h->cells[cur_pos].pref1, buf1);
			strcpy(h->cells[cur_pos].pref2, buf2);
			h->cells[cur_pos].suf = (SufList)malloc(sizeof(SufNode));
			h->cells[cur_pos].suf->i = 0;
			h->cells[cur_pos].suf->next = NULL;
		}
		++h->cells[cur_pos].SufNum;

        // Add the second prefix to previous prefix's suffix list.
		if (pre_pos != -1)
			add_suf(h, pre_pos, cur_pos);

		strcpy(buf1, buf2);
		pre_pos = cur_pos;
	}

    return h;
}


// Generate a new text based on the original one.
inline void generate_article(HashTbl h, const char *beg1, const char *beg2)
{
	char *article;
	char *pref1, *pref2, *suf; 
	int cur_pos; 
	int suf_hash;
	int total_size, cnt;
	int i, j = 0;
	double seed = 997.0;

    // Write the first two words in the article.
	pref1 = (char *)beg1;
	pref2 = (char *)beg2;
	article = (char*)malloc(sizeof(char) * 2800000);
	for (i = 0; pref1[i] != '\0'; ++i, ++j)
		article[j] = pref1[i];
	article[j++] = ' ';
	for (i = 0; pref2[i] != '\0'; ++i, ++j)
		article[j] = pref2[i];

    // Get the total output from the screen.
	scanf("%d", &total_size);
	for (cnt = 2; cnt < total_size; ++cnt)
	{
        // Calculate the position of prefixes in the hash table.
		cur_pos = get_pos(h, pref1, pref2, TABLESIZE);

        // Choose a suffix randomly.
		suf_hash = get_suf(h, cur_pos, &seed);

        // End of text.
		if (suf_hash == -1)
		{
			article[j] = '\0';
			break;
		}
        
        // Write the suffix in the article.
		article[j++] = ' ';
		suf = h->cells[suf_hash].pref2;
		for (i = 0; suf[i] != '\0'; ++i, ++j)
			article[j] = suf[i];

		pref1 = pref2;
		pref2 = suf;
	}

    // Write the text to markov.txt.
	write_article(article, j + 1);
}


// Create a hash table.
inline HashTbl create_hashtbl()
{
    HashTbl h;
    int i;

    h = (HashTbl)malloc(sizeof(HashTblRecord));
    if (h == NULL)
        return NULL;

	h->cells = (Cell *)malloc(sizeof(Cell) * TABLESIZE); //创建散列表
	for (i = 0; i < TABLESIZE; ++i)
		h->cells[i].SufNum = 0;

    return h;
}


// Read article from article.txt.
inline char *read_article()
{
    FILE *fp;
    char *article;
    int len;

    fp = fopen("article.txt", "r");
    if (fp == NULL)
        return NULL;

    article = (char *)malloc(sizeof(char) * ARTICLESIZE);
	len = fread(article, sizeof(char), ARTICLESIZE, fp); //读取article
	article[len] = '\0';
    fclose(fp);

    return article;
}


// Write the text to the markov.txt.
inline void write_article(char *article, int size)
{
    FILE *fp;
    
    fp = fopen("markov.txt", "w");
    fwrite(article, sizeof(char), size, fp);
	free(article);

    fclose(fp);
}


// Add the second prefix to previous prefix's suffix list.
inline void add_suf(HashTbl h, int pre_pos, int cur_pos)
{
    SufList cur_sl;

    for (cur_sl = h->cells[pre_pos].suf; cur_sl->next != NULL; cur_sl = cur_sl->next);

    // Create a new suffix list if current one is full.
	if (cur_sl->i == SUFLISTNUM)
	{
		cur_sl->next = (SufList)malloc(sizeof(SufNode));
		cur_sl = cur_sl->next;
		cur_sl->i = 0;
		cur_sl->next = NULL;
	}
	cur_sl->list[cur_sl->i++] = cur_pos;
}


// Read word from the article.
inline int read_word(char *dst, const char *src, int p_src)
{
    int i;

    p_src = skip_blank(src, p_src);

    for (i = 0; !is_blank(src[p_src]); ++i, ++p_src)
		{
			if (src[p_src] == '\0' || src[p_src] == '\r') 
                break;
			dst[i] = src[p_src];
		}
	dst[i] = '\0';

    return p_src;
}


// Skip the blank in the article.
inline int skip_blank(const char *src, int p_src)
{
    for (; is_blank(src[p_src]) || src[p_src] == '\r' || src[p_src] == '\n'; ++p_src);
    
    return p_src;
}


// Calculate the position of prefixes according to their hashes.
inline int get_pos(const HashTbl h, const char *s1, const char *s2, int tbl_size)
{
    int pos, collision_num = 0;

    pos = hash(s1, s2, tbl_size);

    // Solve the conflicts by quadratic probing.
    while (h->cells[pos].SufNum != 0 && (strcmp(h->cells[pos].pref1, s1) || strcmp(h->cells[pos].pref2, s2)))
	{
		pos += 2 * ++collision_num - 1;
		if (pos >= tbl_size)
			pos -= tbl_size;
	}

    return pos;
}


// Choose the suffix randomly.
inline int get_suf(const HashTbl h, int pos, double *seed)
{
    int n;
    int suf_hash;
    int i, p, q;
    SufList cur_sl;

    if (h->cells[pos].SufNum == 1)
		n = 0;
	else
		n = (int)(rrand(seed) * h->cells[pos].SufNum);

	if (n < SUFLISTNUM)
		suf_hash = h->cells[pos].suf->list[n];
	else
	{
		p = n / 64;
		q = n % 64;
		for (cur_sl = h->cells[pos].suf, i = 0; i < p; cur_sl = cur_sl->next, ++i);
		suf_hash = cur_sl->list[q];
	}

    return suf_hash;
}


inline int hash(const char *s1, const char *s2, int tbl_size)
{
    int i;
    int l1, l2, step;
    int pos = 0;

    l1 = strlen(s1);
	l2 = strlen(s2);

	if (l1 > 10)
	{
		step = l1 >> 3;
		for (i = 0; i < l1; i += step) 
            pos = pos * HASHSEED + s1[i];
	}
	else
		for (i = 0; i < l1; ++i) 
            pos = pos * HASHSEED + s1[i];

	if (l2 > 10)
	{
		step = l2 >> 3;
		for (i = 0; i < l2; i += step) 
            pos = pos * HASHSEED + s2[i];
	}
	else
		for (i = 0; i < l2; ++i) 
            pos = pos * HASHSEED + s2[i];
	
    return ((pos & 0x7FFFFFFF) % tbl_size);
}


inline double rrand(double *seed)
{
    double lambda = 3125.0;
    double m = 34359738337.0;
    double r;
    
    *seed = fmod(lambda * (*seed), m);
    r = *seed / m;

    return r;
}


// Clean the memory.
inline void clean_memory(HashTbl h)
{
    int i;
    SufList p, q;

    for (i = 0; i < TABLESIZE; ++i)
    {
        if (h->cells[i].SufNum != 0)
        {
            p = h->cells[i].suf;
            while (p != NULL)
            {
                q = p->next;
                free(p);
                p = q;
            }
        } 
    }

    free(h->cells);
    free(h);
}