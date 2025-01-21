#ifndef BUCKETSORT_HPP
#define BUCKETSORT_HPP

//Copyright 2024, Bradley Peterson, Weber State University, all rights reserved.
#include <cstdio>
#include <random>
#include <vector>
#include <string>
#include <thread>
#include <mutex>

using std::vector;
using std::string;
using std::stringstream;
using std::mutex;
using std::thread;

//*** Prototypes ***
void sortOneVector(vector<unsigned int>& bucket);
void _sortOneVector(vector<unsigned int>& arr, const unsigned int first, const unsigned int last);
unsigned int _quickSortPartition(vector<unsigned int>& arr, const unsigned int first, const unsigned int last);
void createArray();
unsigned int* getArray();
unsigned int getArrSize();
void deleteArray();
void printArray(const string& msg);
void printAllBuckets(const string& msg);
void pressEnterToContinue();
void step1();
void step2();
void step3();

//***GLOBAL VARIABLES***  (These are global as they will help with an upcoming multithreaded assignment)
unsigned int numBuckets{ 0 };
unsigned int numThreads{ 0 };
const unsigned int UINTMAX = 4294967295;
unsigned int* arr{ nullptr };
unsigned int arrSize{ 0 };
vector<unsigned int>* buckets{ nullptr };
unsigned int currentBucket{ 0 };

bool useMultiThreading{ false }; // To turn off multithreading for any debugging purposes, set this to false.


mutex ourMutex;

void step1() {
  // TODO: Iterate through all values in the array.
  // Determine which bucket an array value should go into, 
  // then assign that array value into that bucket.
  //buckets[computedBucket].push_back(arr[i]);

  //find range per bucket
  unsigned int rangePer = UINTMAX/numBuckets + 1;
  unsigned int bucketNum {0};
  //iterating through the array
  for(unsigned int i = 0; i < arrSize; i++){


      //if there is only one bucket, put all entries into that bucket. otherwise sort into according buckets
      if(numBuckets == 1){
          bucketNum = 0;
      } else {
          //find the bucket to put the entry in
          bucketNum = arr[i] / rangePer;
      }//end if/else statement

      //put entry in its appropriate bucket
      buckets[bucketNum].push_back(arr[i]);
  }//end for loop iterating through arr
}

void singleThreadedStep2() {
  // Sort the bucket at currentBucket

  // TODO: Iterate numBuckets times
  // Each iteration, sort the ith bucket by using sortOneVector

  //simply iterate numBuckets times and sort each bucket
  for(int i = 0; i < numBuckets; i++){
      sortOneVector(buckets[i]);
  }
}

void multiThreadedStep2() {
  // TODO: 
  // 
  // Set up a work unit system within an infinite while loop.
  // In a critical region of code (mutex), obtain the next 
  // bucket to sort, then increment the global buckets counter 
  // variable (it should have been previously initialized to zero).
  // After the critical region of code, see if the bucket to work on is 
  // an actual bucket and not out of bounds. If it is out of bounds,
  // return. If not, sort that bucket.

  int localWorkUnit {0};
  while(true){
      //lock the mutex, get work unit, unlock mutex
      ourMutex.lock();
      localWorkUnit = currentBucket;
      currentBucket += 1;
      ourMutex.unlock();

      //if we have our work unit exceeds the number of buckets, break the while loop
      if(localWorkUnit >= numBuckets){
          break;
      }

      //if the while loop isn't broken, we are working within a bucket
      //sort the bucket at the current workUnit
      sortOneVector(buckets[localWorkUnit]);
  }

}

void step3() {
  // TODO: Copy all items out of all buckets back out to the array.
  // This requires three indexes and two loops
  // Index of which bucket you are working with
  // Index of a value in that particular bucket
  // Index of your next open spot in the output array

  int k = 0; //this is the array index we will insert at

  //i will correspond to the bucket we are in. j will be the index of that bucket. We will do nested for loops
  //that insert buckets[i][j] into arr[k], and then increment k

  for(unsigned int i = 0; i < numBuckets; i++){
      for(unsigned int j = 0; j < buckets[i].size(); j++){
          arr[k] = buckets[i][j];
          k++;
      }
  }//end nested for loop
}

void singleThreadedBucketSort() {

  printArray("Before Step 1"); //useful for debugging small amounts of numbers.  

  step1();
  printAllBuckets("Step 1 check");

  singleThreadedStep2();
  printAllBuckets("Step 2 check");

  step3();

  printArray("After Step 3"); //useful for debugging small amounts of numbers.  
}



void multiThreadedBucketSort() {

  printArray("Before Step 1"); //useful for debugging small amounts of numbers.  

  step1();
  printAllBuckets("Step 1 check");

  // TODO:
  // Set the currentBucket global variable to 0, this tracks what bucket to work on next.
  currentBucket = 0;

  // TODO:
  // Create array of thread trackers
  // Launch/fork child threads on multiThreadedStep2, the goal is a bunch of "thread(multiThreadedStep2)" calls
  // Join all child threads
  // Delete array of thread trackers

  //create array
  thread* threadTrackers = new thread[numThreads];

  //fork
  for(unsigned int i = 0; i < numThreads; i++){
      threadTrackers[i] = thread(multiThreadedStep2);
  }

  //join
  for(unsigned int i = 0; i < numThreads; i++){
      threadTrackers[i].join();
  }

  //delete threadTrackers
  delete[] threadTrackers;

  
  printAllBuckets("Step 2 check");

  step3();

  printArray("After Step 3"); //useful for debugging small amounts of numbers.  
}

// The function you want to use.  Just pass in a vector, and this will sort it.
void sortOneVector(vector<unsigned int>& bucket) {
  _sortOneVector(bucket, 0u, (unsigned int)bucket.size());
}

// A function used by sortOneVector().  You won't call this function.
void _sortOneVector(vector<unsigned int>& bucket, const unsigned int first, const unsigned int last) {
  //first is the first index
  //last is the one past the last index (or the size of the array
  //if first is 0)

  if (first < last) {
    //Get this subarray into two other subarrays, one smaller and one bigger
    unsigned int pivotLocation = _quickSortPartition(bucket, first, last);
    //printf("first: %u last: %u pivotLocation: %u\n", first, last, pivotLocation);
    _sortOneVector(bucket, first, pivotLocation);
    _sortOneVector(bucket, pivotLocation + 1u, last);
  }
}

// A function used by sortOneVector().  You won't call this function.
unsigned int _quickSortPartition(vector<unsigned int>& arr, const unsigned int first, const unsigned int last) {
  auto pivotData = arr[first];
  auto smallIndex = first;

  unsigned int temp;

  for (unsigned int index = first + 1; index < last; index++) {
    if (arr[index] < pivotData) {
      smallIndex++;
      //swap the two
      //printf("Swapping\n");
      temp = arr[smallIndex];
      arr[smallIndex] = arr[index];
      arr[index] = temp;
    }
  }
  
  //Move pivot into the sorted location
  temp = arr[first];
  arr[first] = arr[smallIndex];
  arr[smallIndex] = temp;

  //Tell where the pivot is
  return smallIndex;

}

// A function to create and load the array with random values.  The tests call this method, you won't call it directly.
void createArray() {
  arr = new unsigned int[arrSize];

  //std::random_device rd;
  //std::mt19937 gen(rd());
  std::mt19937 gen(0);
  std::uniform_int_distribution<unsigned long> dis(0, UINTMAX);

  for (unsigned int i = 0; i < arrSize; i++) {
    arr[i] = dis(gen);
  }
}

unsigned int* getArray()  {
  return arr;
}

unsigned int getArrSize() {
  return arrSize;
}

// A function to delete the array
void deleteArray() {
  delete[] arr;
}

// Print the array in hexadecimal.  Printing in hex is beneficial for the next function, printAllBuckets()
void printArray(const string& msg) {
  if (arrSize <= 100) {
    printf("%s\n", msg.c_str());
    for (unsigned int i = 0; i < arrSize; i++) {
      printf("%08x ", arr[i]);
    }
    printf("\n");
  }
}

// A function to determine how many threads to use on a given machine, depending on its number of cores
unsigned int getNumThreadsToUse() {
  unsigned int numThreadsToUse{ 0 };

  if (useMultiThreading) {
    //Find out how many threads are supported
    unsigned int threadsSupported = std::thread::hardware_concurrency();
    printf("This machine has %d cores.\n", threadsSupported);
    if (threadsSupported == 1 && numBuckets > 1) {
      numThreadsToUse = 2;
    }
    else if (numBuckets < threadsSupported) {
      numThreadsToUse = numBuckets;
    }
    else {
      numThreadsToUse = threadsSupported;
    }
    printf("For the upcoming problem, %d threads will be used\n", numThreadsToUse);
  }
  else {
    numThreadsToUse = 1;
  }
  return numThreadsToUse;
}

// A function to print the array in hexadecimal.  Hex is incredibly useful as an output over base 10/decimal.
// For example, suppose numBuckets = 2.  Then bucket 0 should have all values starting with digit 0-7, and bucket 1 should have all values starting with digit 8-f.
// Also, suppose numBuckets = 4.  Bucket = 0's first digits should be 0-3, bucket 1's first digits should be 4-7, bucket 2's first digits should be 8-b, bucket 3's first digits should be c-f
void printAllBuckets(const string& msg) {

  //Displays the contents of all buckets to the screen.
  if (arrSize <= 100) {
    printf("%s\n", msg.c_str());
    // just uncomment this code when you have arr properly declared as a data member
    printf("******\n");
    for (unsigned int bucketIndex = 0; bucketIndex < numBuckets; bucketIndex++) {
      printf("bucket number %d\n", bucketIndex);
      for (unsigned int elementIndex = 0; elementIndex < buckets[bucketIndex].size(); elementIndex++) {
        printf("%08x ", buckets[bucketIndex][elementIndex]);

      }
      printf("\n");
    }
    printf("\n");
  }
}

#endif

