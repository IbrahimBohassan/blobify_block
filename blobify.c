#include "blobify.h"

blb_block_t *blb_block_create(uint32_t size){
    if(size == 0 || (size > BLB_BLOCK_MAX_SIZE)) return NULL;

    blb_block_t *blk = (blb_block_t*)malloc(sizeof(blb_block_t));

    if(blk){
        blk->base = (uint8_t*)malloc(size);
        if(!blk->base){
            free(blk);
            return NULL;
        }
        blk->size = size;
        blk->allocated = true;

        memset(blk->base, 0, blk->size);
        return blk;
    }

    return NULL;
}

void blb_block_delete(blb_block_t *block){
    if(block && block->allocated){
        if(block->base){
            free(block->base);
        }
        free(block);
    }
}

bool blb_block_put(blb_block_t *block, int32_t offset, uint8_t value){
    if((!block) || (!block->base) || (offset < 0) || (offset >= block->size)){
        return false;
    }
    block->base[offset] = value;
    return true;
}

bool blb_block_get(blb_block_t *block, int32_t offset, uint8_t *value){
     if((!block) || (!block->base) || (!value) || (offset < 0) || (offset >= block->size)){
        return false;
    }
    *value = block->base[offset];
    return true;
}

void blb_block_print(blb_block_t *block, FILE *output){
    if((!block) || (!block->base) || (!output)){
        return;
    }

    for(int32_t i = 0; i < block->size; i++){
        fprintf(output, "%d: %02X\n", i, block->base[i]);
    }
}

blb_range_t *blb_range_create(uint32_t start, uint32_t size, uint8_t step, bool fixed){
    blb_range_t *range = (blb_range_t*)malloc(sizeof(blb_range_t));
    if(!range) return NULL;

    range->start = start;
    range->size = size;
    range->step = step;
    range->fixed = fixed;

    return range;
}

void blb_range_delete(blb_range_t *range){
    if(range) free(range);
}

bool blb_range_slide(blb_range_t *range, int32_t steps){
    if((!range) || (steps == 0) || (range->fixed)) return false;

    int64_t location = range->start + steps;
    if((location < 0)){
        return false;
    }

    range->start = (uint32_t)location;
    return true;
}

bool blb_range_resize(blb_range_t *range, int32_t size){
    if((!range) || (size == 0) || (range->fixed)) return false;

    int64_t new_size = range->size + size;
    if((new_size < 0)){
        return false;
    }

    range->size = (uint32_t)new_size;
    return true;
}

bool blb_range_in(blb_range_t *range, int32_t value){
    if((range)){
        uint32_t end = range->start + range->size;
        return (value >= range->start) && (value < end);
    }
    return false;
}

blb_cursor_t *blb_cursor_create(int32_t offset, bool fixed){
    blb_cursor_t *cursor = (blb_cursor_t*)malloc(sizeof(blb_cursor_t));
    if(!cursor) return NULL;

    cursor->offset = offset;
    cursor->fixed = fixed;
    return cursor;
}

void blb_cursor_delete(blb_cursor_t *cursor){
    if(cursor) free(cursor);
}

bool blb_cursor_step(blb_cursor_t *cursor, int32_t step){
    if((!cursor) || (step == 0) || (cursor->fixed)) return false;

    cursor->offset += step;
    return true;
}

bool blb_cursor_jump(blb_cursor_t *cursor, uint32_t value){
    if((!cursor) || (cursor->fixed)) return false;

    cursor->offset = value;
    return true;
}

blb_blob_t *blb_blob_create(uint32_t size, uint8_t step){
    blb_blob_t *blob = (blb_blob_t*)malloc(sizeof(blb_blob_t));
    if(blob){
        blob->block = blb_block_create(size);
        if(blob->block){
            blob->range = blb_range_create(0, size, step, false);
            if(blob->range){
                blob->cursor = blb_cursor_create(-1, false);
                if(blob->cursor) {
                    return blob;
                }
                blb_range_delete(blob->range);
            }
            blb_block_delete(blob->block);
        }
        free(blob);
    }
    return NULL;
}

void blb_blob_delete(blb_blob_t *blob){
    if(blob){
        if(blob->block) blb_block_delete(blob->block);
        if(blob->range) blb_range_delete(blob->range);
        if(blob->cursor) blb_cursor_delete(blob->cursor);
        free(blob);
    }
}

bool blb_blob_step(blb_blob_t *blob, int32_t step){
    if(blob && blb_range_in(blob->range, (blob->cursor->offset + step))){
        return blb_cursor_step(blob->cursor, step);
    }
    return false;
}

bool blb_blob_put(blb_blob_t *blob, uint8_t value) {
    if (!blob || !blob->block || !blob->range || !blob->cursor) {
        return false;
    }
    
    if (blob->cursor->offset < 0) {
        return false;
    }
    
    if (!blb_range_in(blob->range, blob->cursor->offset)) {
        return false;
    }
    
    return blb_block_put(blob->block, blob->cursor->offset, value);
}

bool blb_blob_get(blb_blob_t *blob, uint8_t *value) {
    if (!blob || !blob->block || !blob->range || !blob->cursor || !value) {
        return false;
    }
    
    if (blob->cursor->offset < 0) {
        return false;
    }
    
    if (!blb_range_in(blob->range, blob->cursor->offset)) {
        return false;
    }
    
    return blb_block_get(blob->block, blob->cursor->offset, value);
}

bool blb_blob_move(blb_blob_t *blob, uint32_t offset) {
    if (!blob || !blob->cursor || blob->cursor->fixed) {
        return false;
    }
    
    return blb_cursor_jump(blob->cursor, offset);
}

bool blb_blob_jump(blb_blob_t *blob, uint32_t offset) {
    return blb_blob_move(blob, offset);
}

bool blb_blob_set_range(blb_blob_t *blob, uint32_t start, uint32_t size) {
    if (!blob || !blob->range || blob->range->fixed) {
        return false;
    }
    
    if (start + size > blob->block->size) {
        return false;
    }
    
    blob->range->start = start;
    blob->range->size = size;
    
    if (blob->cursor && blob->cursor->offset >= 0) {
        if (!blb_range_in(blob->range, blob->cursor->offset)) {
            blob->cursor->offset = start;
        }
    }
    
    return true;
}

void blb_blob_print(blb_blob_t *blob, FILE *output) {
    if (!blob || !blob->block || !blob->range || !output) {
        return;
    }
    
    uint8_t value;
    int32_t cursor_pos = blob->cursor->offset;
    
    for (uint32_t i = blob->range->start; i < (blob->range->start + blob->range->size); i++) {
        blb_block_get(blob->block, i, &value);
        if (i == cursor_pos) {
            fprintf(output, "-> %d: %d\n", i + 1, value);
        } else {
            fprintf(output, "%d: %d\n", i + 1, value);
        }
    }
}

int main(void) {
    blb_blob_t *blob = blb_blob_create(8, 1);
    if (!blob) {
        return 1;
    }
    
    blb_blob_move(blob, 0);
    for (int i = 0; i < 8; i++) {
        blb_blob_put(blob, i);
        blb_blob_step(blob, 1);

    }
    
    blb_blob_jump(blob, 1);
    blb_blob_step(blob, 1);
    blb_blob_put(blob, 2);
    uint8_t value;
    blb_blob_get(blob, &value);
    blb_blob_print(blob, stdout);
    
    blb_blob_delete(blob);
    return 0;
}