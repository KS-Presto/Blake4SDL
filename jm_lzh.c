//===========================================================================
//
//                  LZHUFF COMPRESSION ROUTINES
//                          VERSION 1.0
//
//              Compression algorithm by Haruhiko OKUMURA
//                      Implementation by Jim T. Row
//
//
//   Copyright (c) 1992 -
//
//===========================================================================


#include "3d_def.h"


#define EXIT_OK         0
#define EXIT_FAILED     -1

//
// LZSS Parameters
//
#define N               4096        // size of string buffer
#define F               30          // size of look-ahead buffer
#define THRESHOLD       2
#define NIL             N           // end of tree's node

//
// Huffman coding parameters
//
#define N_CHAR          (256 - THRESHOLD + F)   // character code (= 0..N_CHAR - 1)
#define T               (N_CHAR * 2 - 1)        // size of table
#define R               (T - 1)                 // root position
#define MAX_FREQ        0x8000                  // update when cumulative frequency reaches to this value


static void StartHuff (void);
static void Reconst (void);
static void Update (int c);


static void DeleteNode (int p);
static void InsertNode (int r);
static void InitTree (void);
static void PutCode (void **outfile, int l, uint16_t c);
static void EncodeChar (void **outfile, uint16_t c);
static void EncodePosition (void **outfile, uint16_t c);
static void EncodeEnd (void **outfile);

static int  GetByte (void **infile, uint32_t *CompressLength);
static int  GetBit (void **infile, uint32_t *CompressLength);
static int  DecodeChar (void **infile, uint32_t *CompressLength);
static int  DecodePosition (void **infile, uint32_t *CompressLength);

static void WritePtr (void **outfile, byte data);
static int  ReadPtr (void **infile);


//
// pointing children nodes (son[], son[] + 1)
//
uint16_t code,len;
uint32_t textsize,codesize,datasize;

int16_t  *son;

//
// pointing parent nodes.
// area [T..(T + N_CHAR - 1)] are pointers for leaves
//
int16_t  *prnt;
uint16_t *freq;         // cumulative freq table
byte     *text_buf;

//
// compression variables
//
int16_t  *lson,*rson,*dad;

int      match_position,match_length;
uint16_t putbuf;
uint16_t putlen;

//
// Tables for encoding/decoding upper 6 bits of
// sliding dictionary pointer
//
// encoder table
//

byte p_len[64] =
{
    0x03, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
};

byte p_code[64] =
{
    0x00, 0x20, 0x30, 0x40, 0x50, 0x58, 0x60, 0x68,
    0x70, 0x78, 0x80, 0x88, 0x90, 0x94, 0x98, 0x9C,
    0xA0, 0xA4, 0xA8, 0xAC, 0xB0, 0xB4, 0xB8, 0xBC,
    0xC0, 0xC2, 0xC4, 0xC6, 0xC8, 0xCA, 0xCC, 0xCE,
    0xD0, 0xD2, 0xD4, 0xD6, 0xD8, 0xDA, 0xDC, 0xDE,
    0xE0, 0xE2, 0xE4, 0xE6, 0xE8, 0xEA, 0xEC, 0xEE,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
    0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};


//
// decompression variables
//
// decoder table
//
byte d_code[256] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
    0x0C, 0x0C, 0x0C, 0x0C, 0x0D, 0x0D, 0x0D, 0x0D,
    0x0E, 0x0E, 0x0E, 0x0E, 0x0F, 0x0F, 0x0F, 0x0F,
    0x10, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11,
    0x12, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x13,
    0x14, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x15,
    0x16, 0x16, 0x16, 0x16, 0x17, 0x17, 0x17, 0x17,
    0x18, 0x18, 0x19, 0x19, 0x1A, 0x1A, 0x1B, 0x1B,
    0x1C, 0x1C, 0x1D, 0x1D, 0x1E, 0x1E, 0x1F, 0x1F,
    0x20, 0x20, 0x21, 0x21, 0x22, 0x22, 0x23, 0x23,
    0x24, 0x24, 0x25, 0x25, 0x26, 0x26, 0x27, 0x27,
    0x28, 0x28, 0x29, 0x29, 0x2A, 0x2A, 0x2B, 0x2B,
    0x2C, 0x2C, 0x2D, 0x2D, 0x2E, 0x2E, 0x2F, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
};

byte d_len[256] =
{
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
};

uint16_t getbuf;
uint16_t getlen;


/*
=============================================================================

                COMPRESSION & DECOMPRESSION ROUTINES

=============================================================================
*/


/*
==========================
=
= LZH_Startup
=
==========================
*/

bool LZH_Startup (void)
{
    if (son)
        return true;

    son = SafeMalloc(T * sizeof(*son));
    prnt = SafeMalloc((T + N_CHAR) * sizeof(*prnt));
    freq = SafeMalloc((T + 1) * sizeof(*freq));
    text_buf = SafeMalloc((N + F - 1) * sizeof(*text_buf));

    lson = SafeMalloc((N + 1) * sizeof(*lson));
    rson = SafeMalloc((N + 257) * sizeof(*rson));
    dad = SafeMalloc((N + 1) * sizeof(*dad));

    return true;
}


/*
==========================
=
= LZH_Shutdown
=
==========================
*/

void LZH_Shutdown (void)
{
    free (son);
    son = NULL;

    free (prnt);
    prnt = NULL;

    free (freq);
    freq = NULL;

    free (text_buf);
    text_buf = NULL;

    free (lson);
    lson = NULL;

    free (rson);
    rson = NULL;

    free (dad);
    dad = NULL;
}


/*
==========================
=
= StartHuff
=
= Initialize freq tree
=
==========================
*/

static void StartHuff (void)
{
    int i,j;

    for (i = 0; i < N_CHAR; i++)
    {
        freq[i] = 1;
        son[i] = i + T;
        prnt[i + T] = i;
    }

    i = 0;
    j = N_CHAR;

    while (j <= R)
    {
        freq[j] = freq[i] + freq[i + 1];
        son[j] = i;
        prnt[i] = prnt[i + 1] = j;
        i += 2;
        j++;
    }

    freq[T] = 0xffff;
    prnt[R] = 0;

    putbuf = putlen = match_position = match_length = 0;
}


/*
==========================
=
= Reconst
=
= Reconstruct freq tree
=
==========================
*/

static void Reconst (void)
{
    int      i,j,k;
    unsigned f,l;

//
// halven cumulative freq for leaf nodes
//
    j = 0;

    for (i = 0; i < T; i++)
    {
        if (son[i] >= T)
        {
            freq[j] = (freq[i] + 1) / 2;
            son[j] = son[i];
            j++;
        }
    }

//
// make a tree : first, connect children nodes
//
    for (i = 0, j = N_CHAR; j < T; i += 2, j++)
    {
        k = i + 1;
        f = freq[j] = freq[i] + freq[k];

        for (k = j - 1; f < freq[k]; k--);

        k++;
        l = (j - k) * 2;

        memmove (&freq[k + 1],&freq[k],l);
        freq[k] = f;

        memmove (&son[k + 1],&son[k],l);
        son[k] = i;
    }

//
// connect parent nodes
//
    for (i = 0; i < T; i++)
    {
        k = son[i];

        if (k >= T)
            prnt[k] = i;
        else
            prnt[k] = prnt[k + 1] = i;
    }
}


/*
==========================
=
= Update
=
= Update freq tree
=
==========================
*/

static void Update (int c)
{
    int i,j,k,l;

    if (freq[R] == MAX_FREQ)
        Reconst ();

    c = prnt[c + T];

    do
    {
        k = ++freq[c];

        //
        // swap nodes to keep the tree freq-ordered
        //
        l = c + 1;

        if (k > freq[l])
        {
            while (k > freq[++l]);

            l--;
            freq[c] = freq[l];
            freq[l] = k;

            i = son[c];
            prnt[i] = l;

            if (i < T)
                prnt[i + 1] = l;

            j = son[l];
            son[l] = i;

            prnt[j] = c;

            if (j < T)
                prnt[j + 1] = c;

            son[c] = j;

            c = l;
        }

        c = prnt[c];

    } while (c);    // do it until reaching the root
}


/*
==========================
=
= WritePtr
=
==========================
*/

void WritePtr (void **outfile, byte data)
{
    byte **ptr = (byte **)outfile;

    **ptr = data;
    (*ptr)++;
}


/*
==========================
=
= ReadPtr
=
= Returns the char read in
=
==========================
*/

int ReadPtr (void **infile)
{
    int  value;
    byte **ptr = (byte **)infile;

    value = (byte)**ptr;
    (*ptr)++;

    return value;
}


/*
=============================================================================

                        COMPRESSION ROUTINES

=============================================================================
*/


/*
==========================
=
= DeleteNode
=
= Deleting node from the tree
=
==========================
*/

static void DeleteNode (int p)
{
    int q;

    if (dad[p] == NIL)
        return;         // unregistered

    if (rson[p] == NIL)
        q = lson[p];
    else if (lson[p] == NIL)
        q = rson[p];
    else
    {
        q = lson[p];

        if (rson[q] != NIL)
        {
            do
            {
                q = rson[q];

            } while (rson[q] != NIL);

            rson[dad[q]] = lson[q];
            dad[lson[q]] = dad[q];
            lson[q] = lson[p];
            dad[lson[p]] = q;
        }

        rson[q] = rson[p];
        dad[rson[p]] = q;
    }

    dad[q] = dad[p];

    if (rson[dad[p]] == p)
        rson[dad[p]] = q;
    else
        lson[dad[p]] = q;

    dad[p] = NIL;
}


/*
==========================
=
= InsertNode
=
= Inserting node in the tree
=
==========================
*/

static void InsertNode (int r)
{
    int      i,p,cmp;
    byte     *key;
    unsigned c;

    cmp = 1;
    key = &text_buf[r];
    p = N + 1 + key[0];
    rson[r] = lson[r] = NIL;
    match_length = 0;

    while (1)
    {
        if (cmp >= 0)
        {
            if (rson[p] != NIL)
                p = rson[p];
            else
            {
                rson[p] = r;
                dad[r] = p;
                return;
            }
        }
        else
        {
            if (lson[p] != NIL)
                p = lson[p];
            else
            {
                lson[p] = r;
                dad[r] = p;
                return;
            }
        }

        for (i = 1; i < F; i++)
        {
            cmp = key[i] - text_buf[p + i];

            if (cmp)
                break;
        }

        if (i > THRESHOLD)
        {
            if (i > match_length)
            {
                match_position = ((r - p) & (N - 1)) - 1;
                match_length = i;

                if (match_length >= F)
                    break;
            }

            if (i == match_length)
            {
                c = ((r - p) & (N - 1)) - 1;

                if (c < match_position)
                    match_position = c;
            }
        }
    }

    dad[r] = dad[p];
    lson[r] = lson[p];
    rson[r] = rson[p];
    dad[lson[p]] = r;
    dad[rson[p]] = r;

    if (rson[dad[p]] == p)
        rson[dad[p]] = r;
    else
        lson[dad[p]] = r;

    dad[p] = NIL;   // remove p
}


/*
==========================
=
= InitTree
=
==========================
*/

static void InitTree (void)
{
    int i;

    for (i = N + 1; i <= N + 256; i++)
        rson[i] = NIL;      // root

    for (i = 0; i < N; i++)
        dad[i] = NIL;       // node
}


/*
==========================
=
= PutCode
=
= Output c bits
=
==========================
*/

static void PutCode (void **outfile, int l, uint16_t c)
{
    putbuf |= c >> putlen;
    putlen += l;

    if (putlen >= 8)
    {
        WritePtr (outfile,putbuf >> 8);

        codesize++;
        putlen -= 8;

        if (putlen >= 8)
        {
            WritePtr (outfile,putbuf);

            codesize++;
            putlen -= 8;
            putbuf = c << (l - putlen);
        }
        else
            putbuf <<= 8;
    }
}


/*
==========================
=
= EncodeChar
=
==========================
*/

static void EncodeChar (void **outfile, uint16_t c)
{
    int i,j,k;

    i = j = 0;
    k = prnt[c + T];

    //
    // search connections from leaf node to the root
    //
    do
    {
        i >>= 1;

        //
        // if node's address is odd, output 1 else output 0
        //
        if (k & 1)
            i += MAX_FREQ;

        j++;

        k = prnt[k];

    } while (k != R);

    PutCode (outfile,j,i);

    code = i;
    len = j;
    Update (c);
}


/*
==========================
=
= EncodePosition
=
==========================
*/

static void EncodePosition (void **outfile, uint16_t c)
{
    int i;

    //
    // output upper 6 bits with encoding
    //
    i = c >> 6;
    PutCode (outfile,p_len[i],(uint16_t)p_code[i] << 8);

    //
    // output lower 6 bits directly
    //
    PutCode (outfile,6,(c & 0x3f) << 10);
}


/*
==========================
=
= EncodeEnd
=
==========================
*/

static void EncodeEnd (void **outfile)
{
    if (putlen)
    {
        WritePtr (outfile,putbuf >> 8);

        codesize++;
    }
}


/*
=============================================================================

                    DECOMPRESSION ROUTINES

=============================================================================
*/


/*
==========================
=
= GetByte
=
==========================
*/

static int GetByte (void **infile, uint32_t *CompressLength)
{
    int i;

    while (getlen <= 8)
    {
        if (*CompressLength)
        {
            i = ReadPtr(infile);

            (*CompressLength)--;
        }
        else
            i = 0;

        getbuf |= i << (8 - getlen);
        getlen += 8;
    }

    i = getbuf;
    getbuf <<= 8;
    getlen -= 8;

    return i >> 8;
}


/*
==========================
=
= GetBit
=
==========================
*/

static int GetBit (void **infile, uint32_t *CompressLength)
{
    int i;

    while (getlen <= 8)
    {
        if (*CompressLength)
        {
            i = ReadPtr(infile);

            (*CompressLength)--;
        }
        else
            i = 0;

        getbuf |= i << (8 - getlen);
        getlen += 8;
    }

    i = getbuf;

    getbuf <<= 1;
    getlen--;

    return i >= MAX_FREQ;
}


/*
==========================
=
= DecodeChar
=
==========================
*/

static int DecodeChar (void **infile, uint32_t *CompressLength)
{
    unsigned c;

    c = son[R];

//
// start searching tree from the root to leaves
// choose node #(son[]) if input bit == 0
// else choose #(son[]+1) (input bit == 1)
//
    while (c < T)
    {
        c += GetBit(infile,CompressLength);
        c = son[c];
    }

    c -= T;
    Update (c);

    return c;
}


/*
==========================
=
= DecodePosition
=
==========================
*/

static int DecodePosition (void **infile, uint32_t *CompressLength)
{
    int      i,j;
    unsigned c;

    //
    // decode upper 6 bits from given table
    //
    i = GetByte(infile,CompressLength);
    c = (uint16_t)d_code[i] << 6;
    j = d_len[i];

    //
    // input lower 6 bits directly
    //
    j -= 2;

    while (j--)
        i = (i << 1) + GetBit(infile,CompressLength);

    return c | (i & 0x3f);
}


/*
=============================================================================

                            EXTERNAL REFERENCED
                    COMPRESSION & DECOMPRESSION ROUTINES

=============================================================================
*/


/*
==========================
=
= LZH_Decompress
=
==========================
*/

int32_t LZH_Decompress (void *infile, void *outfile, uint32_t OriginalLength, uint32_t CompressLength)
{
    int     i,j,k,r,c;
    int32_t count;

    datasize = textsize = OriginalLength;
    getbuf = getlen = 0;

    if (!textsize)
        return 0;

    StartHuff ();

    for (i = 0; i < N - F; i++)
        text_buf[i] = ' ';

    r = N - F;

    for (count = 0; count < textsize; )
    {
        c = DecodeChar(&infile,&CompressLength);

        if (c < 256)
        {
            WritePtr (&outfile,c);

            datasize--;         // dec count of bytes to write

            text_buf[r++] = c;
            r &= (N - 1);
            count++;            // inc count of bytes written
        }
        else
        {
            i = (r - DecodePosition(&infile,&CompressLength) - 1) & (N - 1);
            j = c - 255 + THRESHOLD;

            for (k = 0; k < j; k++)
            {
                c = text_buf[(i + k) & (N - 1)];

                WritePtr (&outfile,c);

                datasize--;         // dec count of bytes to write

                text_buf[r++] = c;
                r &= (N - 1);
                count++;            // inc count of bytes written
            }
        }
    }

    return count;
}


/*
==========================
=
= LZH_Compress
=
==========================
*/

int32_t LZH_Compress (void *infile, void *outfile, uint32_t DataLength)
{
    int16_t i,c,len,r,s,last_match_length;

    textsize = DataLength;

    if (!textsize)
        return 0;

    getbuf = getlen = 0;
    textsize = 0;   // rewind and rescan
    codesize = 0;
    datasize = 0;   // init our counter of ReadData

    StartHuff ();
    InitTree ();

    s = 0;
    r = N - F;

    for (i = s; i < r; i++)
        text_buf[i] = ' ';

    for (len = 0; len < F && DataLength > datasize; len++)
    {
        c = ReadPtr(&infile);

        datasize++;       // dec num of bytes to compress
        text_buf[r + len] = c;
    }

    textsize = len;

    for (i = 1; i <= F; i++)
        InsertNode (r - i);

    InsertNode (r);

    do
    {
        if (match_length > len)
            match_length = len;

        if (match_length <= THRESHOLD)
        {
            match_length = 1;
            EncodeChar (&outfile,text_buf[r]);
        }
        else
        {
            EncodeChar (&outfile,255 - THRESHOLD + match_length);
            EncodePosition (&outfile, match_position);
        }

        last_match_length = match_length;

        for (i = 0; i < last_match_length && DataLength > datasize; i++)
        {
            c = ReadPtr(&infile);

            datasize++;

            DeleteNode (s);
            text_buf[s] = c;

            if (s < F - 1)
                text_buf[s + N] = c;

            s = (s + 1) & (N - 1);
            r = (r + 1) & (N - 1);

            InsertNode (r);
        }

        textsize += i;

        while (i++ < last_match_length)
        {
            DeleteNode (s);

            s = (s + 1) & (N - 1);
            r = (r + 1) & (N - 1);

            if (--len)
                InsertNode(r);
        }

    } while (len > 0);

    EncodeEnd (&outfile);

    return codesize;
}
