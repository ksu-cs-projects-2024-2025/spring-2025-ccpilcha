
#define BLOCK_ID_TYPE uint8_t
#define CHUNK_X_SIZE 32
#define CHUNK_Y_SIZE 32
#define CHUNK_Z_SIZE 32
#define CHUNK_SIZE (CHUNK_X_SIZE * CHUNK_Y_SIZE * CHUNK_Z_SIZE)
#define CHUNK_POS_X_TYPE int64_t
#define CHUNK_POS_Y_TYPE int64_t
#define CHUNK_POS_Z_TYPE int64_t

#define CHUNK_DATA std::array<std::array<BLOCK_ID_TYPE, CHUNK_X_SIZE>, CHUNK_Y_SIZE>