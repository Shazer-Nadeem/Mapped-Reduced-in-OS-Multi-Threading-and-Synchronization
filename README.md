# MapReduce Framework Implementation in C++

## Overview
This project implements a basic MapReduce framework in C++ on a single machine, simulating distributed data processing using threads and inter-process communication.

## Contributors
- Muhammad Shahzer (i22-2043)
- Fasih-ur-Rehman (i22-1910)
- Saim Nadeem (i22-1884)
- Uzair Saddique (i22-6181)

## Table of Contents
- [Introduction](#introduction)
- [Approach and Design](#approach-and-design)
  - [Map Phase](#map-phase)
  - [Shuffle Phase](#shuffle-phase)
  - [Reduce Phase](#reduce-phase)
- [Flow/Activity Diagram](#flowactivity-diagram)
- [Code Explanation](#code-explanation)
  - [Mapper](#mapper)
  - [Reducer](#reducer)
- [Results](#results)
- [Conclusion](#conclusion)
- [Future Scope](#future-scope)

## Introduction
The MapReduce framework is a programming paradigm designed for large-scale data processing by breaking tasks into smaller, parallelizable components. The implementation consists of three main phases:
- **Map Phase**: Divides and processes input data into key-value pairs.
- **Shuffle Phase**: Groups data by keys.
- **Reduce Phase**: Aggregates results to produce the final output.

## Approach and Design

### Map Phase
1. The input text is divided into smaller chunks.
2. Threads are created to process each chunk concurrently.
3. Each thread extracts words and emits key-value pairs ("word", 1).
4. Key-value pairs are written to named pipes for inter-process communication.

### Shuffle Phase
1. The Reducer reads key-value pairs from named pipes.
2. Key occurrences are grouped into lists (e.g., `( "blue", [1, 1, 1] )`).
3. Data is grouped correctly before proceeding to the Reduce phase.

### Reduce Phase
1. The Reducer aggregates values for each key.
2. Summed values produce the final word count output.
3. The results are displayed to the user.

## Flow/Activity Diagram
Diagrams depicting the high-level flow of the framework and Mapper-Reducer interaction can be added here.

## Code Explanation

### Mapper
- **`make_chunks_function`**: Splits input text into chunks.
- **`thread_mapper_function`**: Processes chunks to generate key-value pairs and writes them to named pipes.
- **Thread Management**: Uses `pthread_create` for parallel execution and `pthread_join` for synchronization.

### Reducer
- **`pipe_read`**: Reads key-value pairs from named pipes.
- **`shuffle_function`**: Groups key-value pairs by key.
- **`reducer_function`**: Aggregates values for each unique key.

## Results
Example input:
```
red blue green yellow red green blue yellow blue green
Number of Chunks: 4
```
Example Mapper Output:
```
Writing to pipe pipe_0: red 1
Writing to pipe pipe_0: blue 1
Writing to pipe pipe_0: green 1
...
```
Example Reducer Output:
```
--- Shuffle Phase Results ---
Key: red, Value: 1 1
Key: blue, Value: 1 1 1
Key: green, Value: 1 1 1
Key: yellow, Value: 1 1

--- Reduce Phase ---
Key: red, Aggregated Value: 2
Key: blue, Aggregated Value: 3
Key: green, Aggregated Value: 3
Key: yellow, Aggregated Value: 2
```

## Conclusion
The project successfully implements a basic MapReduce framework in C++ on a single machine. Key achievements include:
- Efficient input data chunking.
- Proper synchronization and inter-process communication.
- Accurate word frequency aggregation.

## Future Scope
1. Extend the implementation to a distributed system with multiple nodes.
2. Optimize for handling larger datasets with advanced data structures.
3. Incorporate fault tolerance for thread and process failures.
