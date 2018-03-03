// Lab 6 - COMP3361 Winter 2018
//
// Implementation of three page replacement algorithms
//
// Code is provided to read a text file specifying
// the number of page frames and the trace (sequence) of page
// references. The main program will call each page
// replacement algorithm with the trace and an array
// representing the page frames and the page faults,
// to be filled in by the page replacement algorithm
// simulation.
//
// Code for you to implement is marked as "TODO"

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>
#include <string>

using std::cin;
using std::cout;
using std::cerr;
using std::ifstream;
using std::setw;
using std::string;
using std::vector;

namespace {

// ReadSystemConfig - read the number of page frames
//   and the reference trace from the input file.
//
// @param fileName name of the input file
// @param frameCount returns number of page frames
// @param trace returns trace of page references
void ReadSystemConfig(const char *fileName,
                      int &frameCount,
                      vector<int> &trace) {
  ifstream in;
  in.open(fileName);
  if (!in.is_open()) {
    cerr << "ERROR: input file not found: " << fileName << "\n";
    exit(2);
  }
  
  // Read the number of frames
  if (!(in >> frameCount)) {
    cerr << "ERROR: failed to read frameCount\n";
    exit(2);
  }
  if (frameCount < 1) {
    fprintf(stderr, "ERROR: frameCount < 1\n");
    exit(2);
  }
  cout << frameCount << " page frames\n";
  
  // Read the trace
  int next_ref;
  while (in >> next_ref) {
    trace.push_back(next_ref);
  } 
  
  if (!in.eof()) {
    cerr << "ERROR: failed to read trace\n";
    exit(2);
  }
}

// PrintArray - print a 2D array having the specified number
//   of rows and columns.
//
// @param a array to print
// @param nRows number of rows to print
// @param nCols number of columns to print
void PrintArray(const vector<int> &a, int nRows, int nCols) {
  for (int row = 0; row < nRows; ++row) {
    cout << "   ";
    for (int col = 0; col < nCols; ++col) {
      cout << " " << setw(2) << a.at(row*nCols + col);
    }
    cout << "\n";
  }
}

// RoundRobinPageReplacement - provides an example
//   of the interface for the page replacement
//   algorithm.  This implements a simple algorithm
//   which just picks the next page frame for
//   replacement.
//
// @param trace page reference trace
// @param traceIndex current time step in trace
// @param frameCount number of page frames in system
// @param frameUsage 2D array with frameCount rows
//                   and trace.size() columns
//                   containing frame usage up to the
//                   current time (traceIndex)
// @param accessTime a 1D array with an entry for each
//                   page frame containing the last time
//                   step (trace index) at which the frame
//                   was accessed
int RoundRobinPageReplacement(const vector<int> &trace,
                              int traceIndex,
                              int frameCount,
                              const vector<int> &frameUsage,
                              const vector<int> accessTime) {
  return traceIndex % frameCount;
}

// OptimalPageReplacement - use the optimal page replacement
//   strategy to pick a page to replace.
//
// @param trace page reference trace
// @param traceIndex current time step in trace
// @param frameCount number of page frames in system
// @param frameUsage 2D array with frameCount rows
//                   and trace.size() columns
//                   containing frame usage up to the
//                   current time (traceIndex)
// @param accessTime a 1D array with an entry for each
//                   page frame containing the last time
//                   step (trace index) at which the frame
//                   was accessed

int OptimalPageReplacement(const vector<int> &trace,
                           int traceIndex,
                           int frameCount,
                           const vector<int> &frameUsage,
                           const vector<int> accessTime) {
  int victimPageFrame = 0;
  // TODO: implement the page replacement strategy
  //       here, setting victimPageFrame to the frame
  //       number of the page to be replaced.
  
  return victimPageFrame;  // return the page frame to replace
}

// FIFOPageReplacement - use the FIFO page replacement
//   strategy to pick a page to replace.
//
// @param trace page reference trace
// @param traceIndex current time step in trace
// @param frameCount number of page frames in system
// @param frameUsage 2D array with frameCount rows
//                   and trace.size() columns
//                   containing frame usage up to the
//                   current time (traceIndex)
// @param accessTime a 1D array with an entry for each
//                   page frame containing the last time
//                   step (trace index) at which the frame
//                   was accessed
int FIFOPageReplacement(const vector<int> &trace,
                        int traceIndex,
                        int frameCount,
                        const vector<int> &frameUsage,
                        const vector<int> accessTime) {
  int victimPageFrame = 0;
  // TODO: implement the page replacement strategy
  //       here, setting victimPageFrame to the frame
  //       number of the page to be replaced.
  
  return victimPageFrame;  // return the page frame to replace
}

// LRUPageReplacement - use the LRU page replacement
//   strategy to pick a page to replace.
//
// @param trace page reference trace
// @param traceIndex current time step in trace
// @param frameCount number of page frames in system
// @param frameUsage 2D array with frameCount rows
//                   and trace.size() columns
//                   containing frame usage up to the
//                   current time (traceIndex)
// @param accessTime a 1D array with an entry for each
//                   page frame containing the last time
//                   step (trace index) at which the frame
//                   was accessed

int LRUPageReplacement(const vector<int> &trace,
                       int traceIndex,
                       int frameCount,
                       const vector<int> &frameUsage,
                       const vector<int> accessTime) {
  int victimPageFrame = 0;
  // TODO: implement the page replacement strategy
  //       here, setting victimPageFrame to the frame
  //       number of the page to be replaced.
  
  return victimPageFrame;  // return the page frame to replace
}

// ExecuteReferenceString - compute the usage of each
//   memory frame using the specified page replacement
//   algorithm.
//
// @param label name of algorithm to print out
// @param trace page reference trace
// @param frameCount number of page frames in system
// @param replacementAlgorithm function to use to calculate
//          page frame to replace
void ExecuteReferenceString(const string &label,
                            const vector<int> &trace, 
                            int frameCount,
                            int replacementAlgorithm(
                                    const vector<int> &trace,
                                    int traceIndex,
                                    int frameCount,
                                    const vector<int> &frameUsage,
                                    const vector<int> accessTime)) {
  int pageFaults = 0;  // count of page faults
  
  // Output array for page replacement algorithm. Each row
  // corresponds to a page frame, and each column corresponds
  // to one entry in the page reference trace.  Initialize to -1.
  vector<int> frameUsage(frameCount * trace.size(), -1);
  
  // Access time - array containing last access time (step number) for
  // each page frame. Initialized to -1.
  vector<int> accessTime(frameCount, -1);

  // Iterate through each time step in the trace (reference string)
  for (int t = 0; t < trace.size(); ++t) {
    bool inMemory = false;  // set to true if page already in memory
    int freeFrame = -1;     // set to number of first available frame
    // Check if page is in memory (also keep track of
    // free frame)
    for (int f = 0; f < frameCount; ++f) {
      int fi = f*trace.size() + t;  // index of next frame
      // if not first time step, copy previous frame
      if (t > 0) frameUsage[fi] = frameUsage[fi-1];
      
      if (frameUsage[fi] == trace[t]) {
        inMemory = true;  // frame we need is in memory
        accessTime[f] = t;
      } else if (freeFrame < 0 && frameUsage[fi] < 0) {
        freeFrame = f;   // if first free frame
      }
    }
    
    if (!inMemory) {
      ++pageFaults;
      if (freeFrame >= 0) {
        // Place page in free frame
        frameUsage[freeFrame*trace.size() + t] = trace[t];
        accessTime[freeFrame] = t;
      } else {
        // Choose frame to be replaced
        int replace = 
          (*replacementAlgorithm)(trace, t, frameCount, frameUsage, accessTime);
        if (replace >= 0 && replace < frameCount) {
          frameUsage[replace*trace.size() + t] = trace[t];  // replace frame
          accessTime[replace] = t;
        } else {
          cerr << "ERROR: at step " << t << ", " 
                  << label << " replacement algorithm returned frame "
                  << replace << ", frame count is " << frameCount << "\n";
          exit(2);
        }
      }
    }
  }
  
  cout << "\n" << label << " Page Replacement: (" 
          << pageFaults << " page faults)\n";
  PrintArray(trace, 1, trace.size());
  cout << "----------------------------------------------------------------------\n";
  PrintArray(frameUsage, frameCount, trace.size());
}

} // namespace

int main(int argc, char *argv[]) {
  // Read system configuration
  if (argc != 2) {
    cerr << "usage: " << argv[0] << " filename\n";
    exit(1);
  }
  
  vector<int> trace;  // page reference trace
  int frameCount = 0;    // number of page frames in system
  
  ReadSystemConfig(argv[1], frameCount, trace);
  
  // Compute page replacement with Round Robin algorithm
  // TODO: comment this out when you implement the other algorithms.
  ExecuteReferenceString("Round Robin", trace, frameCount,
                         RoundRobinPageReplacement);
  
  // Compute page replacement with FIFO algorithm
  ExecuteReferenceString("Optimal", trace, frameCount,
                         OptimalPageReplacement);
  
  // Compute page replacement with FIFO algorithm
  ExecuteReferenceString("FIFO", trace, frameCount,
                         FIFOPageReplacement);
  
  // Compute page replacement with LRU algorithm
  ExecuteReferenceString("LRU", trace, frameCount,
                         LRUPageReplacement);

  return 0;
}
