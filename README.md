Blobify
A lightweight C library for managing binary data blobs with cursor-based reading and writing operations.

The Idea
The idea of the program is to provide a structured approach to handling binary data in C. Blobify implements a three-layer architecture that allows for efficient and safe manipulation of raw byte data:

Block Layer - Raw binary data storage with memory management

Range Layer - A window or view into the block data that can be slid and resized

Cursor Layer - Position tracking within the range for sequential or random access

This design allows you to work with binary data similarly to a file stream, but with more control and flexibility. You can define a working range within your data, position a cursor within that range, and perform read/write operations - all with built-in safety checks.

Features
Memory-safe operations with comprehensive boundary checking

Cursor-based navigation for sequential and random access

Range management to work with subsets of data

Flexible stepping through data with adjustable step sizes

Fixed position support for read-only or restricted access patterns

Automatic memory management with create/delete functions

Block size limits to prevent excessive memory allocation (32MB max)

Usage
Basic Blob Operations
Create a blob with desired capacity

Move the cursor to your desired position

Read or write bytes at the cursor position

Delete the blob when done

Working with Ranges
Define a range window within your data

Slide the range forward or backward

Resize the range as needed

Check if values are within the range

Cursor Operations
Create a cursor at a specific offset

Step the cursor forward or backward

Jump directly to any position

Lock cursors in place when needed
