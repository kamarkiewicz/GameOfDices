#ifndef DRY_TYPES_H
#define DRY_TYPES_H

#include <SFML/Graphics.h>

/// some casts
#define DRSTATE(val_) ((drState*)val_)

#define SFTEXTURE(val_) ((sfTexture*)val_)
#define SFFONT(val_) ((sfFont*)val_)
#define SFMUSIC(val_) ((sfMusic*)val_)
#define SFSPRITE(val_) ((sfSprite*)val_)
#define SFTEXT(val_) ((sfText*)val_)

typedef sfUint32* sfUSTR;
#define SFUSTR(val_) ((sfUSTR)val_)

#define DESTRUCTOR(val_) ((void (*)(void*))val_)
#define COMPARATOR(val_) ((int (*)(void*, void*))val_)

#endif // DRY_TYPES_H