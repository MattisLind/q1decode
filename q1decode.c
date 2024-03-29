
#include <stdio.h>
#include <inttypes.h>

#define FM 1
#define Q1 4
#define q1thresh1 39
#define q1thresh2 57
#define q1thresh3 75
#define fmthresh 0
#define mfmthresh1 0
#define mfmthresh2 0
#define postcomp 0.5
#define cwclock 2
#define mfmshort 18.0
#define garbage 0x62
int uencoding = Q1;
int recordSize;
int cnt = 0;



/*
**  ASCII <=> EBCDIC conversion functions
*/

static const unsigned char a2e[256] = {
  0,  1,  2,  3, 55, 45, 46, 47, 22,  5, 37, 11, 12, 13, 14, 15,
  16, 17, 18, 19, 60, 61, 50, 38, 24, 25, 63, 39, 28, 29, 30, 31,
  64, 79,127,123, 91,108, 80,125, 77, 93, 92, 78,107, 96, 75, 97,
  240,241,242,243,244,245,246,247,248,249,122, 94, 76,126,110,111,
  124,193,194,195,196,197,198,199,200,201,209,210,211,212,213,214,
  215,216,217,226,227,228,229,230,231,232,233, 74,224, 90, 95,109,
  121,129,130,131,132,133,134,135,136,137,145,146,147,148,149,150,
  151,152,153,162,163,164,165,166,167,168,169,192,106,208,161,  7,
  32, 33, 34, 35, 36, 21,  6, 23, 40, 41, 42, 43, 44,  9, 10, 27,
  48, 49, 26, 51, 52, 53, 54,  8, 56, 57, 58, 59,  4, 20, 62,225,
  65, 66, 67, 68, 69, 70, 71, 72, 73, 81, 82, 83, 84, 85, 86, 87,
  88, 89, 98, 99,100,101,102,103,104,105,112,113,114,115,116,117,
  118,119,120,128,138,139,140,141,142,143,144,154,155,156,157,158,
  159,160,170,171,172,173,174,175,176,177,178,179,180,181,182,183,
  184,185,186,187,188,189,190,191,202,203,204,205,206,207,218,219,
  220,221,222,223,234,235,236,237,238,239,250,251,252,253,254,255
};

static const unsigned char e2a[256] = {
  0,  1,  2,  3,156,  9,134,127,151,141,142, 11, 12, 13, 14, 15,
  16, 17, 18, 19,157,133,  8,135, 24, 25,146,143, 28, 29, 30, 31,
  128,129,130,131,132, 10, 23, 27,136,137,138,139,140,  5,  6,  7,
  144,145, 22,147,148,149,150,  4,152,153,154,155, 20, 21,158, 26,
  32,160,161,162,163,164,165,166,167,168, 91, 46, 60, 40, 43, 33,
  38,169,170,171,172,173,174,175,176,177, 93, 36, 42, 41, 59, 94,
  45, 47,178,179,180,181,182,183,184,185,124, 44, 37, 95, 62, 63,
  186,187,188,189,190,191,192,193,194, 96, 58, 35, 64, 39, 61, 34,
  195, 97, 98, 99,100,101,102,103,104,105,196,197,198,199,200,201,
  202,106,107,108,109,110,111,112,113,114,203,204,205,206,207,208,
  209,126,115,116,117,118,119,120,121,122,210,211,212,213,214,215,
  216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,
  123, 65, 66, 67, 68, 69, 70, 71, 72, 73,232,233,234,235,236,237,
  125, 74, 75, 76, 77, 78, 79, 80, 81, 82,238,239,240,241,242,243,
  92,159, 83, 84, 85, 86, 87, 88, 89, 90,244,245,246,247,248,249,
  48, 49, 50, 51, 52, 53, 54, 55, 56, 57,250,251,252,253,254,255
};

char ASCIItoEBCDIC(const unsigned char c)
{
  return a2e[c];
}

char EBCDICtoASCII(const unsigned char c)
{
  return e2a[c];
}




unsigned char transposebits (unsigned char in) {
  int i;
  //printf ("\nIN:%02x::", in & 0xff); 
  unsigned char out=0;
  for (i=0;i<8; i++) {
    out = out << 1;
    out |= (0x01 & (in  >> i));
    //printf ("%02x ", out & 0xff);
  }
  //printf ("OUT::%02x\n", out & 0xff);
  return out & 0xff;
}


void process_bit (char bit) {
  static int  data_word = 0;
  static int state = 0;
  static char lastbit; 
  static char mfmword;
  static unsigned char outword;
  static int bitcnt; 
  static int sum;
  static unsigned int crc;
  static int byteCnt;
  static int currentRecordSize;
  static unsigned char buffer[256];
  data_word = data_word << 1;
  data_word |= bit;
  
  //printf("CNT: %05X PROCESS BIT: %08X\n", cnt, data_word);
  if (data_word  == 0x55424954) { // Mark
    //printf("\nCNT: %05X ADDRESS MARK: %08X ", cnt, data_word);
    state = 1; lastbit = 0; mfmword = 0; bitcnt=0; sum=0;
    crc = 0;
    byteCnt = 0;
    currentRecordSize = 2;
    return;
  }
  if (data_word == 0x55424945) { // Mark
    //printf("\nCNT: %05X DATA    MARK: %08X ", cnt, data_word);
    state = 1; lastbit = 1; mfmword = 0; bitcnt=0; sum=0;
    crc = 0x9B;
    byteCnt = 0;
    currentRecordSize = recordSize;
    return;
  }
  //printf ("state = %d bit = %d mfmword=%1X lastbit=%d outword = %02X \n", state, bit, mfmword & 0b11, lastbit, outword);
  if (state == 1) {
    // shift in one bit into mfm word
    mfmword |= bit;
    state = 2;
  } else if (state == 2) {
    mfmword = mfmword << 1;
    mfmword |= bit;
    //printf ("mfmword=%d\n", mfmword & 0x03);
    if ((lastbit == 1) && ((mfmword & 0x3)  == 0b00)) {
      outword = outword << 1;
      outword |= 0;
      lastbit = 0;
      state = 1;
    } else if ((lastbit == 0) && ((mfmword & 0x03) == 0b10)) {
      outword = outword << 1;
      outword |= 0;
      lastbit = 0;
      state = 1;
    }  else if ((mfmword & 0x03) == 0b01) {
      outword = outword << 1;
      outword |= 1;
      lastbit = 1;
      state = 1;
    } else {
      state= 0;
      //printf ("V");
    }
    //printf ("%d", lastbit);
    //if (bitcnt == bitOffset) { 
    //  printf ("%c", 0x7f & EBCDICtoASCII( outword ));
    //}
    bitcnt++;
    if (bitcnt == 8) {
      bitcnt = 0;
      if (byteCnt == currentRecordSize) {
	if ((outword & 0xff) == (crc & 0xff)) {
	  printf("CRC OK\n");
	} else {
	  printf("CRC ERROR\n");
	}
      }
      buffer[byteCnt] = outword & 0xff;
      byteCnt++;
      //printf (" "); 
      outword = outword & 0xff;
      crc += outword & 0xff;
      if (byteCnt <= currentRecordSize) {
	printf ("%02X", outword);
      }
      if (byteCnt==currentRecordSize) {
	printf("\n");
	for (int i=0; i<currentRecordSize;i++) {
	  if ( (buffer[i] >= 32) && (buffer[i] < 127)) {
	    printf ("%c", buffer[i]);
	  } else {
	    printf (" "); 
	  }
	}
	printf("\n");
      }
    }
    mfmword = 0;
    
  }
}


/*
 * Convert Catweasel samples to strings of alternating clock/data bits
 * and pass them to process_bit for further decoding.
 * Ad hoc method using two fixed thresholds modified by a postcomp
 * factor.
 */
void
process_sample(int sample)
{
  static float adj = 0.0;
  int len;

  if (uencoding == FM) {
    if (sample + adj <= fmthresh) {
      /* Short */
      len = 2;
    } else {
      /* Long */
      len = 4;
    }
  } else if (uencoding == Q1) {
    if (sample + adj <= q1thresh1) {
      /* Short */
      len = 2;
    } else if (sample + adj <= q1thresh2) {
      /* Medium */
      len = 3;
    } else if (sample + adj <= q1thresh3) {
      /* Long */
      len = 4;
    } else if (sample + adj <= garbage) {
      len = 5;
    } else {
      len = 6;
    }
    
  } else  {
    if (sample + adj <= mfmthresh1) {
      /* Short */
      len = 2;
    } else if (sample + adj <= mfmthresh2) {
      /* Medium */
      len = 3;
    } else {
      /* Long */
      len = 4;
    }
    
  }
  adj = (sample - (len/2.0 * mfmshort * cwclock)) * postcomp;
  if (len == 6) { cnt++; return; }
  process_bit(1);
  while (--len) process_bit(0);
  cnt ++;
}




int main (int argc, char ** argv) {
  int c;
  if (argc==2) {
    printf ("ARG %s\n", argv[1]);
    sscanf(argv[1], "%d", &recordSize);
    printf ("recordSize=%d\n", recordSize);
  }  else {
    printf("Need one argument. Please specify the record size for the track. Wrong record size will cause the CRC calculation to fail.\n");
    return -1;
  }
  while ((c = getchar()) != -1) {
    process_sample(c & 0x7f);
  }
  return 0;
}
