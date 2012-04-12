
/*

SDL_vnc.h - VNC client implementation

LGPL (c) A. Schiffler, aschiffler at ferzkopp dot net
Additions by B. Slawik, info at bernhardslawik dot de

*/

#ifndef _SDL_vnc_h
#define _SDL_vnc_h

#include <math.h>

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

#define TRACE_LAST_ERROR
#ifdef TRACE_LAST_ERROR
// For external debugging purposes (see compiler switch "TRA
char vncLastError[512];
#endif


#if defined(WIN32) || defined(WIN64)
#include <SDL.h>
#include <SDL_thread.h>
#else
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#endif

	/* ----- Versioning */

#define SDL_VNC_MAJOR	1
#define SDL_VNC_MINOR	0
#define SDL_VNC_MICRO	2

	/* ---- Defines */

#define VNC_BUFSIZE	1024

	/* ---- VNC Protocol Structures */

	/* ---- connection messages */

	typedef struct tSDL_vnc_pixelFormat {
		unsigned char bpp;
		unsigned char depth;
		unsigned char bigendian;
		unsigned char truecolor;
		unsigned short redmax;
		unsigned short greenmax;
		unsigned short bluemax;
		unsigned char redshift;
		unsigned char greenshift;
		unsigned char blueshift;
		unsigned char padding[3];
	} tSDL_vnc_pixelFormat;
	
	typedef struct tSDL_vnc_serverFormat {
		unsigned short width;
		unsigned short height;
		tSDL_vnc_pixelFormat pixel_format;
		unsigned int namelength;
		char name[VNC_BUFSIZE];
	} tSDL_vnc_serverFormat;

	/* --- server messages --- */

	typedef struct tSDL_vnc_updateRequest {
		unsigned char messagetype;
		unsigned char incremental;
		unsigned short x;
		unsigned short y;
		unsigned short w;
		unsigned short h;        
	} tSDL_vnc_updateRequest;

	typedef struct tSDL_vnc_serverMessage {
		unsigned char messagetype;
	} tSDL_vnc_serverMessage;

	typedef struct tSDL_vnc_serverUpdate {
		unsigned char padding;
		unsigned short rectangles;
	} tSDL_vnc_serverUpdate;

	typedef struct tSDL_vnc_serverRectangle {
		unsigned short x;
		unsigned short y;
		unsigned short width;
		unsigned short height;
		unsigned int encoding;
	} tSDL_vnc_serverRectangle;

	typedef struct tSDL_vnc_serverColormap {
		unsigned char padding;
		unsigned short first;
		unsigned short number;
	} tSDL_vnc_serverColormap;

	typedef struct tSDL_vnc_serverText {
		unsigned char padding[3];
		unsigned int length;
	} tSDL_vnc_serverText;

	typedef struct tSDL_vnc_serverCopyrect {
		unsigned short x;
		unsigned short y;
	} tSDL_vnc_serverCopyrect;

	typedef struct tSDL_vnc_serverRRE {
		unsigned int number;
		unsigned int background;
	} tSDL_vnc_serverRRE;

	typedef struct tSDL_vnc_serverRREdata {
		unsigned int color;
		unsigned short x;
		unsigned short y;
		unsigned short width;
		unsigned short height;
	} tSDL_vnc_serverRREdata;

	typedef struct tSDL_vnc_serverCoRRE {
		unsigned int number;
		unsigned int background;
	} tSDL_vnc_serverCoRRE;

	typedef struct tSDL_vnc_serverCoRREdata {
		unsigned int color;
		unsigned char x;
		unsigned char y;
		unsigned char width;
		unsigned char height;
	} tSDL_vnc_serverCoRREdata;

	typedef struct tSDL_vnc_serverHextile {
		unsigned char mode;
	} tSDL_vnc_serverHextile;

	typedef struct tSDL_vnc_serverHextileBg {
		unsigned int color;
	} tSDL_vnc_serverHextileBg;

	typedef struct tSDL_vnc_serverHextileFg {
		unsigned int color;
	} tSDL_vnc_serverHextileFg;

	typedef struct tSDL_vnc_serverHextileSubrects {
		unsigned char number;
	} tSDL_vnc_serverHextileSubrects;

	typedef struct tSDL_vnc_serverHextileColored {
		unsigned int color;
		unsigned char xy;
		unsigned char wh;
	} tSDL_vnc_serverHextileColored;

	typedef struct tSDL_vnc_serverHextileRect {
		unsigned char xy;
		unsigned char wh;
	} tSDL_vnc_serverHextileRect;

	/* ---- client messages ---- */

	typedef struct tSDL_vnc_clientKeyevent {
		unsigned char messagetype;
		unsigned char downflag;
		unsigned char padding[2];
		unsigned int  key;
	} tSDL_vnc_clientKeyevent;

	typedef struct tSDL_vnc_clientPointerevent {
		unsigned char messagetype;
		unsigned char buttonmask;
		unsigned short x;
		unsigned short y;
	} tSDL_vnc_clientPointerevent;
	
	/* ---- main SDL_vnc structure ---- */

	typedef struct tSDL_vnc {
		int socket;				// socket to server
		int versionMajor;				// current VNC version
		int versionMinor;				// current VNC version
		unsigned int security_type;		// current security type
		tSDL_vnc_serverFormat serverFormat;	// current server format
		tSDL_vnc_updateRequest updateRequest;	// standard update request for full screen 
		
		int reading;				// flag indicating we are reading
		int framerate;				// current framerate for update requests
		int delay;					// Throttle down main thread (power saving)
		
		Uint32 rmask, gmask, bmask, amask;	// current RGBA mask
		
		
		SDL_Thread *thread;			// VNC client thread
		SDL_mutex *mutex;			// thread mutex
		
		// Variables below are accessed by the Thread
		// and need to be mutex locked if accessed externally
		
		char *buffer;				// general IO buffer
		
		char *clientbuffer;			// buffer for client-to-server data
		int clientbufferpos;			// current position in buffer
		
		int fbupdated;				// flag indicating that the framebuffer was updated
		SDL_Rect updatedRect;			// rectangle that was updated
		
		SDL_Surface *framebuffer;		// RGB surface of framebuffer
		SDL_Surface *scratchbuffer;		// workbuffer for encodings
		SDL_Surface *tilebuffer;		// workbuffer for encodings
		
		int gotcursor;				// flag indicating that the cursor was updated
		SDL_Surface *cursorbuffer;		// RGBA surface of cursor (fixed at 32x32)
		SDL_Rect cursorhotspot;			// hotspot location of cursor (only .x and .y are used)
		
		
		SDL_Surface *lastTarget;
		int lastOutX;
		int lastOutY;
		float lastOutScale;
		
	} tSDL_vnc;


	/* ---- Prototypes */

	/* ---- Function Prototypes */

#if defined(WIN32) || defined(WIN64)
#  if defined(BUILD_DLL) && !defined(LIBSDL_VNC_DLL_IMPORT)
#    define SDL_VNC_SCOPE __declspec(dllexport)
#  else
#    ifdef LIBSDL_VNC_DLL_IMPORT
#      define SDL_VNC_SCOPE __declspec(dllimport)
#    endif
#  endif
#endif
#ifndef SDL_VNC_SCOPE
#  define SDL_VNC_SCOPE extern
#endif



	/* 
	Connect to VNC server 

	vnc  = pointer to tSDL_vnc structure
	host = hostname or hostip
	port = port
	mode = submode,submode,...
	submode =	raw | 
	copyrect | 
	rre | 
	corre(broken) |
	hextile | 
	zrle(unimplemented) | 
	cursor(ignored) | 
	desktop(ignored)
	password = text
	framerate = 1 to 100
	*/

	SDL_VNC_SCOPE int vncConnect(tSDL_vnc *vnc, char *host, int port, char *mode, char *password, int framerate);



	/* 
	Blit current framebuffer to target

	Only blits if framebuffer exists and was updated. 
	Updated region is stored in urec (if not NULL).

	Returns 1 if the blit occured, 0 otherwise.
	*/

	SDL_VNC_SCOPE void vncRefreshIntermediate();
	SDL_VNC_SCOPE int vncBlitFramebuffer(tSDL_vnc *vnc, SDL_Surface *target, SDL_Rect *urec);
	SDL_VNC_SCOPE int vncBlitFramebufferAdvanced(tSDL_vnc *vnc, SDL_Surface *target, SDL_Rect *urec, int outx, int outy, float outScale, int fullRefresh);
	tSDL_vnc *lastVnc;

	/*
	Blit current cursor to target
	
	Blitting is at the actual the cursor position.
	Returns 1 if blit occured, 0 otherwise 
	*/

	SDL_VNC_SCOPE int vncBlitCursor(tSDL_vnc *vnc, SDL_Surface *target, SDL_Rect *trec);



	/*
	Return cursor hotspot

	(Note: Only returned .x and .y are used.)
	*/

	SDL_VNC_SCOPE SDL_Rect vncCursorHotspot(tSDL_vnc *vnc);


	/*
	Send keyboard and pointer events to server
	*/
	SDL_VNC_SCOPE int vncClientKeyevent(tSDL_vnc *vnc, unsigned char downflag, unsigned int key);
	SDL_VNC_SCOPE int vncClientPointerevent(tSDL_vnc *vnc, unsigned char buttonmask, unsigned short x, unsigned short y);


	/* Disconnect from vnc server */

	SDL_VNC_SCOPE void vncDisconnect(tSDL_vnc *vnc);


	/* Ends C function definitions when using C++ */
#ifdef __cplusplus
};
#endif

#endif				/* _SDL_vnc_h */
