#include <NDL.h>
#include <SDL.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#define keyname(k) #k,
#define NR_KEYS ( sizeof(keyname)/sizeof(keyname[0]) )

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

static uint8_t keysnap[NR_KEYS] = {0};

int SDL_PushEvent(SDL_Event *ev) {
  todo();
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  #define BUF_LEN 16
  // 16 is the max length of the string representing a key event
  char buf[BUF_LEN];
  memset(buf,0,BUF_LEN);
  
  if(NDL_PollEvent(buf, BUF_LEN) == 0) return 0;
  if(strncmp(buf, "kd", 2) == 0 ) ev->type = SDL_KEYDOWN;
  else if(strncmp(buf, "ku", 2) == 0) ev->type = SDL_KEYUP;

  for(size_t i = 0; i < NR_KEYS; ++i){
    if(   ((strlen(buf + 3) - 1) == strlen(keyname[i])) \
       && (strncmp(buf + 3, keyname[i], strlen(keyname[i])) == 0) ) 
       {ev->key.keysym.sym = i; 
        keysnap[i] = (ev->type == SDL_KEYDOWN)? 1: 0;
        break; 
       }
  }
  
  return 1;
}

/* Wait for an event and store it in the event structure */
int SDL_WaitEvent(SDL_Event *event) {
  #define BUF_LEN 16
  char buf[BUF_LEN];
  memset(buf,0,BUF_LEN);
  
  while(NDL_PollEvent(buf, BUF_LEN) == 0){};
  if(strncmp(buf, "kd", 2) == 0 ) 
    event->type = SDL_KEYDOWN;
  else if(strncmp(buf, "ku", 2) == 0) 
    event->type = SDL_KEYUP;

  for(size_t i = 0; i < NR_KEYS; ++i){
    if(    ((strlen(buf + 3) - 1) == strlen(keyname[i]))  \
        && (strncmp(buf + 3, keyname[i], strlen(keyname[i])) == 0) ) 
      {event->key.keysym.sym = i; 
       keysnap[i] = (event->type == SDL_KEYDOWN)? 1: 0;
       break; 
      }
  }
  
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  todo();
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return keysnap;
}
