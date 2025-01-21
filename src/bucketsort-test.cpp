//Copyright 2024, Bradley Peterson, Weber State University, all rights reserved. (07/2024)

#include "bucketsort.hpp"
#include <cstdio>
#include <chrono>
#include <sstream>
#include <iostream>
#include <string>
#include <cstring>

using std::stringstream;
using std::cout;
using std::cin;
using std::endl;
using std::stoi;

bool runSpeedTests{ true };
bool valgrind_mode{ false };

// A helper function to verify if the sort is correct.  The test code calls this for you.
void testSort(int testNum, int& correct, const string& sortTest, std::chrono::duration<double, std::milli>& diff) {
  double val = diff.count();
  unsigned int* arr = getArray();
  unsigned int arraySize = getArrSize();

  for (unsigned int i = 1; i < arraySize; i++) {
    if (arr[i] < arr[i - 1]) {
      printf("------------------------------------------------------\n");
      printf("SORT TEST %s\n", sortTest.c_str());

      if (val != 0.0) {
        printf("Finished bucket sort in %1.16lf milliseconds\n", diff.count());
      }
      printf("ERROR - This list was not sorted correctly.  At index %d is value %08X.  At index %d is value %08X\n", i - 1, arr[i - 1], i, arr[i]);
      printf("------------------------------------------------------\n");
      return;
    }
  }
  printf("------------------------------------------------------\n");
  printf("SORT TEST %s\n", sortTest.c_str());
  if (val != 0.0) {
    printf("Finished bucket sort in %1.16lf milliseconds\n", diff.count());
  }
  printf("PASSED SORT TEST %s - The list was sorted correctly\n", sortTest.c_str());
  printf("------------------------------------------------------\n");
  correct++;
}


void testSpeedup(int testNum, int& correct, const string& speedupName, double actualSpeedup, double speedupMinimum, double speedupMaximum) {

  if (actualSpeedup < speedupMinimum || speedupMaximum < actualSpeedup) {

    cout << "***Failed Test " << testNum << "***" << endl;
    cout << "The " << speedupName << " speedup should have been between " << speedupMinimum << "x to " << speedupMaximum << "x .The speedup was " << actualSpeedup << "x " << endl << endl;
    return;
  }

  correct++;
  cout << "Passed Test " << testNum << endl << endl;
  cout << "Testing: " << speedupName << ". The speedup was " << actualSpeedup << "x " << endl << endl;
}

bool testSmallSingleThreadedSorts() {

  int testNum = 1;
  int correct = 0;
  std::cout << "--------testSmallSingleThreadedSorts Tests--------" << std::endl;

  std::chrono::duration<double, std::milli> diff{ 0 };

  useMultiThreading = false;
  arrSize = 100;

  numBuckets = 2;
  createArray();
  buckets = new vector<unsigned int>[numBuckets];
  numThreads = getNumThreadsToUse();
  printf("\nStarting bucket sort for listSize = %d, numBuckets = %d, numThreads = %d, number of cores = %d\n", arrSize, numBuckets, numThreads, std::thread::hardware_concurrency());
  auto start = std::chrono::high_resolution_clock::now();
  singleThreadedBucketSort();
  auto end = std::chrono::high_resolution_clock::now();
  diff = end - start;
  testSort(testNum++, correct, "2 buckets", diff); // 1
  delete[] buckets;
  deleteArray();

  numBuckets = 4;
  createArray();
  buckets = new vector<unsigned int>[numBuckets];
  numThreads = getNumThreadsToUse();
  printf("\nStarting bucket sort for listSize = %d, numBuckets = %d, numThreads = %d, number of cores = %d\n", arrSize, numBuckets, numThreads, std::thread::hardware_concurrency());
  singleThreadedBucketSort();
  testSort(testNum++, correct, "4 buckets", diff); // 2
  delete[] buckets;
  deleteArray();

  return testNum - 1 == correct;
}


int testLargeSingleThreadedSort() {

  int testNum = 1;
  int correct = 0;
  std::cout << "--------testLargeSingleThreadedSort Tests--------" << std::endl;

  std::chrono::duration<double, std::milli> diff{ 0 };

  useMultiThreading = false;
  arrSize = 1000000;

  numBuckets = 1;
  createArray();
  buckets = new vector<unsigned int>[numBuckets];
  numThreads = getNumThreadsToUse();
  printf("\nStarting bucket sort for listSize = %d, numBuckets = %d, numThreads = %d, number of cores = %d\n", arrSize, numBuckets, numThreads, std::thread::hardware_concurrency());
  auto start = std::chrono::high_resolution_clock::now();
  singleThreadedBucketSort();
  auto end = std::chrono::high_resolution_clock::now();
  diff = end - start;
  testSort(testNum++, correct, "1 bucket", diff); // 1
  delete[] buckets;
  deleteArray();

  numBuckets = 2;
  createArray();
  buckets = new vector<unsigned int>[numBuckets];
  numThreads = getNumThreadsToUse();
  printf("\nStarting bucket sort for listSize = %d, numBuckets = %d, numThreads = %d, number of cores = %d\n", arrSize, numBuckets, numThreads, std::thread::hardware_concurrency());
  singleThreadedBucketSort();
  testSort(testNum++, correct, "2 buckets", diff); // 2
  delete[] buckets;
  deleteArray();

  numBuckets = 4;
  createArray();
  buckets = new vector<unsigned int>[numBuckets];
  numThreads = getNumThreadsToUse();
  printf("\nStarting bucket sort for listSize = %d, numBuckets = %d, numThreads = %d, number of cores = %d\n", arrSize, numBuckets, numThreads, std::thread::hardware_concurrency());
  singleThreadedBucketSort();
  testSort(testNum++, correct, "4 buckets", diff); // 3
  delete[] buckets;
  deleteArray();

  return testNum - 1 == correct;
}

int testSmallMultiThreadedSorts() {

  int testNum = 1;
  int correct = 0;
  std::cout << "--------testSmallMultiThreadedSorts Tests--------" << std::endl;

  std::chrono::duration<double, std::milli> diff{ 0 };

  useMultiThreading = true;
  arrSize = 100;

  numBuckets = 2;
  createArray();
  buckets = new vector<unsigned int>[numBuckets];
  numThreads = getNumThreadsToUse();
  printf("\nStarting bucket sort for listSize = %d, numBuckets = %d, numThreads = %d, number of cores = %d\n", arrSize, numBuckets, numThreads, std::thread::hardware_concurrency());
  auto start = std::chrono::high_resolution_clock::now();
  multiThreadedBucketSort();
  auto end = std::chrono::high_resolution_clock::now();
  diff = end - start;
  testSort(testNum++, correct, "2 buckets", diff); // 1
  delete[] buckets;
  deleteArray();

  numBuckets = 4;
  createArray();
  buckets = new vector<unsigned int>[numBuckets];
  numThreads = getNumThreadsToUse();
  printf("\nStarting bucket sort for listSize = %d, numBuckets = %d, numThreads = %d, number of cores = %d\n", arrSize, numBuckets, numThreads, std::thread::hardware_concurrency());
  multiThreadedBucketSort();
  testSort(testNum++, correct, "4 buckets", diff); // 2
  delete[] buckets;
  deleteArray();

  return testNum - 1 == correct;
}

int testAll() {

  int testNum = 1;
  int correct = 0;
  std::cout << "--------testAll Tests--------" << std::endl;

  std::chrono::duration<double, std::milli> diff{ 0 };
  double baselineTime{ 9999999.0 };
  double bestMultiThreadedTime{ 9999999.0 };
  int bestMultiThreadedBuckets{ 0 };
  double bestSingleThreadedTime{ 9999999.0 };
  int bestSingleThreadedBuckets{ 0 };
  if (!runSpeedTests) {
    cout << "Not running this with CTest or Valgrind because GitHub actions are too slow." << endl;
  }
  else {
    // Get the baseline, single threaded, 
    arrSize = 4000000;
    numBuckets = 1;
    createArray();
    numThreads = getNumThreadsToUse();
    buckets = new vector<unsigned int>[numBuckets];
    printf("\nStarting quick sort for listSize = %d, numBuckets = %d, numThreads = %d, number of cores = %d\n", arrSize, numBuckets, numThreads, std::thread::hardware_concurrency());
    auto start = std::chrono::high_resolution_clock::now();
    singleThreadedBucketSort();
    auto end = std::chrono::high_resolution_clock::now();
    diff = end - start;
    baselineTime = diff.count();
    testSort(testNum++, correct, "4000000 items in 1 bucket with 1 thread - BASELINE", diff); // 1
    delete[] buckets;
    deleteArray();

    for (int mode = 0; mode < 2; mode++) {

      useMultiThreading = (bool)mode; // Run all tests without multithreading, then run all with multithreading.  

      for (numBuckets = 2; numBuckets <= 1024; numBuckets *= 2) {
        arrSize = 4000000;
        createArray();
        numThreads = getNumThreadsToUse();
        buckets = new vector<unsigned int>[numBuckets];
        printf("\nStarting bucket sort for listSize = %d, numBuckets = %d, numThreads = %d, number of cores = %d\n", arrSize, numBuckets, numThreads, std::thread::hardware_concurrency());
        start = std::chrono::high_resolution_clock::now();
        if (!useMultiThreading) {
          singleThreadedBucketSort();
        }
        else {
          multiThreadedBucketSort();
        }

        end = std::chrono::high_resolution_clock::now();
        diff = end - start;
        if (useMultiThreading && (diff.count() < bestMultiThreadedTime)) {
          bestMultiThreadedTime = diff.count();
          bestMultiThreadedBuckets = numBuckets;
        }
        else if (!useMultiThreading && (diff.count() < bestSingleThreadedTime)) {
          bestSingleThreadedTime = diff.count();
          bestSingleThreadedBuckets = numBuckets;
        }

        stringstream ss;
        ss << arrSize << " items in " << numBuckets << " buckets";
        testSort(testNum++, correct, ss.str(), diff);
        delete[] buckets;
        deleteArray();
      }
    }

    printf("\n-----------------------------------------------------------\n");
    printf("              FINAL RESULTS                      \n");
    printf("The baseline (quicksort on 1 thread/1 bucket):  completed in %g ms\n", baselineTime);
    printf("The best singlethreaded result:     %d buckets completed in %g ms\n", bestSingleThreadedBuckets, bestSingleThreadedTime);
    if (useMultiThreading) {
      printf("The best multithreaded result:      %d buckets completed in %g ms\n", bestMultiThreadedBuckets, bestMultiThreadedTime);
    }
    printf("\n-----------------------------------------------------------\n");

    testSpeedup(testNum++, correct, "singlethreaded vs baseline", (baselineTime / bestSingleThreadedTime), 1.2, 2);
    testSpeedup(testNum++, correct, "multithreaded vs baseline", (baselineTime / bestMultiThreadedTime), 1.4, 12);
    testSpeedup(testNum++, correct, "multithreaded vs singlethreaded", (bestSingleThreadedTime / bestMultiThreadedTime), 1.4, 8);

    printf("Note: The last two tests may fail on machines restricting to one core\n");
  }
  return testNum - 1 == correct;
}

int main(int argc, char** argv) {

  int test{ 0 };
  int count{ 0 };
  int smallSingleThreadedSorts{ false };
  int largeSingleThreadedSort{ false };
  int smallMultiThreadedSorts{ false };
  int allTests{ false };

  if (argc > 1) {
    if (strcmp(argv[1], "valgrind_mode") == 0) {
      // The user is running valgrind, don't run speed tests
      valgrind_mode = true;
      runSpeedTests = false;
      test = 0;
    }
    else {
      // CTest mode
      test = stoi(argv[1]);
      runSpeedTests = false;
    }
  }
  else {
    // Normal mode
    runSpeedTests = true;
  }

  switch (test) {
  case 0:
    if (testSmallSingleThreadedSorts()) {
      count++;
      smallSingleThreadedSorts = true;
    }
    if (testLargeSingleThreadedSort()) {
      count++;
      largeSingleThreadedSort = true;
    }
    if (testSmallMultiThreadedSorts()) {
      count++;
      smallMultiThreadedSorts = true;
    }
    if (testAll()) {
      count++;
      allTests = true;
    }

    cout << "----------------" << endl;
    if (!runSpeedTests) {
      cout << "No speed test summary in CTest or Valgrind mode" << endl;
    }
    cout << "Passed " << count << " out of 4 group tests" << endl;
    if (!smallSingleThreadedSorts) { cout << "Failed smallSingleThreadedSorts group tests" << endl; }
    if (!largeSingleThreadedSort) { cout << "Failed largeSingleThreadedSort group tests" << endl; }
    if (!smallMultiThreadedSorts) { cout << "Failed smallMultiThreadedSorts group tests" << endl; }
    if (!allTests) { cout << "Failed testAll group tests" << endl; }
    cout << "--End of tests--" << endl;
    return count != 4;
  case 1:
    return (testSmallSingleThreadedSorts() > 0) ? 0 : 1;
  case 2:
    return (testLargeSingleThreadedSort() > 0) ? 0 : 1;
  case 3:
    return (testSmallMultiThreadedSorts() > 0) ? 0 : 1;
  case 4:
    return (testAll() > 0) ? 0 : 1;
  }
}