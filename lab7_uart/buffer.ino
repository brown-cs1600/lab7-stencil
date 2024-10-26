/*
 * Code to support a circular buffer of 31 (32 - 1) elements
 */
const int bufSize = 32;
volatile byte buf[bufSize];
volatile int iStart = 0;
volatile int iEnd = 0;

/*
 * Returns true if the buffer is empty and false otherwise
 * Assumes the buffer is not overful (contains at most 31 elements)
 */
bool bufIsEmpty() {
  return ____; // TODO
}

/*
 * Removes and returns an item from the front of the buffer
 * Assumes buffer is not empty
 */
byte bufRemove() {
  byte ret = buf[iStart];
  iStart = ____; // TODO
  return ret;
}

/*
 * Adds an item to the end of the buffer
 * Assumes buffer is not full (has fewer than 31 elements)
 */
void bufAdd(byte data) {
  buf[iEnd] = data;
  iEnd = ____; // TODO
}

/*
 * Returns the number of items currently in the buffer
 */
int bufLength() {
  if (iEnd >= iStart) {
    return iEnd - iStart;
  } else {
    return iEnd + bufSize - iStart;
  }
}

/*
 * Returns an element i positions after the front of the buffer
 * Assumes i is less than the length of the buffer
 */
byte bufGet(int i) {
  return buf[(iStart + i) % bufSize];
}