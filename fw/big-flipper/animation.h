#ifndef ANIMATION_H__
#define ANIMATION_H__

// Set the current animation running, 0 will be none. If the index is out of range, then it is set to zero.
//  If the animation changes, then it is reset. If not then this is a no-op. 
void animation_set(uint8_t idx);

// Rese tthe animation back to it's start
void animation_reset();

// Service the animation at whatever rate you want. 
void animation_service();

#endif // ANIMATION_H__
