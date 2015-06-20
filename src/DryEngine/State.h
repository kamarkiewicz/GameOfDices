#ifndef DRY_STATE_H
#define DRY_STATE_H

typedef struct {
	void (*init) (void);
	void (*cleanup) (void);
	void (*pause) (void);
	void (*resume) (void);
	void (*handleEvents) (void);
	void (*update) (void);
	void (*draw) (void);
} drState;

#endif // DRY_STATE_H