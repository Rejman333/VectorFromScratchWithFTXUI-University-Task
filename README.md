# VectorFromScratchWithFTXUI

A C++ application built from scratch that combines a custom dynamic array (SmartArray<T>) with a full-featured terminal GUI using the FTXUI library.
The project includes a book database management system and demonstrates low-level memory management, object-oriented programming,
and modern C++ best practices—all without relying on STL containers like std::vector.

This project was created as part of a university assignment to implement a dynamic container and integrate it into a functional application.

### Features
* Custom Vector Implementation (SmartArray<T>)
  *    Manual memory management with move semantics
  *    Range-checked access .at(), front/back operations
  *    Custom growth strategy and capacity handling
  *    Copy/move constructors and assignment operators
  *    Element insertion, deletion, resize, reserve, shrink-to-fit

* Book Management App
  *    Add new books with terminal form input
  *    List all books in a formatted table view
  *    Save/load book data to/from a binary .bin file
  *    Compute simple statistics (e.g., best sellers)
  *    Full terminal interface using FTXUI
