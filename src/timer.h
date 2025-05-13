#ifndef __TIMER_H__
#define __TIMER_H__

typedef struct {
    void (*Init)(void);
    void (*Pause)(void);
    void (*Resume)(void);
    void (*Update)(void);
    float (*DeltaTime)(void);
} GameTimer;

extern GameTimer gTimer;

#endif