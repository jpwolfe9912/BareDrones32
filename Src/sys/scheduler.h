#ifndef __SCHEDULER__
#define __SCHEDULER__

#define MASK_1000HZ (0x1 << 0U)
#define MASK_500HZ (0x1 << 1U)
#define MASK_200HZ (0x1 << 2U)
#define MASK_100HZ (0x1 << 3U)
#define MASK_50HZ (0x1 << 4U)
#define MASK_10HZ (0x1 << 5U)
#define MASK_5HZ (0x1 << 6U)
#define MASK_1HZ (0x1 << 7U)

#define TOTAL_LOOPS 8U

typedef enum
{
    FRAME_1000HZ = 0,
    FRAME_500HZ,
    FRAME_200HZ,
    FRAME_100HZ,
    FRAME_50HZ,
    FRAME_10HZ,
    FRAME_5HZ,
    FRAME_1HZ
} LoopFreqs_e;

typedef struct Tasks
{
    void (*task)(void);
    struct Tasks *next;
} Tasks;

/* Function Prototypes */
void run(Tasks **head_ref);

void push(Tasks **head_ref, void (*new_task)(void));
void insertAfter(Tasks *prev_node, void (*new_task)(void));
void append(Tasks **head_ref, void (*new_task)(void));

#endif