#ifndef __IMAGEPROGMEM
#define __IMAGEPROGMEM 1

#define __Height 6
#define __Width 6

static const uint8_t red[6][6] PROGMEM = {
  { 21,21,20,16,13,13},
  { 9,21,27,22,16,17},
  { 12,16,15,12,15,23},
  { 12,7,6,12,19,17,16},
  { 15,4,6,18,19,4},
  { 53,20,0,2,10,8}
 // { 66,46,18,4,5,14},
  //{ 28,61,71,41,9,4},
  //{ 0,14,46,66,53,21},
  //{ 0,0,2,29,50,47}
};

static const uint8_t blue[6][6] PROGMEM = {
  { 40,41,40,36,34,34},
  { 28,40,47,42,37,38},
  { 35,39,39,36,40,46},
  { 39,34,34,40,45,44},
  { 47,37,37,50,49,34},
  { 89,56,33,36,43,40}
//  { 108,86,58,42,42,49,49,44,42,41} ,
//  { 74,104,115,83,50,42,44,40,42,41} ,
//  { 40,61,92,109,95,61,38,31,46,43} ,
//  { 49,42,49,75,93,89,68,54,33,43}
};

static const uint8_t green[6][6] PROGMEM = {
  { 80,78,75,69,63,61},
  { 68,80,84,75,68,65},
  { 77,80,77,72,71,77},
  { 84,77,74,79,80,74},
  { 94,82,81,91,87,68},
  { 137,104,78,81,84,79}
//  { 158,137,107,89,86,91,87,82,76,74} ,
//  { 124,155,164,131,96,87,86,81,80,78} ,
//  { 92,113,142,160,145,110,83,76,89,82} ,
//  { 100,93,101,125,144,139,117,101,79,86}
};

#endif
