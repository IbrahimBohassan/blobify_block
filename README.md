##Blobify
A lightweight C library for managing binary data with cursor-based operations.

Overview
Blobify provides a three-layer architecture for working with binary data:

Block - Raw data storage

Range - Window into the block

Cursor - Position tracker within the range

Features
Memory-safe operations with boundary checking

Cursor-based read/write operations

Range management (slide, resize)

1-based indexing display

Arrow pointer to show current cursor position

API Functions
Function	Description
blb_blob_create(size, step)	Create a new blob
blb_blob_delete(blob)	Delete a blob
blb_blob_move(blob, offset)	Move cursor to offset
blb_blob_jump(blob, offset)	Alias for move
blb_blob_step(blob, step)	Step cursor by delta
blb_blob_put(blob, value)	Write value at cursor
blb_blob_get(blob, &value)	Read value at cursor
blb_blob_print(blob, stdout)	Print blob with cursor arrow
