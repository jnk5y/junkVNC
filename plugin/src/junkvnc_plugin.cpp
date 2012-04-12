/**
 * VNC plugin for webOS developed by John Kyrus
 *
 * Heavily based on
 * VNC Client for webOS
 * 2010 by Bernhard Slawik Digital, http://www.bernhardslawik.de
 *
 * Based on SDL_vnc by Andreas Schiffer, released under LGPL, see SDL_vnc/LICENSE
 *
 **/
// General Includes
#include <stdio.h>
#include <math.h>
#include <sstream>
using namespace std;

// PDK includes
//#include <GLES2/gl2.h>
#include "SDL.h"
#include "PDL.h"

// SDL_vnc includes
#ifdef WIN32
	#include <windows.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "sdlvnc/SDL_vnc.h"

// Pinch to zoom (experimental and unstable!)
//#define ALLOW_ZOOMING

SDL_Surface *Surface;               // Screen surface to retrieve width/height information

// VNC Stuff
char *vnc_server = strdup("192.168.1.1"); //NULL;
int   vnc_port = 5900;
char *vnc_method = strdup("hextile,rre,copyrect,raw");
char *vnc_password = strdup("");
int   vnc_framerate = 10;
tSDL_vnc vnc;
bool bConnect;

#include <fstream>
#include <iostream>
using namespace std;

void DoVNCLoop(SDL_Surface *screen, tSDL_vnc *vnc);

int toInt(string const & s) {
    stringstream ss;
    int val;
    ss << s;
    ss >> val;
    return val;
}

string toStr(int i) {
    stringstream ss;
    string str;
    ss << i;
    getline(ss, str);
    return str;
}

#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// SDL_vnc 1.0.0 includes
////////////////////////////////////////////////////////////////////////////////


// VNC Drawing loop
void DoVNCLoop(SDL_Surface *screen, tSDL_vnc *vnc) {
	SDL_Event event;
	SDL_Rect updateRect;
	Uint8 mousebuttons, buttonmask;
	int mousex, mousey;
	Uint32 key;
	bool keyMeta = false;

	int mtx = -1;
	int mty = -1;
	int mtb = 0;
	int mti = 0;
	
	// Multi Touch Variables
	int mtn = 0;
	int mtmaxn = 0;
	int mt2x = 0;
	int mt2y = 0;
	float mt2r = 0;
	int mtoldn = 0;
	int mt2oldx = 0;
	int mt2oldy = 0;
	float mt2oldr = 0;
	int mt2downx = 0;
	int mt2downy = 0;
	float mt2downr = 0;
	int ox = 0;
	int oy = 0;
	
	// Pinch/Zoom
	int outx = 0;
	int outy = 0;
	float outScale = 1.0;
	
	do {
		//bool needFlip = false;
		if(bConnect) {

			SDL_GL_SwapBuffers();

			while(SDL_PollEvent(&event)) {

				switch (event.type) {
					case SDL_VIDEORESIZE:

						//Surface = SDL_SetVideoMode(event.resize.w, event.resize.h, 0, SDL_SWSURFACE | SDL_RESIZABLE);
						screen = SDL_SetVideoMode(0,0, 0, SDL_SWSURFACE | SDL_RESIZABLE);
						if (Surface == NULL) {
							PDL_Quit();
							SDL_Quit();
							exit(1);
						}
						vncBlitFramebufferAdvanced(vnc, screen, &updateRect, outx, outy, outScale, 1);
						break;

				//	case SDL_KEYDOWN:
					case SDL_KEYUP:
						// Map SDL key to VNC key
						key = event.key.keysym.sym;
						
						// Handle non-alphabetic keys
						switch (event.key.keysym.sym) {
							case SDLK_BACKSPACE: key=0xff08; break;
							case SDLK_TAB: key=0xff09; break;
							case SDLK_RETURN: key=0xff0d; break;
							case SDLK_ESCAPE: key=0xff1b; break;
							case SDLK_INSERT: key=0xff63; break;
							case SDLK_DELETE: key=0xffff; break;
							case SDLK_HOME: key=0xff50; break;
							case SDLK_END: key=0xff57; break;
							case SDLK_PAGEUP: key=0xff55; break;
							case SDLK_PAGEDOWN: key=0xff56; break;
							case SDLK_LEFT: key=0xff51; break;
							case SDLK_UP: key=0xff52; break;
							case SDLK_RIGHT: key=0xff53; break;
							case SDLK_DOWN: key=0xff54; break;
							case SDLK_F1: key=0xffbe; break;
							case SDLK_F2: key=0xffbf; break;
							case SDLK_F3: key=0xffc0; break;
							case SDLK_F4: key=0xffc1; break;
							case SDLK_F5: key=0xffc2; break;
							case SDLK_F6: key=0xffc3; break;
							case SDLK_F7: key=0xffc4; break;
							case SDLK_F8: key=0xffc5; break;
							case SDLK_F9: key=0xffc6; break;
							case SDLK_F10: key=0xffc7; break;
							case SDLK_F11: key=0xffc8; break;
							case SDLK_F12: key=0xffc9; break;
							case SDLK_LSHIFT: key=0xffe1; break;
							case SDLK_RSHIFT: key=0xffe2; break;
							case SDLK_LCTRL: key=0xffe3; break;
							case SDLK_RCTRL: key=0xffe4; break;
							case SDLK_LMETA: key=0xffe7; break;
							case SDLK_RMETA: key=0xffe8; break;
							case SDLK_LALT: key=0xffe9; break;
							case SDLK_RALT: key=0xffea; break;
							default: break;
						}
						
						//	* SHIFT   = 1   (KMOD_SHIFT) = Shift
						//	* ORANGE  = 512              = Ctrl+Alt
						//	* SYM     = 128              = Ctrl
						//	* META    = KEY=231          = ---
						//	* BACK    =                  = Escape
						//	* FWD     = 229              = ??
						// Handle upper case letters.
						if (event.key.keysym.mod & KMOD_SHIFT) {
							key = toupper(key);
						}
						
						// Add client event
						vncClientKeyevent(vnc,SDL_KEYDOWN, key);
						break;

					case SDL_MOUSEBUTTONDOWN:
					case SDL_MOUSEBUTTONUP:
					case SDL_MOUSEMOTION:
					
						// Scan all fingers
						mt2x = 0;
						mt2y = 0;
						mtn = 0;
						mt2r = 0;
						mti = 0;
						while ((mti < 4) && (SDL_GetMultiMouseState(mti, &mtx, &mty) != 0)) {
					
							// That finger is down right now...
							mt2x += mtx;
							mt2y += mty;
							if (mtn > 0) {
								float dx = ox - mtx;
								float dy = oy - mty;
								mt2r += sqrt(dx*dx + dy*dy);
							}
							ox = mtx;
							oy = mty;
							mtn++;
							mti++;
						}

						// Handle Multi Touch gestures
						if (mtn > 1) {
							// Average touch center
							mt2x /= mtn;
							mt2y /= mtn;
							mt2r /= mtn;
						
							// Store Multi Touch Down State (when another finger presses)
							if (mtn > mtoldn) {
								mt2downx = mt2x;
								mt2downy = mt2y;
								mt2downr = mt2r;
								mt2oldx = mt2downx;
								mt2oldy = mt2downy;
								mt2oldr = mt2downr;
							}
						
							// Handle 2-Finger-Gestures
							if (mtn == 2) {
								// Pinch-to-zoom
								if (mt2downr > 5) {
									float zoomDelta = (mt2r / mt2downr) - (mt2oldr / mt2downr);
									//FIXME: Activate when Surface scales
#ifdef ALLOW_ZOOMING
									outScale = outScale + zoomDelta;
									if (outScale < 0.1) outScale = 0.1;
									else if (outScale > 4.0) outScale = 4.0;
#else
									// Zooming disabled
									outScale = 1.0;
#endif
								}
							
								// 2-Finger-Scroll
								int dx = mt2x - mt2oldx;
								int dy = mt2y - mt2oldy;
								outx += dx;
								outy += dy;
							
								// Clip
								//if (outx > screen->w/2) outx = screen->w/2;
								if (outx > 0) outx = 0;
								if (outx + vnc->framebuffer->w < screen->w) outx = screen->w - vnc->framebuffer->w;
							
//								if (outy > screen->h/2) outy = screen->h/2;
								if (outy > 0) outy = 0;
								if (outy + vnc->framebuffer->h < screen->h) outy = screen->h - vnc->framebuffer->h;
								
								// Quick redraw full screen
								vncBlitFramebufferAdvanced(vnc, screen, &updateRect, outx, outy, outScale, 1);
							}
							mt2oldx = mt2x;
							mt2oldy = mt2y;
							mt2oldr = mt2r;
						} // End of multi touch handling
						
						if (mtn == 0) {
							// Always keep mt-variables valid
							mt2x = mt2oldx;
							mt2y = mt2oldy;
							mt2r = mt2oldr;
						}
						
						if (mtn > mtmaxn) mtmaxn = mtn;

						// Single Touch: VNC Pointer Event
						// Filter out "start of multitouch": see if the last gesture was multitouch
						if ((mtn <= 1) && (mtmaxn <= 1)) {
							// Get current mouse state
							mousebuttons = SDL_GetMouseState(&mousex,&mousey);
							// Map SDL buttonmask to VNC buttonmask
							buttonmask = 0;
							if (mousebuttons & SDL_BUTTON(SDL_BUTTON_LEFT)) buttonmask       |= 1;
							if (mousebuttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) buttonmask     |= 2;
							if (mousebuttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) buttonmask      |= 4;
							if (mousebuttons & SDL_BUTTON(SDL_BUTTON_WHEELUP)) buttonmask    |= 8;
							if (mousebuttons & SDL_BUTTON(SDL_BUTTON_WHEELDOWN)) buttonmask  |= 16;
							// Add client event
							// Don't forget scale/offset!
							vncClientPointerevent(vnc, buttonmask, (mousex / outScale) - outx, (mousey / outScale) - outy);
						}
					
						if ((mtn == 0) && (mtoldn == 0)) mtmaxn = 0;
						mtoldn = mtn;
						break;

					default:
						break;
				
				}
			}
		}

		// Blit VNC screen
		if(bConnect) {
			if (vncBlitFramebufferAdvanced(vnc, screen, &updateRect, outx, outy, outScale, 0) ) {
			}
			vnc->delay = 0;
		}
		
		// Delay to limit rate
		SDL_Delay(1);
		
	} while (event.type != SDL_QUIT);
}

////////////////////////////////////////////////////////////////////////////////


#ifdef WIN32
extern "C"
#endif

void MakeConnection() {

	if(bConnect)
	{
		// Open vnc connection
		if (vncConnect(&vnc, vnc_server, vnc_port, vnc_method, vnc_password, vnc_framerate) == 0) {;
			const char *params[1];
			params[0] = "VNC Connection Failed";
			PDL_CallJS("pluginError", params, 1);
		}
	}
	else 
	{
		// Force double buffering
		//video_flags |= SDL_DOUBLEBUF;
		Surface = SDL_SetVideoMode(0, 0, 0, SDL_SWSURFACE | SDL_RESIZABLE);
		if (Surface == NULL) {
			PDL_Quit();
			SDL_Quit();
			exit(1);
		}

		// VNC Main Loop (Do all the drawing work)
		DoVNCLoop(Surface, &vnc);	
	}
		

}

////////////////////////////////////////////////////////////////////////////////
// Connect Function for WebOS hybrid app to call
////////////////////////////////////////////////////////////////////////////////

PDL_bool Connect(PDL_JSParameters *params) {

	vnc_server = strdup( PDL_GetJSParamString(params, 0) );
	vnc_port = toInt(PDL_GetJSParamString(params, 1));
	vnc_password = strdup( PDL_GetJSParamString(params, 2) );

	bConnect = true;

	MakeConnection();

	return PDL_TRUE;
}

PDL_bool Disconnect(PDL_JSParameters *params) {

	bConnect = false;

	vncDisconnect(&vnc);

	Surface = SDL_SetVideoMode(0, 0, 0, SDL_SWSURFACE | SDL_RESIZABLE);
	if (Surface == NULL) {
		PDL_Quit();
		SDL_Quit();
		exit(1);
	}

	return PDL_TRUE;
}

PDL_bool KeyPress(PDL_JSParameters *params) {

	Uint32 key;

	key = PDL_GetJSParamInt(params, 0);

	vncClientKeyevent(&vnc,SDL_KEYDOWN, key);
	
	return PDL_TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {

	// Title
	fprintf(stderr, "VNC Client PDK, (C) 2010 HotKey (Bernhard Slawik), info@bernhardslawik.de\n");
	fprintf(stderr, "Based on SDL_vnc by A. Schiffler (LGPL, http://sf.net/projects/sdlvnc, aschiffler at ferzkopp dot net)\n\n");
	
	// webOS init:
	// start the PDL library
	PDL_Init(0);	
	// start the SDL library. NOPARACHUTE = SDL provides "SegFault"-Catcher to clean up SDL on crash.
	int result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE);
	if (result != 0) {
		exit(1);
	}

	// register the js callback
	PDL_RegisterJSHandler("keypress", KeyPress);
	PDL_RegisterJSHandler("connect", Connect);
	PDL_RegisterJSHandler("disconnect", Disconnect);
	PDL_JSRegistrationComplete();
	PDL_CallJS("ready", NULL, 0);
	
	/*
	// Tell it to use OpenGL version 2.0
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	*/

	bConnect = false;
	MakeConnection();

	// Cleanup
    	PDL_Quit();
    	SDL_Quit();

	return 0;
}
